
import sys
from elftools.elf.elffile import ELFFile
from elftools.dwarf.locationlists import (
  LocationEntry, LocationExpr, LocationParser
)

import func_range
import inst_corr
import var_corr

def make_die_index(compile_unit):
  index = {}
  for entry in compile_unit.iter_DIEs():
    if entry.tag not in index:
      index[entry.tag] = []
    index[entry.tag].append(entry)
  return index

def process_file(filename):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)
    print(compile_unit['version'])

    index = make_die_index(compile_unit)
    
    '''
    addr_to_func = func_range.get_addr_func(index)
    entries = inst_corr.get_entries(dwarf_info, compile_unit, addr_to_func)

    for func_id, dbg_v in entries:
      func_name = addr_to_func[func_id][2]
      entry_v = inst_corr.process_entries(dbg_v)

      for entry in entry_v:
        print('%x %d %d %s'%(entry.address, entry.column, entry.line, func_name))
    '''

    '''
    A couple of ways to represent (single location)
    1. Memory
    2. Register
    3. Empty
    4. No location, but a value terminated with DW_OP_stack_value
    note: we do not support composite locations.

    Location lists:
    instances of LocationEntry
    1. Begin addr offset
    2. End addr offset
    3. Descriptor
    '''
    for v in var_corr.get_vars(dwarf_info, index, compile_unit['version']):
      pass

process_file(sys.argv[1])
