
import sys
from elftools.elf.elffile import ELFFile

def make_die_index(compile_unit):
  index = {}
  for entry in compile_unit.iter_DIEs():
    if entry.tag not in index:
      index[entry.tag] = []
    index[entry.tag].append(entry)
  return index

def process_file(filename, handler):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)
    index = make_die_index(compile_unit)

    handler(dwarf_info, compile_unit, index)

