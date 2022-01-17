
#ifndef CODEINFO_H
#define CODEINFO_H

#include <cinttypes>
#include <unordered_map>
#include <iostream>

#include "instruction.h"
#include "llvm_info.h"

/*  Base class for any kind of code information. We follow
    the CRTP to get static polymorphism here. We pass the impl
    and result type separately since we can use result_type
    separately from the derived type. Technically we can use a
    typedef from the derived class, but this feels cleaner 

    Note- CRTP is the curiously recurring template patern. 

    As described in documentation, we want the ability to 
    1) update on an executed instruction (sequentially), and
    2) query past results it produced. The latter is common
       functionality for all code_infos, so we have the past
       results map defined here. */
template <template <typename> class impl, typename result_type>
class code_info {
protected:
  std::unordered_map<uint64_t, result_type> past_results;
  uint64_t past_ip;

public:
  code_info() : past_ip(0) {}
  virtual ~code_info(){}

  /*  sometimes, due to Champsim's execution effects, the ip being executed
      in a loop may be the same, so we want to make sure we are removing
      duplicates in the stream. */
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

/* the derived class, which each code_info implementation specializes */
template <typename result_type>
class code_info_impl;

#endif
