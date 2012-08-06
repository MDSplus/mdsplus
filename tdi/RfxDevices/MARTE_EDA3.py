# -*- coding: iso-8859-1 -*-
from MDSplus import *
import time
import MARTE_GENERIC

class MARTE_EDA3(MARTE_GENERIC.MARTE_GENERIC):
    print 'MARTe EDA3'
    parNames = ['tauVTheta','tauVTor','tauD','kp','ki','kd','vCompOn','iCompOn','iFBOn','iPTrig','tTrig',
       'deltaVTorTrig','deltaTBT1','deltaTBT2','rVesselTuningFactor']
    parValues = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0] 
    parts = []
    for i in range(len(MARTE_GENERIC.MARTE_GENERIC.parts)):
      parts.append(MARTE_GENERIC.MARTE_GENERIC.parts[i])
    parts.append({'path':'.PARAMS', 'type':'structure'})
    parts.append({'path':'.PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(parNames)})
    for i in range(len(parNames)):
      print 'MARTe mhd_bt', i, 1
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      print 'MARTe mhd_bt', i, 2
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      print 'MARTe mhd_bt', i, 3
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text', 'value':parNames[i]})
      print 'MARTe mhd_bt', i, 4
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      print 'MARTe mhd_bt', i, 5
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      print 'MARTe mhd_bt', i, 6
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric','value':parValues[i]})

    for i in range(len(parNames), 256):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric'})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':0})
    for i in range(64):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})

