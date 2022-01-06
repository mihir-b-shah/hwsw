
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Triple.h>
#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/CodeGen/MachineModuleInfo.h>
#include <llvm/CodeGen/MIRParser/MIRParser.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstrDesc.h"
#include <llvm/Support/Host.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

#include <memory>
#include <utility>
#include <unordered_map>
#include <fstream>
#include <string>
#include <cinttypes>

using namespace llvm;

using objdump_info = std::unordered_map<std::string, 
  std::vector<std::pair<uint64_t, std::string>>>;

class llvm_data {
public:
  llvm_data(const char* fpath){
    SMDiagnostic err;
    InitializeAllTargets();
    InitializeAllTargetMCs();

    // nullptr is an optional IR handler, might find this useful later...
    std::unique_ptr<MIRParser> parser = createMIRParserFromFile(fpath, err, context, nullptr);
    std::unique_ptr<TargetMachine> tgt_machine;
    Triple tgt_triple;

    auto SetDataLayout = [&](StringRef triple) -> Optional<std::string> {
      tgt_triple.setTriple(sys::getDefaultTargetTriple());

      std::string Error;
      tgt = TargetRegistry::lookupTarget(codegen::getMArch(), tgt_triple, Error);
      
      std::string CPUStr = codegen::getCPUStr();
      std::string FeaturesStr = codegen::getFeaturesStr();

      //Options.MCOptions.PreserveAsmComments = PreserveComments;
      tgt_machine = std::unique_ptr<TargetMachine>(tgt->createTargetMachine(tgt_triple.getTriple(), 
        codegen::getCPUStr(), codegen::getFeaturesStr(), 
        codegen::InitTargetOptionsFromCodeGenFlags(tgt_triple), 
        codegen::getExplicitRelocModel()));

      return tgt_machine->createDataLayout().getStringRepresentation();
    };

    std::unique_ptr<Module> module = parser->parseIRModule(SetDataLayout);
    MachineModuleInfoWrapperPass* mmiwp = new MachineModuleInfoWrapperPass(
      static_cast<LLVMTargetMachine*>(tgt_machine.get()));
    parser->parseMachineFunctions(*module, mmiwp->getMMI());

    llvm_machine = static_cast<LLVMTargetMachine*>(tgt_machine.release());
    mod = module.release();
    mmi = &(mmiwp->getMMI());
    info = tgt->createMCInstrInfo();
  }

  void process_function(objdump_info& obj_map, const MachineFunction* mf, std::vector<std::pair<uint64_t, std::string>>& instr_list){
    int iptr = 0;
    int ct = 0;
    for(const MachineBasicBlock& mbb : *mf){
      errs() << "New basic block.\n";
      for(const MachineInstr& mi : mbb){
        if(mi.isMetaInstruction()){
          continue;
        }
        while(iptr < instr_list.size() && instr_list[iptr].second.find("nop") == 0){
          ++iptr;
        }
        if(iptr >= instr_list.size()){
          // panic
          errs() << "OUCH " << __LINE__ << " " << iptr << " " << instr_list.size() << "\n";
          mi.print(errs());
          return;
        }
        
        const MCInstrDesc& desc = mi.getDesc();
        errs() << instr_list[iptr].second << " " << info->getName(mi.getOpcode()) << " " << desc.isPseudo() << '\n';
        ++iptr;
      }
    }
    while(iptr < instr_list.size() && instr_list[iptr].second.find("nop") == 0){
      ++iptr;
    }
    if(iptr < instr_list.size()){
      // panic
      errs() << "OUCH " << __LINE__ << " " << iptr << " " << instr_list.size() << "\n";
      return;
    }
  }

  void write_mapping(objdump_info& obj_map){
    for(const Function& f : mod->getFunctionList()){
      outer: 
      const MachineFunction* mf = mmi->getMachineFunction(f);
      errs() << "Function: " << f.getName() << '\n';
      errs() << "MFunction: " << (mf==nullptr ? "NULL" : mf->getName()) << '\n';
      if(mf != nullptr){
        auto key = mf->getName().str();
        if(obj_map.find(key) == obj_map.end()){
          continue;
        }
        auto& instr_list = obj_map[key];
        errs() << "Wowy: " << mf->getName() << '\n';

        process_function(obj_map, mf, instr_list);
      }
    }
  }

private:
  LLVMContext context;
  const Target* tgt;
  MCInstrInfo* info;
  codegen::RegisterCodeGenFlags cgf;
  LLVMTargetMachine* llvm_machine;
  Module* mod;
  MachineModuleInfo* mmi;
};

objdump_info get_mapping(char* fname){
  objdump_info info;

  std::ifstream fin(fname);
  uint64_t addr;
  std::string func_name;
  std::string instr;
  while(fin >> std::hex >> addr >> func_name >> instr){
    info[func_name].push_back(std::make_pair(addr, instr));
  }

  /*
  for(auto iter = info.begin(); iter != info.end(); ++iter){
    const std::string& func_name_iter = iter->first;
    auto& v = iter->second;
    for(auto viter = v.begin(); viter != v.end(); ++viter){
      errs() << func_name_iter << " " << viter->first << " " << viter->second << '\n';
    }
  }
  */

  return info;
}

int main(int argc, char** argv){
  llvm_data meme(argv[1]);
  //auto result = get_mapping(argv[2]);
  //meme.write_mapping(result);
}
