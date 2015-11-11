from MDSplus import Data
try:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals(), level=1).MARTE_GENERIC
except:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals()).MARTE_GENERIC

class MARTE_EDA3(MARTE_GENERIC):
    print('MARTE_EDA3')
    parNames = ['tauVTheta','tauVTor','tauD','kp','ki','kd','vCompOn','iCompOn','iFBOn','iPTrig','tTrig',
       'deltaVTorTrig','deltaTBT1','deltaTBT2','rVesselTuningFactor', 'thetaNom,', 'tTrigVth', 'deltaTBT1Vth', 'DeadTimeCompOn', 'kDeadTimeComp', 'DeTiMinHystCurrThreshold', 'DeTiMaxHystCurrThreshold']
    parValues = [0.002,0.0005,0.002,1.367,0.,0.,0.,1.,1.,0.,0.002,0.,0.01,0.01,1.,1.42,0, 0, 0,0,0,0]
    parts = list(MARTE_GENERIC.parts)
    parts.append({'path':'.PARAMS', 'type':'structure'})
    parts.append({'path':'.PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(parNames)})
    for i in range(len(parNames)):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text', 'value':parNames[i]})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric','value':parValues[i]})

    for i in range(len(parNames), 256):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric'})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    waveParNames = ['ppTorInvCurrentRefs_1','ppTorInvCurrentRefs_2','ppTorInvCurrentRefs_3','ppTorInvCurrentRefs_4','ppTorInvCurrentRefs_5',
        'ppTorInvCurrentRefs_6','ppTorInvCurrentRefs_7','ppTorInvCurrentRefs_8','ppTorInvCurrentRefs_9','ppTorInvCurrentRefs_10',
        'ppTorInvCurrentRefs_11','ppTorInvCurrentRefs_12','capacitorsVoltageRefs_1','capacitorsVoltageRefs_2','capacitorsVoltageRefs_3',
        'capacitorsVoltageRefs_4','capacitorsVoltageRefs_5',
        'capacitorsVoltageRefs_6','capacitorsVoltageRefs_7','capacitorsVoltageRefs_8','capacitorsVoltageRefs_9','capacitorsVoltageRefs_10',
        'capacitorsVoltageRefs_11','capacitorsVoltageRefs_12', 'fRef',
        'ppTorInvVoltageRefs_1','ppTorInvVoltageRefs_2','ppTorInvVoltageRefs_3','ppTorInvVoltageRefs_4','ppTorInvVoltageRefs_5',
        'ppTorInvVoltageRefs_6','ppTorInvVoltageRefs_7','ppTorInvVoltageRefs_8','ppTorInvVoltageRefs_9','ppTorInvVoltageRefs_10',
        'ppTorInvVoltageRefs_11','ppTorInvVoltageRefs_12']

    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(waveParNames)})
    for i in range(len(waveParNames)):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text', 'value':waveParNames[i]})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric', 'value':Data.compile('[0.,1.]')})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric', 'value':Data.compile('[0.,0.]')})

    for i in range(len(waveParNames), 64):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})
