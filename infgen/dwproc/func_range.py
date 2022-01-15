
from elftools.dwarf.descriptions import describe_form_class

'''
input: (from the index), a list of subprogram objects
output: a list of (low_pc, high_pc, func_name) tuples
'''
def get_addr_func(index):
  return sorted(
    map(lambda entry : (entry[0].value, 
                        entry[1].value 
                          if describe_form_class(entry[1].form) == 'address' 
                          else entry[1].value + entry[0].value, 
                        str(entry[2].value, encoding='utf8')),
    map(lambda entry : (entry.attributes['DW_AT_low_pc'], 
                        entry.attributes['DW_AT_high_pc'], 
                        entry.attributes['DW_AT_name']),
    index['DW_TAG_subprogram'])))
    
