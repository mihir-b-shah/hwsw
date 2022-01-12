
#include "instruction.h"

#include "info_manager.h"
#include "llvm_info.h"
#include "code_informer.h"
#include "call_stack.h"

void info_manager::update_all(ooo_model_instr* instr){
  llvm_info* instance = llvm_info::get_instance();
  llvm_info::inst_range* correl_irs = instance->get_llvm_instrs(instr->ip);

  // all instances
  code_informer<call_stack>::get_instance()
    ->handle_retire(correl_irs, instr);
}
