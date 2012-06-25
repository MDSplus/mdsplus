from MDSplus import *

class RFXWAVESETUP(Device):
    print 'RFXWaveSetup'
    """Generic Wavefor configuration mdsplus device"""
    parts=[]
    parts=[{'path':':COMMENT','type':'text'}]
    for i in range(1,7):
         parts.append({'path':'.WAVE_%d'%(i),'type':'structure'})
         parts.append({'path':'.WAVE_%d:COMMENT'%(i),'type':'text'})
         parts.append({'path':'.WAVE_%d:MIN_X'%(i),'type':'numeric', 'value':0})
         parts.append({'path':'.WAVE_%d:MAX_X'%(i),'type':'numeric', 'value':1})
         parts.append({'path':'.WAVE_%d:MIN_Y'%(i),'type':'numeric', 'value':0})
         parts.append({'path':'.WAVE_%d:MAX_Y'%(i),'type':'numeric', 'value':10})
         parts.append({'path':'.WAVE_%d:WAVE'%(i),'type':'signal', 'options':'compress_on_put'})
