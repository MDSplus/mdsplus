from MDSplus import *
from numpy import *
from threading import *
import time
class NI6259AI(Device):
    print 'NI6259AI'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """NI PXI-6259 M-series multi functional data acquisition card"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
      {'path':':COMMENT', 'type':'text'},
      {'path':':INPUT_MODE', 'type':'text', 'value':'RSE'},
      {'path':':CLOCK_MODE', 'type':'text', 'value':'INTERNAL'},
      {'path':':CLOCK_FREQ', 'type':'numeric', 'value':1000},
      {'path':':BUF_SIZE', 'type':'numeric', 'value':1000},
      {'path':':SEG_LENGTH', 'type':'numeric', 'value':10000},
      {'path':':CLOCK_SOURCE', 'type':'numeric'}]
    for i in range(0,32):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:STATE'%(i+1), 'type':'text', 'value':'ENABLED'})
        parts.append({'path':'.CHANNEL_%d:POLARITY'%(i+1), 'type':'text', 'value':'BIPOLAR'})
        parts.append({'path':'.CHANNEL_%d:RANGE'%(i+1), 'type':'text', 'value':'10V'})
        parts.append({'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal'})
    del i
    parts.append({'path':':NUM_SAMPLES', 'type':'numeric'})
    parts.append({'path':':INIT_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('PXI_SERVER','READY',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot',)})
    

      
