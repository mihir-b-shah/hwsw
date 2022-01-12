
import re
import sys

# global vars
file_inp = re.compile(r'^EVENT:\s(\w+)\sPC:\s[0-9a-f]+\sBEFCTXT:\s\[([^\[\]]*)\]\sAFTCTXT:\s\[([^\[\]]*)\]\s*$')
fpath = sys.argv[1]
STK_LEN=10
CHECK_SUBSETS=False

# data collectors
class EqualCtrs:
  def __init__(self):
    self.ctrs = [0]*(STK_LEN+1)

  def update(self, bef_ctxt, aft_ctxt):
    i = 0
    while(i < len(bef_ctxt) and bef_ctxt[i] == aft_ctxt[i]):
      self.ctrs[i] += 1
      i += 1
    if(i == 0):
      # should not happen, but if traces don't start at the start of program, can
      pass
    self.ctrs[STK_LEN] += 1

  def display(self, header):
    print('=========== %s ==========='%(header))
    for i in range(0,STK_LEN):
      print(self.ctrs[i]/self.ctrs[STK_LEN])

# collectors
eql_ctrs_div = [None]*(1 << STK_LEN if CHECK_SUBSETS else 1)
eql_ctrs_cvg = [None]*(1 << STK_LEN if CHECK_SUBSETS else 1)
if(CHECK_SUBSETS):
  for i in range(0, 1 << STK_LEN):
    eql_ctrs_div[i] = EqualCtrs()
    eql_ctrs_cvg[i] = EqualCtrs()
else:
  eql_ctrs_div[0] = EqualCtrs()
  eql_ctrs_cvg[0] = EqualCtrs()
  

def mask_arr(mask, arr):
  ret = []
  for i,e in enumerate(arr):
    if(mask & (1 << i)):
      ret.append(e)
  return ret

# main code
with open(fpath, 'r') as f:
  for line in f:
    res = file_inp.match(line)
    if(res != None):
      event_type = res.group(1)
      if(res.group(2) == 'INVALID' or res.group(3) == 'INVALID'):
        continue

      bef_ctxt = res.group(2).split(' ')
      bef_ctxt.pop()
      aft_ctxt = res.group(3).split(' ')
      aft_ctxt.pop()

      if(event_type == 'fresh'):
        continue

      if(event_type == 'divrg'):
        if(CHECK_SUBSETS):
          for i in range(1 << STK_LEN):
            mask_bef = mask_arr(i, bef_ctxt)
            mask_aft = mask_arr(i, aft_ctxt)
            eql_ctrs_div[i].update(mask_bef, mask_aft)
        else:
          eql_ctrs_div[0].update(bef_ctxt, aft_ctxt)

      elif(event_type == 'convg'):
        if(CHECK_SUBSETS):
          for i in range(1 << STK_LEN):
            mask_bef = mask_arr(i, bef_ctxt)
            mask_aft = mask_arr(i, aft_ctxt)
            eql_ctrs_cvg[i].update(mask_bef, mask_aft)
        else:
          eql_ctrs_cvg[0].update(bef_ctxt, aft_ctxt)
      else:
        raise RuntimeError('Error in parsing- not recognized.')

if(CHECK_SUBSETS):
  for i in range(1 << STK_LEN):
    eql_ctrs_div[i].display('Divergence counters for subset %d'%(i))
    eql_ctrs_cvg[i].display('Convergence counters for subset %d'%(i))
else:
  eql_ctrs_div[0].display('Divergence counters')
  eql_ctrs_cvg[0].display('Convergence counters')
