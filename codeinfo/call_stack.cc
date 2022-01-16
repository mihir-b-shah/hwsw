
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Casting.h>

#include <string>
#include <algorithm>
#include <cassert>

#include "call_stack.h"
#include "llvm_info.h"

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
static bool prev_call = false;
static bool prev_ret = false;
static std::vector<llvm::Function*> call_stk;
static bool valid = false;

llvm::Function* get_func(llvm_info::inst_range* llvm_instrs){
  if(llvm_instrs == nullptr || llvm_instrs->size() == 0){
    return nullptr;
  } else {
    return llvm_instrs->back()->getFunction();
  }
}

template<typename T>
bool is_instr_type(llvm_info::inst_range* llvm_instrs){
  if(llvm_instrs == nullptr) return false;
  for(const llvm::Instruction* instr : *llvm_instrs){
    if(llvm::isa<T>(instr)) return true;
  }
  return false;
}
 
std::string print_func(llvm::Function* fn){
  return fn != nullptr ? fn->getName().str() : "nilf";
}

call_stack code_info_impl<call_stack>::update_impl(
  llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){

  llvm::Function* curr_func = get_func(llvm_instrs);
  valid = curr_func != nullptr;
  // std::cerr << std::hex << (instr->ip - llvm_info::get_instance()->vaddr_offset()) << std::dec << " " << print_func(prev_func) << " -> " << print_func(curr_func) << '\n';
  if(prev_func != nullptr && curr_func != nullptr && prev_func != curr_func){
    if(prev_call){
      call_stk.push_back(curr_func);
    } else if(prev_ret){
      if(!call_stk.empty()) call_stk.pop_back();
    } else {
      // i dont know- could be a jmp of some kind, just dont touch the call stack.
    }
  }

  /*
  2 states- either in user code or non-user code
  we only have info about user code.

  hence transitions as follows:
  code->code (normal)
  lib->lib (dont care)
  lib->code (only one that matters is main())
  code->lib (should come back to me)

  hence, if we don't care about seeing main (not too big deal, as a feature
  it is useless since everyone is from main), we can treat lib context
  as just straight-line code in the current context.
  */

  prev_func = curr_func;
  prev_call = is_instr_type<llvm::CallInst>(llvm_instrs);
  prev_ret = is_instr_type<llvm::ReturnInst>(llvm_instrs);

  /*
  if(llvm_instrs != nullptr){
    std::cerr << llvm_instrs->back()->getOpcodeName() << " ";
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
  
