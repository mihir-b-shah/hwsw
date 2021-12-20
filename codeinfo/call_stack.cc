
#include "call_stack.h"

int32_t call_stack::funcmap_context(uintptr_t ip){
  sym_ref key(nullptr, ip);
  auto iter = std::upper_bound(func_starts.begin(), func_starts.end(), key);
  if(iter == func_starts.end()){
    // probably not in a function- somewhere external !!
    return -1;
  } else {
    return std::distance(func_starts.begin(), iter)-1;
  }
}

void call_stack::init_instr_set(std::unordered_set<uintptr_t>& instrs, FILE* f){
  uintptr_t addr;
  while(fscanf(f, "%" PRIxPTR "\n", &addr) > 0){
    instrs.insert(addr);
  }
}

// impl
call_stack::call_stack() : prev_call(false), prev_ret(false), prev_area(false) {
  using namespace std;

  string trace_name = get_trace_name();
  trace_name = string("info/") + trace_name;

  {
    std::ifstream fin(trace_name+".offset");
    std::string buf;
    fin >> buf;
    addr_offset = static_cast<uintptr_t>(stoull(buf, nullptr, 0));
  }

  // Setup to allow us to find out, which function am I in?
  FILE* names = fopen((trace_name+".names").c_str(), "r");

  uintptr_t addr;
  char name[1025];

  while(fscanf(names, "%" PRIxPTR " %s\n", &addr, name) > 0){
    func_starts.push_back(sym_ref(const_cast<const char*>(name), addr));
  }
  func_starts.push_back(sym_ref("__SENTINEL__", 0x100000000));
  sort(func_starts.begin(), func_starts.end());
  fclose(names);

  // Setup to find out, is this a call/ret/other instruction?
  FILE* fcalls = fopen((trace_name+".calls").c_str(), "r");
  init_instr_set(call_instrs, fcalls);
  fclose(fcalls);

  FILE* frets = fopen((trace_name+".rets").c_str(), "r");
  init_instr_set(ret_instrs, frets);
  fclose(frets);
}
  
void* call_stack::update_inner(ooo_model_instr* instr){
  uintptr_t ip = instr->ip - vaddr_offset();
  int32_t fid = funcmap_context(ip);

  /*
  std::fprintf(stderr, "rip: %lx ip: %lx call: %d ret: %d parea: %d fid: %d ", rip, ip, prev_call, prev_ret, prev_area, fid);
  if(fid == -1){
    std::cerr << "fname: " << "LIB_FUNC" << '\n';
  } else {
    std::cerr << "fname: " << func_starts[fid].name << '\n';
  }
  */

  // state machine time!
  switch((prev_call << 3) | (prev_ret << 2) 
     | ((fid != -1) << 1) | prev_area){
  case 0b0001:
  case 0b0101:
  case 0b1001:
    // if transitioning, don't push new context.
    // technically, only 1001 should be valid, but
    // bc of jumps/trampolining using ret, could be valid.
    break;
  case 0b0011:
    // running along in the program.
    break;
  case 0b1011:
    // just had call instr in program, push new context!
    call_stk.push_back(fid);
    break;
  case 0b0111:
    // just had ret instr in program, pop context!
    // think should be asserted...
    if(!call_stk.empty()){
      call_stk.pop_back();
    }
    // cannot assert that top() == fid, b/c of possible trampolining.
    break;
  case 0b0000:
    // i am running in library code, do nothing.
    break;
  case 0b0010:
  case 0b0110:
  case 0b1010:
    // i was in library code, came into program space.
    // only time to add myself to stack is if nothing 
    // is there. Nothing should be there the first time.
    if(call_stk.empty()){
      call_stk.push_back(fid);
    }
    break;
  case 0b0100:
  case 0b1000:
    // if not in a exe region, i dont care about call/ret instructions.
    // possible assert?
    break;
  case 0b1100:
  case 0b1101:
  case 0b1110:
  case 0b1111:
    // invalid, prev_call cannot be on with prev_ret
    assert(false);
    break;
  }

  prev_call = call_instrs.find(ip) != call_instrs.end();
  prev_ret = ret_instrs.find(ip) != ret_instrs.end();
  assert(!prev_call || !prev_ret);
  prev_area = fid != -1;

  if(call_stk.size() <= 1){
    return nullptr;
  } else {
    return const_cast<char*>(func_starts[call_stk[call_stk.size()-2]].name.c_str());
  }
}
  
