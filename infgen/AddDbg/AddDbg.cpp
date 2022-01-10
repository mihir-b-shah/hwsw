
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassInstrumentation.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"

#include <unordered_map>
#include <cassert>

#define DEBUG_TYPE "debugify"

using namespace llvm;

namespace {

uint64_t getAllocSizeInBits(Module &M, Type *Ty) {
  return Ty->isSized() ? M.getDataLayout().getTypeAllocSizeInBits(Ty) : 0;
}

bool isFunctionSkipped(Function &F) {
  return F.isDeclaration(); // || !F.hasExactDefinition();
}

/// Find the basic block's terminating instruction.
///
/// Special care is needed to handle musttail and deopt calls, as these behave
/// like (but are in fact not) terminators.
Instruction *findTerminatingInstruction(BasicBlock &BB) {
  if (auto *I = BB.getTerminatingMustTailCall())
    return I;
  if (auto *I = BB.getTerminatingDeoptimizeCall())
    return I;
  return BB.getTerminator();
}
} // end anonymous namespace

template<typename T>
constexpr unsigned myMask(){ return 1 << (8*sizeof(T)-1); }

bool applyDebugifyMetadata(
    Module &M, iterator_range<Module::iterator> Functions, StringRef Banner,
    std::function<bool(DIBuilder &DIB, Function &F)> ApplyToMF) {
  DIBuilder DIB(M);
  LLVMContext &Ctx = M.getContext();
  auto *Int32Ty = Type::getInt32Ty(Ctx);
    
  // Get a DIType which corresponds to Ty.
  DenseMap<uint64_t, DIType *> TypeCache;
  auto getCachedDIType = [&](Type *Ty) -> DIType * {
    uint64_t Size = getAllocSizeInBits(M, Ty);
    DIType *&DTy = TypeCache[Size];
    if (!DTy) {
      std::string Name = "ty" + utostr(Size);
      DTy = DIB.createBasicType(Name, Size, dwarf::DW_ATE_unsigned);
    }
    return DTy;
  };

  auto File = DIB.createFile(M.getName(), "/");
  auto CU = DIB.createCompileUnit(dwarf::DW_LANG_C, File, "debugify",
                                  /*isOptimized=*/true, "", 0);

  unsigned NextLine = 0;
  unsigned NextVar = 0;

  // Visit each global.
  unsigned glb_ctr = 1;
  for(auto& global : M.getGlobalList()){
    global.addDebugInfo(DIB.createGlobalVariableExpression(CU, global.getName(),
    "", File, myMask<unsigned int>() | glb_ctr, getCachedDIType(global.getType()), true));
    ++glb_ctr;
  }

  // Visit each instruction.
  for (Function &F : Functions) {
    if(isFunctionSkipped(F)){
      continue;
    }

    bool InsertedDbgVal = false;
    auto SPType = DIB.createSubroutineType(DIB.getOrCreateTypeArray(None));
    DISubprogram::DISPFlags SPFlags =
        DISubprogram::SPFlagDefinition | DISubprogram::SPFlagOptimized;
    if (F.hasPrivateLinkage() || F.hasInternalLinkage())
      SPFlags |= DISubprogram::SPFlagLocalToUnit;
    auto SP = DIB.createFunction(CU, F.getName(), F.getName(), File, NextLine,
                                 SPType, NextLine, DINode::FlagZero, SPFlags);
    F.setSubprogram(SP);
    
    struct instr_id {
      unsigned bb;
      unsigned short instr;

      instr_id() : bb(0), instr(0) {}
      instr_id(unsigned bb, unsigned short instr) : bb(bb), instr(instr) {}
    };
    std::unordered_map<llvm::Instruction*, instr_id> imap;

    // Helper that inserts a dbg.value before \p InsertBefore, copying the
    // location (and possibly the type, if it's non-void) from \p TemplateInst.
    auto insertDbgVal = [&](Instruction &TemplateInst,
                            Instruction *InsertBefore) {
      assert(imap.find(&TemplateInst) != imap.end());
      auto info = imap[&TemplateInst];
      std::string Name = utostr(info.bb);
      Value *V = &TemplateInst;
      if (TemplateInst.getType()->isVoidTy())
        V = ConstantInt::get(Int32Ty, 0);
      const DILocation* loc = TemplateInst.getDebugLoc().get();
      auto LocalVar = DIB.createAutoVariable(SP, Name, File, info.instr,
                                             getCachedDIType(V->getType()),
                                             /*AlwaysPreserve=*/true);
      DIB.insertDbgValueIntrinsic(V, LocalVar, DIB.createExpression(), loc,
                                  InsertBefore);
    };

    unsigned bbCtr = 1;
    for (BasicBlock &BB : F) {
      // Attach debug locations.
      unsigned iCtr = 1;
      for (Instruction &I : BB){
        imap[&I] = instr_id(bbCtr, iCtr);
        ++iCtr;
      }
      ++bbCtr;
    }

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        instr_id loc_info = imap[&I];
        I.setDebugLoc(DILocation::get(Ctx, myMask<unsigned int>() | (loc_info.bb), 
          myMask<unsigned short>() | (loc_info.instr), SP));
      }

      // Inserting debug values into EH pads can break IR invariants.
      if (BB.isEHPad())
        continue;

      // Find the terminating instruction, after which no debug values are
      // attached.
      Instruction *LastInst = findTerminatingInstruction(BB);
      assert(LastInst && "Expected basic block with a terminator");

      // Maintain an insertion point which can't be invalidated when updates
      // are made.
      BasicBlock::iterator InsertPt = BB.getFirstInsertionPt();
      assert(InsertPt != BB.end() && "Expected to find an insertion point");
      Instruction *InsertBefore = &*InsertPt;

      // Attach debug values.
      for (Instruction *I = &*BB.begin(); I != LastInst; I = I->getNextNode()) {
        // Skip void-valued instructions.
        if (I->getType()->isVoidTy())
          continue;

        // Phis and EH pads must be grouped at the beginning of the block.
        // Only advance the insertion point when we finish visiting these.
        if (!isa<PHINode>(I) && !I->isEHPad())
          InsertBefore = I->getNextNode();

        insertDbgVal(*I, InsertBefore);
        InsertedDbgVal = true;
      }
    }

    // Add params
    unsigned arg_ctr = 1;
    for(Argument& arg : F.args()){
      // don't care about line here
      auto dParam = DIB.createParameterVariable(SP, arg.getName(), arg_ctr, File, 
        0, getCachedDIType(arg.getType()), true);
      ++arg_ctr;
      
      Instruction* firstInstr = F.front().getFirstNonPHIOrDbgOrLifetime();
      DIB.insertDbgValueIntrinsic(&arg, dParam, DIB.createExpression(), 
        DILocation::get(F.getParent()->getContext(), 0, 0, SP), firstInstr);
    }

    // Make sure we emit at least one dbg.value, otherwise MachineDebugify may
    // not have anything to work with as it goes about inserting DBG_VALUEs.
    // (It's common for MIR tests to be written containing skeletal IR with
    // empty functions -- we're still interested in debugifying the MIR within
    // those tests, and this helps with that.)
    if (!InsertedDbgVal) {
      auto *Term = findTerminatingInstruction(F.getEntryBlock());
      insertDbgVal(*Term, Term);
    }
    if (ApplyToMF)
      ApplyToMF(DIB, F);
    DIB.finalizeSubprogram(SP);
  }
  DIB.finalize();

  // Track the number of distinct lines and variables.
  NamedMDNode *NMD = M.getOrInsertNamedMetadata("llvm.debugify");
  auto addDebugifyOperand = [&](unsigned N) {
    NMD->addOperand(MDNode::get(
        Ctx, ValueAsMetadata::getConstant(ConstantInt::get(Int32Ty, N))));
  };
  addDebugifyOperand(NextLine - 1); // Original number of lines.
  addDebugifyOperand(NextVar - 1);  // Original number of variables.
  assert(NMD->getNumOperands() == 2 &&
         "llvm.debugify should have exactly 2 operands!");

  // Claim that this synthetic debug info is valid.
  StringRef DIVersionKey = "Debug Info Version";
  if (!M.getModuleFlag(DIVersionKey))
    M.addModuleFlag(Module::Warning, DIVersionKey, DEBUG_METADATA_VERSION);

  return true;
}

/// ModulePass for attaching synthetic debug info to everything, used with the
/// legacy module pass manager.
struct AddDbg : public ModulePass {
  bool runOnModule(Module &M) override {
    bool res = applyDebugifyMetadata(M, M.functions(), "ModuleDebugify: ", nullptr);
    return res;
  }

  AddDbg() : ModulePass(ID) {}
  void getAnalysisUsage(AnalysisUsage &AU) const override { AU.setPreservesAll(); }
  static char ID;
};

char AddDbg::ID = 0;
static RegisterPass<AddDbg> DM("adddbg", "Attach debug info to everything");
