
import sys

import load
import var_corr

'''
This is right now a separate main file (and not called anywhere).
Should we decide to use debug information about values/variables as
well, this could be merged into our main.py file.

An idea about how the format would work for the output .crl file:

right now, it's like this:
FUNC low_pc high_pc name
IR pc ins_num bb_num
IR pc ins_num bb_num
...

Since each value, except parameters and globals, is associated with a single
IR, we could just add our information on as part of the IR line. Parameters
could go along with the FUNC, and globals could go in their own section.
'''
def var_runner(dwarf_info, compile_unit, index):
  for v in var_corr.get_vars(dwarf_info, index, compile_unit['version']):
    pass

load.process_file(sys.argv[1], var_runner)
