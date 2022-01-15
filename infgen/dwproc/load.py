
'''
The loader- there are generally three data structures from pyelftools
that we want- the dwarf_info object, that has some useful methods,
the compile_unit, which is accessed from dwarf_info but we don't want
to have to dereference every time, and the index of the compile_unit.
'''

import sys
from elftools.elf.elffile import ELFFile

def make_die_index(compile_unit):
  index = {}
  for entry in compile_unit.iter_DIEs():
    if entry.tag not in index:
      index[entry.tag] = []
    index[entry.tag].append(entry)
  return index

'''
Note the pattern here- this is NOT the main function,
main is typically the handler- so we can isolate what
we want to do from the loading logic.
'''
def process_file(filename, handler):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)
    index = make_die_index(compile_unit)

    handler(dwarf_info, compile_unit, index)

