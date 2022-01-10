
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

#include "champsim.h"

namespace llvm_info_impl {

static std::string make_fname(const char* suffix){
  return std::string("info") + get_trace_name() + suffix;
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

std::unordered_map<std::string, std::vector<correl_info>> read_dbg_file(){
  std::string fname = make_fname(".crl");
  std::ifstream fin(fname);

  std::unordered_map<std::string, std::vector<correl_info>> by_func;

  std::string func;
  uint64_t pc;
  uint16_t instr;
  uint32_t bb;

  while(fin >> std::hex >> pc >> std::dec >> instr >> bb >> func){
    by_func[func].emplace_back(bb, instr, pc);
  }

  for(auto iter = by_func.begin(); iter != by_func.end(); ++iter){
    std::sort(iter->second.begin(), iter->second.end());
  }
  return by_func;
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
    std::ifstream fin(va_fname);
    std::string buf;
    fin >> buf;
    addr_offset = static_cast<uint64_t>(stoull(buf, nullptr, 0));
  }

  // get debug info
  auto fmap = read_dbg_file(); 
  for(const auto& entry : fmap){
    Function* f = mod->getFunction(entry.first);
    if(f == nullptr){
      continue;
    }

    my_inst_iterator llvm_iter(f);

    // guaranteed every llvm instr can correspond to only one dbg element
    for(const correl_info& cinfo : entry.second){
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
  }

  std::sort(correl.begin(), correl.end());
}
  
llvm_info::inst_range& llvm_info::get_llvm_instr(uint64_t pc){
  auto iter = std::upper_bound(correl.begin(), correl.end(), pc,
    [](double v, const map_info& info){ return v < info.first; });
  return iter->second;
}

