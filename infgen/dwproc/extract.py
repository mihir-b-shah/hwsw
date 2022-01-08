
import sys
import bisect
import itertools
from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class

def get_addr_func(compile_unit):
  return sorted(
    map(lambda entry : (entry[0].value, 
                        entry[1].value 
                          if describe_form_class(entry[1].form) == 'address' 
                          else entry[1].value + entry[0].value, 
                        str(entry[2].value, encoding='utf8')),
    map(lambda entry : (entry.attributes['DW_AT_low_pc'], 
                        entry.attributes['DW_AT_high_pc'], 
                        entry.attributes['DW_AT_name']),
    filter(lambda entry : entry.tag == 'DW_TAG_subprogram',
    compile_unit.iter_DIEs()))))
    
LINE_MASK = 0x80000000
COL_MASK = 0x8000

def process_entry(entry):
  entry.line = (entry.line ^ LINE_MASK)-1
  entry.column = (entry.column ^ COL_MASK)-1
  return entry

def func_index(pc, addr_to_func):
  return bisect.bisect_right(addr_to_func, (pc, 0xffffffff, ''))-1

def make_dbg_sorter(addr_to_func):
  def dbg_sorter(v):
    return func_index(v.address, addr_to_func)
  return dbg_sorter

# note line is actually the basic block number, and col is the instr-number.
# didnt want to wrap everything for now, but...
def process_file(filename):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)

    addr_to_func = get_addr_func(compile_unit)

    entries = itertools.groupby(
      map(lambda entry : process_entry(entry),
      filter(lambda entry : (entry.line & LINE_MASK) != 0,
      filter(lambda entry : (entry.column & COL_MASK) != 0,
      map(lambda lpe : lpe.state, 
      filter(lambda entry : entry.state != None, 
        dwarf_info.line_program_for_CU(compile_unit).get_entries()
    ))))), key=make_dbg_sorter(addr_to_func))

    for func_id, dbg_v in entries:
      func_name = addr_to_func[func_id][2]
      entry_v = sorted(dbg_v, key=lambda v: (v.line << 16) | v.column)
      for entry in entry_v:
        print('%x %d %d %s'%(entry.address, entry.column, entry.line, func_name))

process_file(sys.argv[1])
