
#ifndef CODE_INFORMER_H
#define CODE_INFORMER_H

#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <tuple>

#include "code_info.h"
#include "llvm_info.h"
#include "call_stack.h"

#define TYPE_EXPAND(V) std::tuple<        \
  V<call_stack>                           \
>
typedef TYPE_EXPAND(std::vector) results_t;
typedef TYPE_EXPAND(code_info_impl) info_impls_t;

typedef std::vector<std::vector<uint64_t>> instr_ids_t;
  
struct query {
  uint64_t trigger;
  std::vector<std::vector<uint64_t>> iids; 

  query(uint64_t trigger, instr_ids_t& iids) 
    : trigger(trigger), iids(iids) {}
  virtual ~query(){}
  virtual void run_cb(const results_t& results) = 0;
};

template<typename query_cb>
struct query_impl : query {
  query_cb cb;

  query_impl(uint64_t trigger, instr_ids_t& iid_list, query_cb cb) 
    : query(trigger, iid_list), cb(cb) {}
  ~query_impl(){}
  void run_cb(const results_t& results) override { cb(results); }
};

class code_informer {
private:
  template<size_t I>
  void update_infos(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){
    std::get<I>(infos).update(llvm_instrs, instr);
  }

  template<size_t I>
  void update_results(instr_ids_t& iids, llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){
    std::get<I>(results).resize(1+iids[I].size());
    std::get<I>(results)[0] = std::get<I>(infos).query_past(instr->instr_id);

    size_t loop_ctr = 0;
    for(uint64_t iid : iids[I]){
      std::get<I>(results)[1+loop_ctr++] = std::get<I>(infos).query_past(iid);
    }
  }

public:
  code_informer() {}

  template <typename query_cb>
  void accept_query(uint64_t trigger, instr_ids_t& iid_list, query_cb cb){
    todo[trigger] = new query_impl<query_cb>(trigger, iid_list, cb);
  }

  void handle_retire(llvm_info::inst_range* llvm_instrs, ooo_model_instr* instr){
    update_infos<0>(llvm_instrs, instr);

    auto todo_iter = todo.find(instr->instr_id);
    if(todo_iter == todo.end()){ return; }
    query* q = todo_iter->second;
    
    auto& iids = q->iids;

    update_results<0>(iids, llvm_instrs, instr);

    q->run_cb(results);
    todo.erase(instr->instr_id);
    delete q;
  }
  
  static code_informer* get_instance(){
    static code_informer instance;
    return &instance;
  }

private:
  results_t results;
  info_impls_t infos;

  std::unordered_map<uint64_t, query*> todo;
};

#endif
