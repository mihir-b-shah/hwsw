
#ifndef CODEINFO_H
#define CODEINFO_H

#include <cinttypes>
#include <unordered_map>
#include <iostream>

#include "instruction.h"
#include "llvm_info.h"

template <template <typename> class impl, typename result_type>
class code_info {
protected:
  std::unordered_map<uint64_t, result_type> past_results;
  uint64_t past_ip;

public:
  code_info() : past_ip(0) {}
  virtual ~code_info(){}

  void update(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){
    if(instr->ip != past_ip){
      past_results[instr->instr_id] = static_cast<impl<result_type>*>(this)
        ->update_impl(llvm_instrs, instr);
      past_ip = instr->ip;
    }
  }

  result_type query_past(uint64_t instr_id){
    return past_results[instr_id];
  }
};

template <typename result_type>
class code_info_impl;

#endif
