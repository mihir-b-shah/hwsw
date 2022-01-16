
#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include <cinttypes>

#include "code_info.h"
#include "llvm_info.h"
  
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
