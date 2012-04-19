# -*- coding: iso-8859-1 -*-
from MDSplus import *
import time
import MARTE_GENERIC

class MARTE_MHD_CTRL(MARTE_GENERIC.MARTE_GENERIC):
    print 'MARTe MhdCtrl'
    parNames = ['cleaningEnable', 'extrapolationEnable', 'reconfigurationEnable', 'dynamicDecouplerEnable', 'staticDecouplerEnable', 'applyCleaning', 'extrapolationRadius', 
    'rgM_1','rgM_2','rgM_3','rgM_4','rgM_5','rgM_6','rgM_7','rgM_8', 
    'rgN_1','rgN_2','rgN_3','rgN_4','rgN_5','rgN_6','rgN_7','rgN_8',
    'rgFreq_1','rgFreq_2','rgFreq_3','rgFreq_4','rgFreq_5','rgFreq_6','rgFreq_7','rgFreq_8',
    'rgAmpl_1','rgAmpl_2','rgAmpl_3','rgAmpl_4','rgAmpl_5','rgAmpl_6','rgAmpl_7','rgAmpl_8',
    'rgPhase_1','rgPhase_2','rgPhase_3','rgPhase_4','rgPhase_5','rgPhase_6','rgPhase_7','rgPhase_8',
    'rgTStart_1','rgTStart_2','rgTStart_3','rgTStart_4','rgTStart_5','rgTStart_6','rgTStart_7','rgTStart_8',
    'rgRiseTime_1','rgRiseTime_2','rgRiseTime_3','rgRiseTime_4','rgRiseTime_5','rgRiseTime_6','rgRiseTime_7','rgRiseTime_8',
    'rgTEnd_1','rgTEnd_2','rgTEnd_3','rgTEnd_4','rgTEnd_5','rgTEnd_6','rgTEnd_7','rgTEnd_8',
    'rgFallTime_1','rgFallTime_2','rgFallTime_3','rgFallTime_4','rgFallTime_5','rgFallTime_6','rgFallTime_7','rgFallTime_8',
    'rgThreshold_1','rgThreshold_2','rgThreshold_3','rgThreshold_4','rgThreshold_5','rgThreshold_6','rgThreshold_7','rgThreshold_8',
    'rgIsRelative_1','rgIsRelative_2','rgIsRelative_3','rgIsRelative_4','rgIsRelative_5','rgIsRelative_6','rgIsRelative_7','rgIsRelative_8',
    'ffgM_1','ffgM_2','ffgM_3','ffgM_4','ffgM_5','ffgM_6','ffgM_7','ffgM_8',
    'ffgN_1','ffgN_2','ffgN_3','ffgN_4','ffgN_5','ffgN_6','ffgN_7','ffgN_8',
    'ffgFreq_1','ffgFreq_2','ffgFreq_3','ffgFreq_4','ffgFreq_5','ffgFreq_6','ffgFreq_7','ffgFreq_8',
    'ffgAmpl_1','ffgAmpl_2','ffgAmpl_3','ffgAmpl_4','ffgAmpl_5','ffgAmpl_6','ffgAmpl_7','ffgAmpl_8',
    'ffgPhase_1','ffgPhase_2','ffgPhase_3','ffgPhase_4','ffgPhase_5','ffgPhase_6','ffgPhase_7','ffgPhase_8',
    'ffgTStart_1','ffgTStart_2','ffgTStart_3','ffgTStart_4','ffgTStart_5','ffgTStart_6','ffgTStart_7','ffgTStart_8',
    'ffgRiseTime_1','ffgRiseTime_2','ffgRiseTime_3','ffgRiseTime_4','ffgRiseTime_5','ffgRiseTime_6','ffgRiseTime_7','ffgRiseTime_8',
    'ffgTEnd_1','ffgTEnd_2','ffgTEnd_3','ffgTEnd_4','ffgTEnd_5','ffgTEnd_6','ffgTEnd_7','ffgTEnd_8',
    'ffgFallTime_1','ffgFallTime_2','ffgFallTime_3','ffgFallTime_4','ffgFallTime_5','ffgFallTime_6','ffgFallTime_7','ffgFallTime_8',
    'ffgThreshold_1','ffgThreshold_2','ffgThreshold_3','ffgThreshold_4','ffgThreshold_5','ffgThreshold_6','ffgThreshold_7','ffgThreshold_8',
    'ffgIsRelative_1','ffgIsRelative_2','ffgIsRelative_3','ffgIsRelative_4','ffgIsRelative_5','ffgIsRelative_6','ffgIsRelative_7','ffgIsRelative_8',
    'horFieldControlEnable_1','horFieldControlEnable_2','horFieldControlEnable_3','horFieldControlEnable_4','horFieldControlEnable_5','horFieldControlEnable_6','horFieldControlEnable_7','horFieldControlEnable_8',
    'kpAmplitudes_1','kpAmplitudes_2','kpAmplitudes_3','kpAmplitudes_4','kpAmplitudes_5','kpAmplitudes_6','kpAmplitudes_7','kpAmplitudes_8',
    'kpPhases_1','kpPhases_2','kpPhases_3','kpPhases_4','kpPhases_5','kpPhases_6','kpPhases_7','kpPhases_8',
    'ki_1','ki_2','ki_3','ki_4','ki_5','ki_6','ki_7','ki_8',
    'kd_1','kd_2','kd_3','kd_4','kd_5','kd_6','kd_7','kd_8',
    'tStart_1','tStart_2','tStart_3','tStart_4','tStart_5','tStart_6','tStart_7','tStart_8',
    'tEnd_1','tEnd_2','tEnd_3','tEnd_4','tEnd_5','tEnd_6','tEnd_7','tEnd_8',
    'cutoffFreq_1','cutoffFreq_2','cutoffFreq_3','cutoffFreq_4','cutoffFreq_5','cutoffFreq_6','cutoffFreq_7','cutoffFreq_8',
    'vsKpAmplitudes_1','vsKpAmplitudes_2','vsKpAmplitudes_3','vsKpAmplitudes_4','vsKpAmplitudes_5','vsKpAmplitudes_6','vsKpAmplitudes_7','vsKpAmplitudes_8',
    'vsKpPhases_1','vsKpPhases_2','vsKpPhases_3','vsKpPhases_4','vsKpPhases_5','vsKpPhases_6','vsKpPhases_7','vsKpPhases_8',
    'vsKp_1','vsKp_2','vsKp_3','vsKp_4','vsKp_5','vsKp_6','vsKp_7','vsKp_8',
    'vsKi_1','vsKi_2','vsKi_3','vsKi_4','vsKi_5','vsKi_6','vsKi_7','vsKi_8',
    'vsKd_1','vsKd_2','vsKd_3','vsKd_4','vsKd_5','vsKd_6','vsKd_7','vsKd_8',
    'vsTStart_1','vsTStart_2','vsTStart_3','vsTStart_4','vsTStart_5','vsTStart_6','vsTStart_7','vsTStart_8',
    'vsTEnd_1','vsTEnd_2','vsTEnd_3','vsTEnd_4','vsTEnd_5','vsTEnd_6','vsTEnd_7','vsTEnd_8',
    'vsCutoffFreq_1','vsCutoffFreq_2','vsCutoffFreq_3','vsCutoffFreq_4','vsCutoffFreq_5','vsCutoffFreq_6','vsCutoffFreq_7','vsCutoffFreq_8',
    'staticDecoupler']
    parValues = [0,0,0,0,0,Data.compile('zero(192, 0)'), 0]
    for i in range(len(parNames) - 8):
      parValues.append(Data.compile('zero(192, 0.)'))
    parValues.append(Data.compile('diagonal(zero(192,0.)+1)'))
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

    for i in range(len(parNames), 512):
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
    print 'PARTS FATTE!!!'


