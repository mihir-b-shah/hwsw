
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
  static constexpr size_t STK_SIZE = 10;
  std::array<int32_t,STK_SIZE> call_stk;

  call_stack() : call_stk{0} {}
  explicit call_stack(const std::vector<int32_t>& call_stk_v){
    auto fin = std::copy_n(call_stk_v.rbegin(), std::min(STK_SIZE, call_stk_v.size()), call_stk.begin());
    std::fill(fin, call_stk.end(), 0);
  }
  friend std::ostream& operator<<(std::ostream& os, const call_stack& cs){
    os << '[';
    for(auto iter = cs.call_stk.begin(); iter != cs.call_stk.end(); ++iter){
      os << *iter << ' ';
    }
    os << ']';
    return os;
  }
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
    return call_stack(impl.call_stk);
  }

private:
  call_stack_impl impl;
};

#endif
