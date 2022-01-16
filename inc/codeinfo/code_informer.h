
#ifndef CODE_INFORMER_H
#define CODE_INFORMER_H

#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <tuple>

#include "code_info.h"
#include "loop_info.h"
#include "call_stack.h"
#include "llvm_info.h"

#define TYPE_EXPAND(V) std::tuple<        \
  V<call_stack>,                          \
  V<loop_info>                            \
>
typedef TYPE_EXPAND(std::vector) results_t;
#define GET_RESULT(results, type) std::get<std::vector<type>>(results)

typedef TYPE_EXPAND(code_info_impl) info_impls_t;

typedef std::vector<uint64_t> instr_ids_t;
typedef std::initializer_list<uint64_t> init_iid_t;
  
struct query {
  uint64_t trigger;
  instr_ids_t iids; 

  query(uint64_t trigger, init_iid_t iids) : trigger(trigger), iids(iids) {}
  virtual ~query(){}
  virtual void run_cb(const results_t& results) = 0;
};

template<typename query_cb>
struct query_impl : query {
  query_cb cb;

  query_impl(uint64_t trigger, init_iid_t iid_list, query_cb cb) 
    : query(trigger, iid_list), cb(cb) {}
  ~query_impl(){}
  void run_cb(const results_t& results) override { cb(results); }
};

class code_informer {
public:
  code_informer() {}

  template <typename query_cb>
  void accept_query(uint64_t trigger, init_iid_t iid_list, query_cb cb){
    todo[trigger] = new query_impl<query_cb>(trigger, iid_list, cb);
  }

  void handle_retire(ooo_model_instr* instr);
  
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
