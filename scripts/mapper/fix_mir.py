
import sys
import re

class Actions:
  def __init__(self):
    self.actions = []

  def register(self, pat, action):
    self.actions.append((pat, action))

  def ready(self):
    self.actions.append((re.compile('^.*$'), lambda m : m.group(0)))

  def run(self, line):
    for rule in self.actions:
      m = rule[0].match(line)
      if(m):
        print(rule[1](m))
        break

actions = Actions()
actions.register(re.compile('^(\s*value:\s+\'<(\d+)\s+x\s+(\w+)>)\s+zeroinitializer\'\s*$'), 
  lambda m : '%s <%s>\''%(m.group(1),', '.join(['%s 0'%(m.group(3))]*int(m.group(2)))))
actions.register(re.compile('^\s*failsVerification:\s+false\s*$'), lambda m : '')
actions.register(re.compile('^\s*tracksDebugUserValues:\s+true\s*$'), lambda m : '')
actions.ready()
  
for line in sys.stdin:
  actions.run(line)
