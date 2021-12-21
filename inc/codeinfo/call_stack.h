
#ifndef CALL_STACK_H
#define CALL_STACK_H

#include <vector>
#include <unordered_set>
#include <cinttypes>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <fstream>
#include <string>

#include "code_info.h"
#include "champsim.h"
  
struct sym_ref {
  string name;
  uintptr_t addr;

  sym_ref(const char* name, uintptr_t addr) : name(name == nullptr ? "" : name), addr(addr) {}
  bool operator<(const sym_ref& other) const { return addr < other.addr; }
};

struct call_stack {
  std::string caller;

  call_stack() : caller("") {}
  explicit call_stack(std::string name) : caller(name) {}
};

struct call_stack_impl {
  // vars
  std::vector<sym_ref> func_starts;
  std::unordered_set<uintptr_t> call_instrs;
  std::unordered_set<uintptr_t> ret_instrs;
  std::vector<int32_t> call_stk;

  bool prev_call;
  bool prev_ret;
  bool prev_area;
  uintptr_t addr_offset;

  // methods
  call_stack_impl();
  uintptr_t vaddr_offset(){ return addr_offset; }
  int32_t funcmap_context(uintptr_t ip);
  void init_instr_set(std::unordered_set<uintptr_t>& instrs, FILE* f);
  void update_state(ooo_model_instr* instr);
};

template<>
class code_info_impl<call_stack> : public code_info<code_info_impl, call_stack> {
public:
  code_info_impl<call_stack>(){}
  call_stack update_impl(ooo_model_instr* instr){
    impl.update_state(instr);
    string ref = impl.call_stk.empty() ? "" : impl.func_starts[impl.call_stk.back()].name;
    return call_stack(ref);
  }

private:
  call_stack_impl impl;
};

#endif
