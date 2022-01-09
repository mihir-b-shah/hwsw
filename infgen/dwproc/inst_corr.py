
import bisect
import itertools

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
def get_entries(dwarf_info, compile_unit, addr_to_func):
  return itertools.groupby(
    map(lambda entry : process_entry(entry),
    filter(lambda entry : (entry.line & LINE_MASK) != 0,
    filter(lambda entry : (entry.column & COL_MASK) != 0,
    map(lambda lpe : lpe.state, 
    filter(lambda entry : entry.state != None, 
      dwarf_info.line_program_for_CU(compile_unit).get_entries()
  ))))), key=make_dbg_sorter(addr_to_func))
