import os
for m in os.listdir(os.path.dirname(__file__)):
  if m.endswith('.py') and m.startswith('do'):
    modnam=m[0:len(m)-3]
    exec ('from '+modnam+' import '+modnam,globals())
