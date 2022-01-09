
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
    
