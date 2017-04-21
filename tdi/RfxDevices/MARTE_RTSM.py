try:
    MARTE_COMMON = __import__('MARTE_COMMON', globals(), level=1).MARTE_COMMON
except:
    MARTE_COMMON = __import__('MARTE_COMMON', globals()).MARTE_COMMON

class MARTE_RTSM(MARTE_COMMON):
    parNames = ['numStates', 'stateTime','stateNumNext','stateNext','stateNextMask','stateNextPattern','stateOutMode','stateOutBits', 'triggerTime', 'frequency']

    parValues = ['1']+['ZERO(8,0.)']*2+['ZERO(64,0.)']*4+['ZERO(8,0.)', '0', '1000']

    waveParNames = ['state1_wave1','state1_wave2','state1_wave3','state1_wave4','state1_wave5','state1_wave6','state1_wave7','state1_wave8',
        'state2_wave1','state2_wave2','state2_wave3','state2_wave4','state2_wave5','state2_wave6','state2_wave7','state2_wave8',
        'state3_wave1','state3_wave2','state3_wave3','state3_wave4','state3_wave5','state3_wave6','state3_wave7','state3_wave8',
        'state4_wave1','state4_wave2','state4_wave3','state4_wave4','state4_wave5','state4_wave6','state4_wave7','state4_wave8',
        'state5_wave1','state5_wave2','state5_wave3','state5_wave4','state5_wave5','state5_wave6','state5_wave7','state5_wave8',
        'state6_wave1','state6_wave2','state6_wave3','state6_wave4','state6_wave5','state6_wave6','state6_wave7','state6_wave8',
        'state7_wave1','state7_wave2','state7_wave3','state7_wave4','state7_wave5','state7_wave6','state7_wave7','state7_wave8',
        'state8_wave1','state8_wave2','state8_wave3','state8_wave4','state8_wave5','state8_wave6','state8_wave7','state8_wave8']

    parts = list(MARTE_COMMON.parts)
    parts.append({'path':'.PARAMS', 'type':'structure'})
    parts.append({'path':'.PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(parNames)})
    for i in range(len(parNames)):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text', 'value':parNames[i]})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric','valueExpr':parValues[i]})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':0})
    for i in range(len(waveParNames)):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text', 'value':waveParNames[i]})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})
    del(parNames,parValues,waveParNames,i)
