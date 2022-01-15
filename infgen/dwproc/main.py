
import sys

import load
import func_range
import inst_corr

'''
The main logic (right now, for instruction correlation only.
'''

def my_runner(dwarf_info, compile_unit, index):
  addr_to_func = func_range.get_addr_func(index)
  entries = inst_corr.get_entries(dwarf_info, compile_unit, addr_to_func)
  
  '''
  addr_to_func gives us information about a function's address range.
  So we are iterating over each function, and then for each function printing
  information about each instruction.
  '''
  for func_id, dbg_v in entries:
    func_info = addr_to_func[func_id]

    # note the members of func_info are low_pc, high_pc, and name
    print('FUNC %x %x %s'% func_info)
    
    func_name = func_info[2]
    entry_v = inst_corr.process_func_entries(dbg_v)

    for entry in entry_v:
      '''
      note here, the column and line are instruction number and basic block 
      number, respectively.
      '''
      print('IR %x %d %d'%(entry.address, entry.column, entry.line))

load.process_file(sys.argv[1], my_runner)
