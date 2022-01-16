
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>

#include <vector>
#include <cassert>

#include "loop_info.h"
#include "llvm_info.h"

template<size_t N>
class info_cache {
private:
  struct func_loop_info {
    llvm::Function* func;
    llvm::DominatorTree* dtree;
    llvm::LoopInfo* info;
      

    func_loop_info() : func(nullptr), dtree(nullptr), info(nullptr) {}
    func_loop_info(llvm::Function* f) : func(f) {
      this->dtree = new llvm::DominatorTree(*f);
      this->info = new llvm::LoopInfo(*dtree);
    }
      
    void destroy(){
      if(func != nullptr){
        assert(dtree != nullptr && info != nullptr);
        delete dtree;
        delete info;

        func = nullptr;
        dtree = nullptr;
        info = nullptr;
      }
    }
  };
  
public:
  info_cache() : info_cache_(N) {
    static_assert(N >= 1 && (N & -N) == N);
  }
  // make sure to respect lifetimes.
  func_loop_info get(llvm::Function* func){
    uintptr_t ptr = reinterpret_cast<uintptr_t>(func);
    func_loop_info& li_pos = info_cache_[ptr & (N-1)];
    if(li_pos.func != func){
      // reset it.
      li_pos.destroy();
      // put myself.
      li_pos = func_loop_info(func);
    }
    return li_pos;
  }

private:
  std::vector<func_loop_info> info_cache_;
};

static info_cache<16> cache;

loop_info code_info_impl<loop_info>::update_impl(
  llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){

  if(llvm_instrs != nullptr){
    llvm::Instruction* instr = llvm_instrs->back();

    llvm::LoopInfo* lp_info = cache.get(instr->getFunction()).info;
    llvm::Loop* lp = lp_info->getLoopFor(instr->getParent());

    int ctr = 0;
    while(lp != nullptr){
      lp = lp->getParentLoop();
      ++ctr;
    }
    return loop_info(ctr);
  } else {
    return loop_info();
  }
}
