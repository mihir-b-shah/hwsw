
import itertools
from elftools.common.py3compat import itervalues
from elftools.dwarf.locationlists import (
  LocationEntry, LocationExpr, LocationParser
)
from elftools.dwarf.dwarf_expr import (
  DWARFExprParser
)

def make_loc_extr(version, loc_parser):
  def get_loc_info(entry):
    for attr in itervalues(entry.attributes):
      if loc_parser.attribute_has_location(attr, version):
        return loc_parser.parse_from_attribute(attr, version)
    return None
  # the enclosed function
  return get_loc_info

def attr_check(attr, entry, msg):
  if attr not in entry.attributes:
    raise NotImplementedError(msg)
  return entry.attributes[attr].value

def process_loclist_item(item):
  if isinstance(item, LocationEntry):
    return (item.loc_expr, item.begin_offset)
  else:
    raise NotImplementedError('LocList entry: ' + str(type(item)))

def parent_func_info(entry, attr, enc):
  parent = entry.get_parent()
  if(parent.tag == 'DW_TAG_compile_unit'):
    return None #global
  elif(parent.tag == 'DW_TAG_subprogram'):
    return enc(attr_check(attr, parent, 'Func has no %s'%(attr)))
  else:
    raise NotImplementedError('Cannot handle non func/mod parent.')

def process_entry_only(entry):
  parent_func = parent_func_info(entry, 'DW_AT_name',
    lambda name : str(name, encoding='utf8'))

  line = attr_check('DW_AT_decl_line', entry, 'Need decl_line.')

  if(entry.tag == 'DW_TAG_formal_parameter'):
    return ('PARAM', parent_func, line) #line is argnum
  elif(entry.tag == 'DW_TAG_variable'):
    GLOB_MASK = 0x80000000
    if((line & GLOB_MASK) != 0):
      glob_list_pos = line ^ GLOB_MASK
      return ('GLOB', glob_list_pos)
    else:
      name = attr_check('DW_AT_name', entry, 'Need name.')
      bb_num, i_num = (int(name), line)
      return ('LOC', parent_func, bb_num, i_num)
  else:
    raise AssertionError('entry.tag is not param or var.')

def make_entry_processor(cu_base_addr, expr_parser):
  # doesn't need to be closure, just for dumper
  def expr_handler(elist):
    return elist
    
  def parse_exprs(elist):
    return map(lambda pr : (expr_parser.parse_expr(pr[0]), pr[1]), elist)

  def process_entry(pr):
    (entry, loc) = pr
    corr_info = process_entry_only(entry)
    print(corr_info)

    elist = None
    if(isinstance(loc, LocationExpr)):
      elist = [(loc.loc_expr, cu_base_addr)]
    elif(isinstance(loc, list)):
      elist = map(process_loclist_item, loc)
    else:
      raise NotImplementedError('Top-level loc: ' + str(type(loc)))

    print(list(expr_handler(parse_exprs(elist))))

  # the enclosed function
  return process_entry

def get_vars(dwarf_info, index, version):
  loc_parser = LocationParser(dwarf_info.location_lists()) 
  loc_extractor = make_loc_extr(version, loc_parser)
  cu_base_addr = index['DW_TAG_compile_unit'][0].attributes['DW_AT_low_pc'].value

  entry_processor = make_entry_processor(cu_base_addr,
    DWARFExprParser(dwarf_info.structs))

  return list(map(entry_processor,
    filter(lambda pr : pr[1] != None,
    map(lambda entry: (entry, loc_extractor(entry)),
    itertools.chain(index['DW_TAG_variable'], index['DW_TAG_formal_parameter'])))))
