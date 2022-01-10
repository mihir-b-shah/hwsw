
import itertools
from elftools.common.py3compat import itervalues
from elftools.dwarf.locationlists import (
  LocationEntry, LocationExpr, LocationParser
)
from elftools.dwarf.dwarf_expr import (
  DWARFExprParser
)
from elftools.dwarf.descriptions import (
  ExprDumper
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

def process_entry_only(entry):
  if(entry.tag == 'DW_TAG_formal_parameter'):
    pass
  elif(entry.tag == 'DW_TAG_variable'):
    line = attr_check('DW_AT_decl_line', entry, 'Need decl_line.')
    GLOB_MASK = 0x80000000
    if((line & GLOB_MASK) != 0):
      glob_list_pos = line ^ GLOB_MASK
      return ('GLOB', glob_list_pos)
    else:
      name = attr_check('DW_AT_name', entry, 'Need name.')
      bb_num, i_num = (int(name), line)
      return ('LOC', bb_num, i_num)
  else:
    raise AssertionError('entry.tag is not param or var.')

def make_entry_processor(cu_base_addr, expr_parser, expr_dumper):
  # doesn't need to be closure, just for dumper
  def expr_handler(elist):
    for item in elist:
      print(expr_dumper.dump_expr(item[0]))
    
  def parse_exprs(elist):
    #return map(lambda pr : (expr_parser.parse_expr(pr[0]), pr[1]), elist)
    return elist

  def process_entry(pr):
    (entry, loc) = pr
    corr_info = process_entry_only(entry)

    if(isinstance(loc, LocationExpr)):
      expr_handler([(loc.loc_expr, cu_base_addr)])
    elif(isinstance(loc, list)):
      expr_handler(list(parse_exprs(map(process_loclist_item, loc))))
    else:
      raise NotImplementedError('Top-level loc: ' + str(type(loc)))

  # the enclosed function
  return process_entry

def get_vars(dwarf_info, index, version):
  loc_parser = LocationParser(dwarf_info.location_lists()) 
  loc_extractor = make_loc_extr(version, loc_parser)
  cu_base_addr = index['DW_TAG_compile_unit'][0].attributes['DW_AT_low_pc'].value

  entry_processor = make_entry_processor(cu_base_addr,
    DWARFExprParser(dwarf_info.structs), ExprDumper(dwarf_info.structs))

  return list(map(entry_processor,
    filter(lambda pr : pr[1] != None,
    map(lambda entry: (entry, loc_extractor(entry)),
    itertools.chain(index['DW_TAG_variable'], index['DW_TAG_formal_parameter'])))))
