from MDSplus import *
from ctypes import *

class REDPYTADC(Device):
  parts=[{'path':':IP_ADDR', 'type':'text'},{'path':':COMMENT', 'type':'text'},
  {'path':':ACQ_TIME', 'type':'numeric', 'value':10E3},
  {'path':':TRIG_MODE', 'type':'text', 'value':'INTERNAL'},
  {'path':':FULL_SCALE', 'type':'text', 'value':'12V'},
  {'path':':FREQUENCY', 'type':'numeric', 'value':1E4},
  {'path':':TRIG_TIME', 'type':'numeric'},
  {'path':':CHANNEL_1', 'type':'signal'},
  {'path':':CHANNEL_2', 'type':'signal'}]

  parts.append({'path':':INIT_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','INIT',50,None),Method(None,'init',head))",
	'options':('no_write_shot',)})
  parts.append({'path':':STORE_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','STORE',50,None),Method(None,'store',head))",
	'options':('no_write_shot',)})

  print 'REDPYTADC activated'
  