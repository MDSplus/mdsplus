from MDSplus import *
import time
import MARTE_COMMON

class MARTE_DEVICE(MARTE_COMMON.MARTE_COMMON):
    print 'MARTE_DEVICE'
    parNames = ['param1', 'param2', 'param3', 'param4', 'param5', 'param6', 'param7', 'param8', 'param9', 'param10', 'param11', 'param12', 'param13', 'param14', 'param15', 'param16' ]
    waveParNames = ['paramWave1', 'paramWave2', 'paramWave3', 'paramWave4', 'paramWave5', 'paramWave6', 'paramWave7', 'paramWave8']
    parValues = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    parts = []
    for i in range(len(MARTE_COMMON.MARTE_COMMON.parts)):
      parts.append(MARTE_COMMON.MARTE_COMMON.parts[i])
    parts.append({'path':'.PARAMS', 'type':'structure'})
    parts.append({'path':'.PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(parNames)})
    for i in range(len(parNames)):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text', 'value':parNames[i]})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric','value':parValues[i]})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':0})
    for i in range(len(waveParNames)):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text', 'value':waveParNames[i]})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})
    
