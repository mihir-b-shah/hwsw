
#ifndef CODEINFO_H
#define CODEINFO_H

#include <cinttypes>
#include <vector>
#include <unordered_map>
#include <initializer_list>

#include "instruction.h"

enum info_type {
  CALL_STACK
};

struct query {
  info_type type;
  std::vector<uint64_t> iids;
 
  query(info_type type, std::initializer_list<uint64_t> iid_list) : type(type), iids(iid_list) {}
  virtual ~query(){}
  virtual void run_cb(std::vector<void*>&) = 0;
};

template<typename query_cb>
struct query_impl : query {
  query_cb cb;

  query_impl(info_type type, std::initializer_list<uint64_t> iid_list, query_cb cb) 
    : query(type, iid_list), cb(cb) {}
  void run_cb(std::vector<void*>& v) override { cb(v); }
};

class code_info {
protected:
  std::unordered_map<uint64_t, void*> past_results;

public:
  code_info();
  virtual ~code_info(){}

  virtual void* update_inner(ooo_model_instr* instr) = 0;
  void update(ooo_model_instr* instr);
  void* query_past(uint64_t instr_id);
};

class code_informer {
private:
  std::vector<code_info*> infos;
  std::unordered_map<uint64_t, query*> todo;

public:
  code_informer();

  template<typename query_cb>
  void accept_query(info_type type, uint64_t trigger, std::initializer_list<uint64_t> iids, query_cb cb){
    todo[trigger] = new query_impl<query_cb>(type, iids, cb);
  }

  void handle_retire(ooo_model_instr* instr);
  
  static code_informer* get_instance();
};

#endif
