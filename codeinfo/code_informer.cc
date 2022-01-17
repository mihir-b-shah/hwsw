
#include "code_informer.h"

/*  This is a lot of template voodoo, but the interfaces
    to use it are hopefully simple. 
    
    Motivation: we have our results_t and info_impls_t, from
    the code_informer, which are both tuples. For both, we want
    to run a function on each element of the tuple.
    Since tuples can only be indexed at compile time, we need
    the function to take a template index parameter.

    Hence, we want a way to generate a series of calls for a function,
    e.g. F<0>(...); F<1>(...); etc. */

namespace ci_helper {

/*  iseq_run, when instantiated, generates calls to F for each templated index
    from 0 to the 'I' it is given, minus 1. This occurs by delegating successive
    invocations to the constructors, which by C++ rules run in reverse order. 
    Also note, args are arbitrary, so we can run an arbitrary function. Maybe
    a lambda could be used instead, but I couldn't figure out how to do that. */
template<template<size_t I> typename F, size_t I, typename... Args> 
struct iseq_run : iseq_run<F,I-1,Args...> {
  iseq_run(Args... args) : iseq_run<F,I-1,Args...>(args...) { F<I-1>::call(args...); }
};

/*  base case of iseq_run, stop here. */
template<template<size_t I> typename F, typename... Args>
struct iseq_run<F,0,Args...> { iseq_run(Args... args) {} };

/*  note, this must be static- because any instance variables would mean
    we would need to pass the struct itself, and not the type- which
    would not work. As such, it takes as parameter infos, which is an instance
    variable of code_informer. */
template<size_t I> struct update_infos {
  static void call(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr, 
                   info_impls_t& infos){
    std::get<I>(infos).update(llvm_instrs, instr);
  }
};
/*  instantiating this object will be equivalent to calling update,
    on each impl in the tuple. */
typedef iseq_run<update_infos, std::tuple_size<info_impls_t>::value,
    llvm_info::inst_range*, ooo_model_instr*, info_impls_t&>
    update_infos_runner;

template<size_t I> struct update_results {
  static void call(instr_ids_t& iids, llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr,
                   info_impls_t& infos, results_t& results){
    /*  results, as described in documentation, is formatted so position 0
        has the results for the instruction the query originated from, and then
        the other past ones line up. Critically, results is reused between
        answering queries- so be careful with lifetimes. */
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

}

/*  Since this is the overall interface for the ooo_cpu to give the retired instruction
    for code infos to update, we just take the instr as parameter, and go fetch the
    corresponding llvm info, and call updates using the ci_helper(s). */
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
