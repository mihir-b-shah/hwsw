
#ifndef CODE_INFORMER_H
#define CODE_INFORMER_H

#include <unordered_map>
#include <vector>
#include <initializer_list>

#include "code_info.h"

template<typename result_type>
class code_informer {
public:
  struct query {
    uint64_t trigger;
    std::initializer_list<uint64_t> iids; 

    query(uint64_t trigger, std::initializer_list<uint64_t> iids) 
      : trigger(trigger), iids(iids) {}
    virtual ~query(){}
    virtual void run_cb(const std::vector<result_type>& results) = 0;
  };

  template<typename query_cb>
  struct query_impl : query {
    query_cb cb;

    query_impl(uint64_t trigger, std::initializer_list<uint64_t> iid_list, query_cb cb) 
      : query(trigger, iid_list), cb(cb) {}
    ~query_impl(){}
    void run_cb(const std::vector<result_type>& results) override { cb(results); }
  };

public:
  code_informer() {}

  template <typename query_cb>
  void accept_query(uint64_t trigger, std::initializer_list<uint64_t> iid_list, query_cb cb){
    todo[trigger] = new query_impl<query_cb>(trigger, iid_list, cb);
  }

  void handle_retire(ooo_model_instr* instr){
    info.update(instr);

    auto todo_iter = todo.find(instr->instr_id);
    if(todo_iter == todo.end()){ return; }
    query* q = todo_iter->second;
    
    auto& iids = q->iids;
    results.resize(1+iids.size());
    results[0] = info.query_past(instr->instr_id);

    size_t loop_ctr = 0;
    for(uint64_t iid : iids){
      results[1+loop_ctr++] = info.query_past(iid);
    }
    q->run_cb(results);
    todo.erase(instr->instr_id);
    delete q;
  }
  
  static code_informer<result_type>* get_instance(){
    static code_informer<result_type> instance;
    return &instance;
  }

private:
  code_info_impl<result_type> info;
  std::unordered_map<uint64_t, query*> todo;
  std::vector<result_type> results;

};

#endif
