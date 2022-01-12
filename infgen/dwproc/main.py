
import sys

import load
import func_range
import inst_corr

def my_runner(dwarf_info, compile_unit, index):
  addr_to_func = func_range.get_addr_func(index)
  entries = inst_corr.get_entries(dwarf_info, compile_unit, addr_to_func)

  for func_id, dbg_v in entries:
    func_info = addr_to_func[func_id]
    print('FUNC %x %x %s'% func_info)

    func_name = func_info[2]
    entry_v = inst_corr.process_entries(dbg_v)

    for entry in entry_v:
      print('IR %x %d %d'%(entry.address, entry.column, entry.line))

load.process_file(sys.argv[1], my_runner)
