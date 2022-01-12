
import re

with open('results', 'r') as f:
  for line in f:
    for match in re.finditer(r'(DW_OP_\w+)', line):
      print(match.group(1))

