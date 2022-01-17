
#ifndef CALL_STACK_H
#define CALL_STACK_H

#include <llvm/IR/Function.h>

#include <array>
#include <vector>
#include <cinttypes>
#include <fstream>

#include "code_info.h"
#include "llvm_info.h"
  
/*  valid is unset if for example, we are running in library
    code, or something else, other than user code 

    we limit the size of the call stack feature, otherwise
    this was causing out-of-memory due to us storing the call
    stack produced by every instr_id (in code_info base class) */
struct call_stack {
  static constexpr size_t STK_SIZE = 10;

  bool valid;
  std::array<llvm::Function*,STK_SIZE> call_stk;

  call_stack() : valid(false), call_stk{nullptr} {}
  call_stack(bool valid, const std::vector<llvm::Function*>& call_stk_v);
  friend std::ostream& operator<<(std::ostream& os, const call_stack& cs);
};

template<>
class code_info_impl<call_stack> : public code_info<code_info_impl, call_stack> {
public:
  code_info_impl<call_stack>(){}
  call_stack update_impl(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr);
};

#endif
