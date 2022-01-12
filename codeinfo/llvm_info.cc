
#include "llvm_info.h"

#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include <cstdlib>

#include "champsim.h"

namespace llvm_info_impl {

static std::string make_fname(const char* suffix){
  return std::string("info/") + get_trace_name() + suffix;
}

struct correl_info {
  uint32_t bb;
  uint16_t instr;
  uint64_t pc_start;

  correl_info(uint32_t bb, uint16_t instr, uint64_t pc)
    : bb(bb), instr(instr), pc_start(pc) {}

  bool operator<(const correl_info& other) const {
    return bb == other.bb ? instr < other.instr : bb < other.bb;
  }
};

struct parsed_objs {
  std::unordered_map<std::string, std::vector<correl_info>> by_func;
  std::unordered_map<std::string, std::pair<uint64_t, uint64_t>> frange;
};

uint64_t parse_u64(const std::string& str, int base){
  return std::stoull(str, nullptr, base);
}

parsed_objs* read_dbg_file(){
  std::string fname = make_fname(".crl");
  std::ifstream fin(fname);

  // lifetimes here are driving me crazy
  parsed_objs* ret_obj = new parsed_objs;

  auto& by_func = ret_obj->by_func;
  auto& frange = ret_obj->frange;

  std::regex func_header("^FUNC\\s+([0-9a-f]+)\\s+([0-9a-f]+)\\s+(\\S+)$");
  std::regex ir_entry("^IR\\s+([0-9a-f]+)\\s+(\\d+)\\s+(\\d+)$");

  std::string func;
  while(!fin.eof()){
    std::string line;
    std::getline(fin, line);

    std::smatch match;
    if(std::regex_match(line, match, ir_entry)){
      by_func[func].emplace_back(parse_u64(match[3], 10), 
        parse_u64(match[2], 10), parse_u64(match[1], 16));
    } else if(std::regex_match(line, match, func_header)){
      func = match[3];
      frange[func] = std::make_pair<uint64_t, uint64_t>(
        parse_u64(match[1], 16), parse_u64(match[2], 16));
    } // else just skip it.
  }

  for(auto iter = by_func.begin(); iter != by_func.end(); ++iter){
    std::sort(iter->second.begin(), iter->second.end());
  }
  return ret_obj;
}

class my_inst_iterator {
private:
  llvm::Function* f;
  llvm::BasicBlock* bb;
  uint32_t bb_num_;
  uint16_t i_num_;
  llvm::inst_iterator inst_iter_;
  bool done_;

  void skip_debug(){
    while(inst_iter_ != llvm::inst_end(f) && llvm::isa<llvm::DbgInfoIntrinsic>(&*inst_iter_)){
      ++inst_iter_;
    }
  }

public:
  my_inst_iterator(llvm::Function* f) 
    : f(f), bb(nullptr), bb_num_(0), i_num_(0), done_(false) {
    inst_iter_ = llvm::inst_begin(f);
    skip_debug();
  }

  uint32_t bb_num(){ return bb_num_; }
  uint32_t i_num(){ return i_num_; }
  llvm::Instruction* get_instr(){ return &*inst_iter_; }
  bool done(){ return done_; }

  void advance(){
    if(done_){ return; }

    ++inst_iter_;
    skip_debug();
    if(inst_iter_ == llvm::inst_end(f)){
      done_ = true;
      return;
    }
    
    if(bb != nullptr && inst_iter_->getParent() != bb){
      ++bb_num_;
      i_num_ = 0;
    } else {
      ++i_num_;
    }
    bb = inst_iter_->getParent();
  }
};

}

llvm_info::llvm_info(){
  using namespace llvm;
  using namespace llvm_info_impl;

  // get module

  const auto ll_fname = make_fname(".ll");
  SMDiagnostic err;
  auto SetDataLayout = [](StringRef) -> Optional<std::string> { return None; };
  this->mod = parseIRFile(ll_fname, err, this->context, SetDataLayout).release();

  // get vaddr offset
  const auto va_fname = make_fname(".offset");
  {
    std::cerr << va_fname << '\n';
    std::ifstream fin(va_fname);
    std::string buf;
    fin >> buf;
    addr_offset = static_cast<uint64_t>(stoull(buf, nullptr, 0));
  }

  // get debug info
  parsed_objs* dbg_objs = read_dbg_file(); 
  auto& fmap = dbg_objs->by_func;
  auto& frange = dbg_objs->frange;

  for(const auto& entry : fmap){
    Function* f = mod->getFunction(entry.first);
    if(f == nullptr){
      continue;
    }
    assert(frange.find(entry.first) != frange.end());
    this->func_starts[f] = dbg_objs->frange[entry.first];

    my_inst_iterator llvm_iter(f);
    int first_correl_pos = -1;

    // guaranteed every llvm instr can correspond to only one dbg element
    for(const correl_info& cinfo : entry.second){
      if(first_correl_pos == -1){
        first_correl_pos = static_cast<int>(correl.size());
      }
      correl.emplace_back();
      bool matches = false;
      do {
        Instruction* ins = llvm_iter.get_instr();

        correl.back().first = cinfo.pc_start;
        correl.back().second.push_back(ins);

        matches = (llvm_iter.bb_num() == cinfo.bb) && (llvm_iter.i_num() == cinfo.instr);
        llvm_iter.advance();
      } while(!llvm_iter.done() && !matches);
    }

    // include the function prologue in the first correlation's range.
    if(first_correl_pos != -1){
      correl[first_correl_pos].first = this->func_starts[f].first;
    }
  }

  // first dummy
  correl.emplace_back();
  correl.back().first = 0x0;
  // last dummy
  correl.emplace_back();
  correl.back().first = 0x200000000000ULL;

  std::sort(correl.begin(), correl.end());
}
  
llvm_info::inst_range* llvm_info::get_llvm_instrs(uint64_t pc){
  map_info info;
  uint64_t real_addr = pc-addr_offset;
  info.first = real_addr;

  auto iter = std::upper_bound(correl.begin(), correl.end(), info,
    [](const map_info& prior, const map_info& info){ return prior.first < info.first; });
  --iter;

  if(iter->second.size() == 0){
    // invalid.
    return nullptr;
  } else {
    auto instr = iter->second.back();
    auto& addr_range = this->func_starts[instr->getFunction()];
    if(addr_range.first <= real_addr && real_addr <= addr_range.second){
      // valid
      return &(iter->second);
    } else {
      return nullptr;
      // invalid
    }
  }
}

