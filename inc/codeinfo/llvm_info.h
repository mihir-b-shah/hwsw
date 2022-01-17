
#ifndef LLVM_INFO_H
#define LLVM_INFO_H

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <utility>

/*  An interface to llvm information, that the code_informer accesses
    and then passes on through the successive update methods to code_info's. 
    For now, it is responsible for reading ALL files in the info/ folder
    corresponding to our trace. */
class llvm_info {
public:
  llvm_info();

  static llvm_info* get_instance(){
    static llvm_info instance;
    return &instance;
  }

  typedef llvm::SmallVector<llvm::Instruction*, 2> inst_range;
  /*  Note, pc is NOT address adjusted, vaddr_offset() is called internally. */
  inst_range* get_llvm_instrs(uint64_t pc);

  /*  The offset between instruction (and likely other sections, like .bss)
      addresses in the ELF file and at runtime, in the trace. Loaded from the
      info/<trace_name>.offset file. */
  uint64_t vaddr_offset(){ return addr_offset; }

private:
  /*  It's very important the context stays here and thereby has lifetime
      from first use to the end of the program- otherwise referencing
      LLVM data structures may not work. */
  llvm::LLVMContext context;
  llvm::Module* mod;
  uint64_t addr_offset;
  
  /*  correl allows us to know starting at a particular pc, what range
      of llvm instructions exist. The result of fusing the .ll and .crl
      files- see the llvm_info.cc file. */
  typedef std::pair<uint64_t, inst_range> map_info;
  std::vector<map_info> correl;

  /*  for each function, the pc at which it starts and ends- from the .crl file */
  std::unordered_map<llvm::Function*, std::pair<uint64_t, uint64_t>> func_starts;
};

#endif
