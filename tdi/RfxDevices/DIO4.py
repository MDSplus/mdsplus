from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import time

#import pdb

class DIO4(Device):
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """INCAA DIO4 Timing Module"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':SW_MODE', 'type':'text', 'value':'LOCAL'},
        {'path':':IP_ADDR', 'type':'text'},
        {'path':':COMMENT', 'type':'text'},
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'INTERNAL'},
        {'path':':REC_START_EV', 'type':'text'},
        {'path':':REC_EVENTS', 'type':'text'},
        {'path':':REC_TIMES', 'type':'numeric'},
        {'path':':SYNCH', 'type':'text', 'value':'NO'},
        {'path':':SYNCH_EVENT', 'type':'text'}]


    for i in range(8):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:FUNCTION'%(i+1), 'type':'text', 'value':'CLOCK'})
        parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i+1), 'type':'text', 'value':'SOFTWARE'})
        parts.append({'path':'.CHANNEL_%d:EVENT'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d:CYCLIC'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d:DELAY'%(i+1), 'type':'numeric', 'value':0.})
        parts.append({'path':'.CHANNEL_%d:DURATION'%(i+1), 'type':'numeric', 'value':1.})
        parts.append({'path':'.CHANNEL_%d:FREQ_1'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:FREQ_2'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_1'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_2'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:DUTY_CYCLE'%(i+1), 'type':'numeric', 'value':50})
        parts.append({'path':'.CHANNEL_%d:TRIGGER'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:CLOCK'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_1'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_2'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:COMMENT'%(i+1), 'type':'text'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV1'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TERM'%(i+1), 'type':'text', 'value':'ON'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:EDGE'%(i+1), 'type':'text', 'value':'RISING'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV2'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TERM'%(i+1), 'type':'text', 'value':'ON'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:EDGE'%(i+1), 'type':'text', 'value':'RISING'})


    parts.append({'path':'.OUT_EV_SW', 'type':'structure'})
    parts.append({'path':'.OUT_EV_SW:NAME', 'type':'text'})
    parts.append({'path':'.OUT_EV_SW:CODE', 'type':'numeric'})
    parts.append({'path':'.OUT_EV_SW:TIME', 'type':'numeric', 'value':0})