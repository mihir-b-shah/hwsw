
#include "code_informer.h"

struct ci_helper {
  template<template<size_t I> typename F, size_t I, typename... Args> 
  struct iseq_run : iseq_run<F,I-1,Args...> {
    iseq_run(Args... args) : iseq_run<F,I-1,Args...>(args...) { F<I-1>::call(args...); }
  };

  template<template<size_t I> typename F, typename... Args>
  struct iseq_run<F,0,Args...> { iseq_run(Args... args) {} };

  template<size_t I> struct update_infos {
    static void call(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr, 
                     info_impls_t& infos){
      std::get<I>(infos).update(llvm_instrs, instr);
    }
  };
  typedef iseq_run<update_infos, std::tuple_size<info_impls_t>::value,
      llvm_info::inst_range*, ooo_model_instr*, info_impls_t&>
      update_infos_runner;

  template<size_t I> struct update_results {
    static void call(instr_ids_t& iids, llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr,
                     info_impls_t& infos, results_t& results){
      std::get<I>(results).resize(1+iids.size());
      std::get<I>(results)[0] = std::get<I>(infos).query_past(instr->instr_id);

      size_t loop_ctr = 0;
      for(uint64_t iid : iids){
        std::get<I>(results)[1+loop_ctr++] = std::get<I>(infos).query_past(iid);
      }
    }
  };
  typedef iseq_run<update_results, std::tuple_size<results_t>::value,
      instr_ids_t&, llvm_info::inst_range*, ooo_model_instr*,
      info_impls_t&, results_t&>
      update_results_runner;
};

void code_informer::handle_retire(ooo_model_instr* instr){
  llvm_info::inst_range* llvm_instrs = llvm_info::get_instance()->get_llvm_instrs(instr->ip);

  ci_helper::update_infos_runner(llvm_instrs, instr, infos);

  auto todo_iter = todo.find(instr->instr_id);
  if(todo_iter == todo.end()){ return; }
  query* q = todo_iter->second;
  
  ci_helper::update_results_runner(q->iids, llvm_instrs, instr, infos, results);

  q->run_cb(results);
  todo.erase(instr->instr_id);
  delete q;
}
