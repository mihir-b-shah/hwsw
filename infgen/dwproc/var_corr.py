
def get_vars(dwarf_info, compile_unit):
  return list(
    map(lambda entry: entry.attributes['DW_AT_location'],
    filter(lambda entry: 'DW_AT_location' in entry.attributes,
    filter(lambda entry: entry.tag in ['DW_TAG_variable', 'DW_TAG_formal_parameter'],
    compile_unit.iter_DIEs()))))
