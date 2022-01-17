
import bisect
import itertools

LINE_MASK = 0x80000000
COL_MASK = 0x8000

def entry_remove_masks(entry):
  entry.line = (entry.line ^ LINE_MASK)-1
  entry.column = (entry.column ^ COL_MASK)-1
  return entry

'''
addr_to_func is a sorted vector by starting pc of the function, and
we want to find the function which our pc is in. bisect_right returns
an insertion point coming to the right of any existing entries (i.e.
for the first instruction of the function). So regardless, we are
guaranteed we will end up in the next function, so subtract one. 

My version of Python doesn't have a key option for bisect_right,
so I am putting dummy fields for the rest of the struct.
'''
def func_index(pc, addr_to_func):
  return bisect.bisect_right(addr_to_func, (pc, 0xffffffff, ''))-1

# closures!
def make_dbg_sorter(addr_to_func):
  def dbg_sorter(v):
    return func_index(v.address, addr_to_func)
  return dbg_sorter

'''
Output: an iterable of pairs, where for each the first entry of the pair
is the function id, from func_index, and the second is an iterable of debug entries.
'''
def get_entries(dwarf_info, compile_unit, addr_to_func):
  # warning- groupby requires inputs to already be sorted on the key.
  return itertools.groupby(
    # note line is actually the basic block number, and col is the instr-number.
    map(lambda entry : entry_remove_masks(entry),
    filter(lambda entry : (entry.line & LINE_MASK) != 0,
    filter(lambda entry : (entry.column & COL_MASK) != 0,
    map(lambda lpe : lpe.state, 
    filter(lambda entry : entry.state != None, 
      dwarf_info.line_program_for_CU(compile_unit).get_entries()
  ))))), key=make_dbg_sorter(addr_to_func))

def entry_sorter(e):
  return (e.line << 16) | e.column

'''
Sort and remove duplicates in a function's debug entry list.
Sorting is useful since it makes easier the ChampSim code's processing of this
information. Removing duplicates would seem that it would not be needed, since
we aren't inserting duplicate debug line infos, but I hypothesize splitting debug
info in some lower-level pass might be happening.
'''
def process_func_entries(entries):
  return map(lambda grp : min(grp[1], key=lambda v: v.address),
    itertools.groupby(sorted(entries, key=entry_sorter), key=entry_sorter))
