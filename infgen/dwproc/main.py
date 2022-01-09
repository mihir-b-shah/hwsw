
import sys
from elftools.elf.elffile import ELFFile

import func_range
import inst_corr
import var_corr

def process_file(filename):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)

    addr_to_func = func_range.get_addr_func(compile_unit)
    entries = inst_corr.get_entries(dwarf_info, compile_unit, addr_to_func)

    for func_id, dbg_v in entries:
      func_name = addr_to_func[func_id][2]
      entry_v = sorted(dbg_v, key=lambda v: (v.line << 16) | v.column)
      for entry in entry_v:
        print('%x %d %d %s'%(entry.address, entry.column, entry.line, func_name))
    
    #for v in var_corr.get_vars(dwarf_info, compile_unit):
    #  print(v)

process_file(sys.argv[1])
