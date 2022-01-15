
import itertools
from elftools.common.py3compat import itervalues
from elftools.dwarf.locationlists import (
  LocationEntry, LocationExpr, LocationParser
)
from elftools.dwarf.dwarf_expr import (
  DWARFExprParser
)

'''
Closures are cool! Return a function that extracts
location information, if any, from an object.
'''
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

# note this tries to be generic
def parent_func_info(entry, attr, enc):
  parent = entry.get_parent()
  if(parent.tag == 'DW_TAG_compile_unit'):
    return None #global
  elif(parent.tag == 'DW_TAG_subprogram'):
    return enc(attr_check(attr, parent, 'Func has no %s'%(attr)))
  else:
    raise NotImplementedError('Cannot handle non func/mod parent.')

'''
Output: a tuple representing the entry, with the first element as a tag-
GLOB, LOC, and PARAM. I could have created a class hierarchy for this,
but I thought such a Lisp-y implementation of a record was simpler.

This also creates a clean division of labor when processing an entry- it
ONLY touches the non-DW_AT_location attributes in the entry.
'''
def process_entry_only(entry):
  parent_func = parent_func_info(entry, 'DW_AT_name',
    lambda name : str(name, encoding='utf8'))

  line = attr_check('DW_AT_decl_line', entry, 'Need decl_line.')

  if(entry.tag == 'DW_TAG_formal_parameter'):
    return ('PARAM', parent_func, line) #line is argnum
  elif(entry.tag == 'DW_TAG_variable'):
    '''
    Note, globals are masked and locals (not parameters) are not,
    in order to differentiate them here.
    '''
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
  # nothing for now
  def expr_handler(elist):
    return elist
    
  def parse_exprs(elist):
    return map(lambda pr : (expr_parser.parse_expr(pr[0]), pr[1]), elist)

  '''
  Process so that single exprs are converted into a location list, where the
  pc range spans the whole compilation unit.
  '''
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

''' 
right now, the list generated is meaningless (a bunch of None's), but the printing
is useful. We can see, for each value, the Function, BB, and Instr it
came from, and then its location information.
'''
def get_vars(dwarf_info, index, version):
  loc_parser = LocationParser(dwarf_info.location_lists()) 
  loc_extractor = make_loc_extr(version, loc_parser)
  cu_base_addr = index['DW_TAG_compile_unit'][0].attributes['DW_AT_low_pc'].value

  entry_processor = make_entry_processor(cu_base_addr,
    DWARFExprParser(dwarf_info.structs))

  # run our code over all formal parameters and variables (including globals)
  return list(map(entry_processor,
    filter(lambda pr : pr[1] != None,
    map(lambda entry: (entry, loc_extractor(entry)),
    itertools.chain(index['DW_TAG_variable'], index['DW_TAG_formal_parameter'])))))
