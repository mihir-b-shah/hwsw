
import sys

import load
import var_corr

def var_runner(dwarf_info, compile_unit, index):
  for v in var_corr.get_vars(dwarf_info, index, compile_unit['version']):
    pass

load.process_file(sys.argv[1], var_runner)
