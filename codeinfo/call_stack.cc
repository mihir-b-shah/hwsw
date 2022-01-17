
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/Support/Casting.h>

#include <string>
#include <algorithm>
#include <cassert>

#include "call_stack.h"
#include "llvm_info.h"

/*  note in the fixed-size call stack representation, we copy the top STK_SIZE
    most recent calls */
call_stack::call_stack(bool valid, const std::vector<llvm::Function*>& call_stk_v) : valid(valid) {
  auto fin = std::copy_n(call_stk_v.rbegin(), std::min(size_t(STK_SIZE), call_stk_v.size()),
    call_stk.begin());
  std::fill(fin, call_stk.end(), nullptr);
}

std::ostream& operator<<(std::ostream& os, const call_stack& cs){
  if(cs.valid){
    os << '[';
    for(auto iter = cs.call_stk.begin(); iter != cs.call_stk.end(); ++iter){
      os << (*iter == nullptr ? "NULL" : (*iter)->getName().str()) << ' ';
    }
    os << ']';
  } else {
    os << "[INVALID]";
  }
  return os;
}

static llvm::Function* prev_func = nullptr;
static std::vector<llvm::Function*> call_stk;

/*  whether the previous instruction was a call or ret, respectively */
static bool prev_call = false;
static bool prev_ret = false;

/*  an instruction range should DEFINETLY not span multiple functions,
    so we just pick the last instruction in the range (which is technically
    also the only one that maps to real asm) */
static llvm::Function* get_func(llvm_info::inst_range* llvm_instrs){
  if(llvm_instrs == nullptr || llvm_instrs->size() == 0){
    return nullptr;
  } else {
    return llvm_instrs->back()->getFunction();
  }
}

template<typename T>
static bool is_instr_type(llvm_info::inst_range* llvm_instrs){
  if(llvm_instrs == nullptr) return false;
  for(const llvm::Instruction* instr : *llvm_instrs){
    if(llvm::isa<T>(instr)) return true;
  }
  return false;
}
 
static std::string print_func(llvm::Function* fn){
  return fn != nullptr ? fn->getName().str() : "nilf";
}

call_stack code_info_impl<call_stack>::update_impl(
  llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){

  llvm::Function* curr_func = get_func(llvm_instrs);
  //std::cerr << std::hex << (instr->ip) << std::dec << " " << print_func(prev_func) << " -> " << print_func(curr_func) << '\n';
  bool valid = curr_func != nullptr;

  /*  2 states- either in user code or non-user code
      we only have info about user code.

      hence transitions as follows:
      code->code (normal)
      lib->lib (dont care)
      lib->code (only one that matters is main())
      code->lib (should come back to me)

      hence, if we don't care about seeing main (not too big deal, as a feature
      it is useless since everyone is from main), we can treat lib context
      as just straight-line code in the current context. */
  if(prev_func != nullptr && curr_func != nullptr && prev_func != curr_func){
    if(prev_call){
      call_stk.push_back(curr_func);
    } else if(prev_ret){
      if(!call_stk.empty()) call_stk.pop_back();
    } else {
      /*  I dont know- could be a jmp of some kind, just dont touch the call stack.
          Maybe we should assert(false) here, if we are sure we handled all cases? */
    }
  }

  prev_func = curr_func;
  prev_call = is_instr_type<llvm::CallInst>(llvm_instrs)
                && !is_instr_type<llvm::IntrinsicInst>(llvm_instrs);
  prev_ret = is_instr_type<llvm::ReturnInst>(llvm_instrs);

  /*
  if(llvm_instrs != nullptr){
    for(auto ins : *llvm_instrs){
      std::cerr << ins->getOpcodeName() << " ";
    }
  } else {
    std::cerr << "none ";
  }
  std::cerr << '[';
  for(auto iter = call_stk.begin(); iter != call_stk.end(); ++iter){
    std::cerr << (*iter == nullptr ? "NULL" : (*iter)->getName().str()) << ' ';
  }
  std::cerr << "]\n";
  */

  return call_stack(valid, call_stk);
}
  
