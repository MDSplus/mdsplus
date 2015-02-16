from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time

class CYGNET4K(Device):
    print 'Cygnet4K'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':NAME', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':EXP_TIME', 'type':'numeric', 'value':20E-3},
      {'path':':BINNING', 'type':'text', 'value':'NO'},
      {'path':':ROI_X', 'type':'numeric', 'value':0},
      {'path':':ROI_Y', 'type':'numeric', 'value':0},
      {'path':':ROI_WIDTH', 'type':'numeric', 'value':2048},
      {'path':':ROI_HEIGHT', 'type':'numeric', 'value':2048},
      {'path':':ROTATION', 'type':'text', 'value':'NONE'},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'INTERNAL'},
      {'path':':FRAME_PERIOD', 'type':'numeric', 'value':100E-3},
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':SENSOR_TEMP', 'type':'numeric'},
      {'path':':PCB_TEMP', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot')})
    print 'Cygnet4K added'
    
 
