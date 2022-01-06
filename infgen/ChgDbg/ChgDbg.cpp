
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

template<typename T>
constexpr unsigned myMask(){ return 1 << (8*sizeof(T)-1); }

void visitor(Function &F) {
  unsigned bbCtr = 0;
  for(BasicBlock& bb : F){
    unsigned iCtr = 0;
    for(Instruction& i : bb){
      auto& loc = i.getDebugLoc();
      if(loc.get() != nullptr){
        auto new_loc = DILocation::get(F.getParent()->getContext(), myMask<unsigned int>() | (1+bbCtr), myMask<unsigned short>() | (1+iCtr), loc.getScope(), loc.getInlinedAt());
        i.setDebugLoc(new_loc);
      }
      ++iCtr;
    }
    ++bbCtr;
  }
}

struct ChgDbg : public FunctionPass {
  static char ID;
  ChgDbg() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override {
    visitor(F);
    return false;
  }
};

}

char ChgDbg::ID = 0;
static RegisterPass<ChgDbg> X("chgdbg", "Change Debug Pass");
