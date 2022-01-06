
import sys
import bisect
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

def process_file(filename):
  with open(filename, 'rb') as f:
    elffile = ELFFile(f)
    dwarf_info = elffile.get_dwarf_info()
    compile_unit = dwarf_info.get_CU_at(0)

    addr_to_func = get_addr_func(compile_unit)

    entries = map(lambda lpe : lpe.state, 
              filter(lambda entry : entry.state != None, 
              dwarf_info.line_program_for_CU(compile_unit).get_entries()))

    LINE_MASK = 0x80000000
    COL_MASK = 0x8000
  
    for entry in entries:
      if((entry.line & LINE_MASK) != 0 and (entry.column & COL_MASK) != 0):
        func_name = addr_to_func[bisect.bisect_right(addr_to_func, (entry.address, 0xffffffff, ''))-1][2]
        print('FName:%s, Addr:%x, BB:%d, Ins:%d'%(func_name, entry.address, entry.line ^ LINE_MASK, entry.column ^ COL_MASK))

process_file(sys.argv[1])
