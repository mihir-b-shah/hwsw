
#include "code_info.h"
#include "call_stack.h"
#include <iostream>

/****************** code_info *****************/

code_info::code_info(){
  // nothing for now.
}

void code_info::update(ooo_model_instr* instr){
  static uintptr_t past_ip = 0;
  if(instr->ip != past_ip){
    void* info_obj = this->update_inner(instr);
    past_results[instr->instr_id] = info_obj; 
    past_ip = instr->ip;
  }
}

void* code_info::query_past(uint64_t instr_id){
  auto iter = past_results.find(instr_id);
  if(iter != past_results.end()){
    return iter->second;
  } else {
    return nullptr;
  }
}

/*************** code_informer ***************/

code_informer::code_informer(){
  infos.push_back(new call_stack());  
}

void code_informer::handle_retire(ooo_model_instr* instr){
  static std::vector<void*> info_buf;
  
  for(code_info* info : infos){
    info->update(instr);
  }

  auto todo_iter = todo.find(instr->instr_id);
  if(todo_iter == todo.end()){
    // nothing to do!
    return;
  }

  query* q = todo_iter->second;
  info_buf.resize(1+q->iids.size());
  code_info* info = infos[q->type];

  info_buf[0] = info->query_past(instr->instr_id);
  for(size_t i = 0; i<info_buf.size()-1; ++i){
    info_buf[i+1] = info->query_past(q->iids[i]);
  }
  q->run_cb(info_buf);
  todo.erase(instr->instr_id);
  delete q;
}

code_informer* code_informer::get_instance(){
  static code_informer instance;
  return &instance;
}
