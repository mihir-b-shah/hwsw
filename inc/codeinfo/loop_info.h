
#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include <cinttypes>

#include "code_info.h"
#include "llvm_info.h"
  
/*  loop_depth is -1 for invalid info, typically if we're not
    in user code.
    right now, this is not very useful information, but if this
    type of info is useful, a lot more could be added- characteristics
    of the induction variable, etc. from the LLVM loop class */
struct loop_info {
  int loop_depth;

  loop_info() : loop_depth(-1) {}
  loop_info(int depth) : loop_depth(depth) {}
};

template<>
class code_info_impl<loop_info> : public code_info<code_info_impl, loop_info> {
public:
  code_info_impl<loop_info>(){}
  loop_info update_impl(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr);
};

#endif
