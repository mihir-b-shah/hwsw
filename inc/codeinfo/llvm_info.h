
#ifndef LLVM_INFO_H
#define LLVM_INFO_H

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <vector>
#include <cstdint>
#include <utility>

class llvm_info {
public:
  llvm_info();

  static llvm_info* get_instance(){
    static llvm_info instance;
    return &instance;
  }

  typedef llvm::SmallVector<llvm::Instruction*, 2> inst_range;
  inst_range& get_llvm_instr(uint64_t pc);

  uint64_t vaddr_offset(){ return addr_offset; }

private:
  llvm::LLVMContext context;
  llvm::Module* mod;
  uint64_t addr_offset;
  
  typedef std::pair<uint64_t, inst_range> map_info;
  std::vector<map_info> correl;
};

#endif