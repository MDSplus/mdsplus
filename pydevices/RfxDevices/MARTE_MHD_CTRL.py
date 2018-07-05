# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

try:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals(), level=1).MARTE_GENERIC
except:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals()).MARTE_GENERIC

class MARTE_MHD_CTRL(MARTE_GENERIC):
    parNames = ['cleaningEnable', 'extrapolationEnable', 'reconfigurationEnable', 'dynamicDecouplerEnable', 'staticDecouplerEnable', 'applyCleaning', 'extrapolationRadius',
    'rgM_1', 'rgN_1', 'rgFreq_1', 'rgAmpl_1','rgPhase_1','rgTStart_1','rgRiseTime_1','rgTEnd_1','rgFallTime_1','rgThreshold_1','rgIsRelative_1',
    'rgM_2', 'rgN_2', 'rgFreq_2', 'rgAmpl_2','rgPhase_2','rgTStart_2','rgRiseTime_2','rgTEnd_2','rgFallTime_2','rgThreshold_2','rgIsRelative_2',
    'rgM_3', 'rgN_3', 'rgFreq_3', 'rgAmpl_3','rgPhase_3','rgTStart_3','rgRiseTime_3','rgTEnd_3','rgFallTime_3','rgThreshold_3','rgIsRelative_3',
    'rgM_4', 'rgN_4', 'rgFreq_4', 'rgAmpl_4','rgPhase_4','rgTStart_4','rgRiseTime_4','rgTEnd_4','rgFallTime_4','rgThreshold_4','rgIsRelative_4',
    'rgM_5', 'rgN_5', 'rgFreq_5', 'rgAmpl_5','rgPhase_5','rgTStart_5','rgRiseTime_5','rgTEnd_5','rgFallTime_5','rgThreshold_5','rgIsRelative_5',
    'rgM_6', 'rgN_6', 'rgFreq_6', 'rgAmpl_6','rgPhase_6','rgTStart_6','rgRiseTime_6','rgTEnd_6','rgFallTime_6','rgThreshold_6','rgIsRelative_6',
    'rgM_7', 'rgN_7', 'rgFreq_7', 'rgAmpl_7','rgPhase_7','rgTStart_7','rgRiseTime_7','rgTEnd_7','rgFallTime_7','rgThreshold_7','rgIsRelative_7',
    'rgM_8', 'rgN_8', 'rgFreq_8', 'rgAmpl_8','rgPhase_8','rgTStart_8','rgRiseTime_8','rgTEnd_8','rgFallTime_8','rgThreshold_8','rgIsRelative_8',
    'ffM_1', 'ffN_1', 'ffFreq_1', 'ffAmpl_1','ffPhase_1','ffTStart_1','ffRiseTime_1','ffTEnd_1','ffFallTime_1','ffThreshold_1','ffIsRelative_1',
    'ffM_2', 'ffN_2', 'ffFreq_2', 'ffAmpl_2','ffPhase_2','ffTStart_2','ffRiseTime_2','ffTEnd_2','ffFallTime_2','ffThreshold_2','ffIsRelative_2',
    'ffM_3', 'ffN_3', 'ffFreq_3', 'ffAmpl_3','ffPhase_3','ffTStart_3','ffRiseTime_3','ffTEnd_3','ffFallTime_3','ffThreshold_3','ffIsRelative_3',
    'ffM_4', 'ffN_4', 'ffFreq_4', 'ffAmpl_4','ffPhase_4','ffTStart_4','ffRiseTime_4','ffTEnd_4','ffFallTime_4','ffThreshold_4','ffIsRelative_4',
    'ffM_5', 'ffN_5', 'ffFreq_5', 'ffAmpl_5','ffPhase_5','ffTStart_5','ffRiseTime_5','ffTEnd_5','ffFallTime_5','ffThreshold_5','ffIsRelative_5',
    'ffM_6', 'ffN_6', 'ffFreq_6', 'ffAmpl_6','ffPhase_6','ffTStart_6','ffRiseTime_6','ffTEnd_6','ffFallTime_6','ffThreshold_6','ffIsRelative_6',
    'ffM_7', 'ffN_7', 'ffFreq_7', 'ffAmpl_7','ffPhase_7','ffTStart_7','ffRiseTime_7','ffTEnd_7','ffFallTime_7','ffThreshold_7','ffIsRelative_7',
    'ffM_8', 'ffN_8', 'ffFreq_8', 'ffAmpl_8','ffPhase_8','ffTStart_8','ffRiseTime_8','ffTEnd_8','ffFallTime_8','ffThreshold_8','ffIsRelative_8',
    'verFieldControlExclude_1', 'kpAmplitudes_1','kpPhases_1','ki_1','kd_1','tStart_1','tEnd_1','cutoffFreq_1',
    'verFieldControlExclude_2', 'kpAmplitudes_2','kpPhases_2','ki_2','kd_2','tStart_2','tEnd_2','cutoffFreq_2',
    'verFieldControlExclude_3', 'kpAmplitudes_3','kpPhases_3','ki_3','kd_3','tStart_3','tEnd_3','cutoffFreq_3',
    'verFieldControlExclude_4', 'kpAmplitudes_4','kpPhases_4','ki_4','kd_4','tStart_4','tEnd_4','cutoffFreq_4',
    'verFieldControlExclude_5', 'kpAmplitudes_5','kpPhases_5','ki_5','kd_5','tStart_5','tEnd_5','cutoffFreq_5',
    'verFieldControlExclude_6', 'kpAmplitudes_6','kpPhases_6','ki_6','kd_6','tStart_6','tEnd_6','cutoffFreq_6',
    'verFieldControlExclude_7', 'kpAmplitudes_7','kpPhases_7','ki_7','kd_7','tStart_7','tEnd_7','cutoffFreq_7',
    'verFieldControlExclude_8', 'kpAmplitudes_8','kpPhases_8','ki_8','kd_8','tStart_8','tEnd_8','cutoffFreq_8',
    'vsKpAmplitudes_1','vsKpPhases_1','vsKp_1', 'vsKi_1','vsKd_1','vsTStart_1','vsTEnd_1','vsCutoffFreq_1',
    'vsKpAmplitudes_2','vsKpPhases_2','vsKp_2', 'vsKi_2','vsKd_2','vsTStart_2','vsTEnd_2','vsCutoffFreq_2',
    'vsKpAmplitudes_3','vsKpPhases_3','vsKp_3', 'vsKi_3','vsKd_3','vsTStart_3','vsTEnd_3','vsCutoffFreq_3',
    'vsKpAmplitudes_4','vsKpPhases_4','vsKp_4', 'vsKi_4','vsKd_4','vsTStart_4','vsTEnd_4','vsCutoffFreq_4',
    'vsKpAmplitudes_5','vsKpPhases_5','vsKp_5', 'vsKi_5','vsKd_5','vsTStart_5','vsTEnd_5','vsCutoffFreq_5',
    'vsKpAmplitudes_6','vsKpPhases_6','vsKp_6', 'vsKi_6','vsKd_6','vsTStart_6','vsTEnd_6','vsCutoffFreq_6',
    'vsKpAmplitudes_7','vsKpPhases_7','vsKp_7', 'vsKi_7','vsKd_7','vsTStart_7','vsTEnd_7','vsCutoffFreq_7',
    'vsKpAmplitudes_8','vsKpPhases_8','vsKp_8', 'vsKi_8','vsKd_8','vsTStart_8','vsTEnd_8','vsCutoffFreq_8',
    'staticDecoupler', 'outCoilEnabled', 'supercoilIdx', 'reconfiguredModes',
    'refCutoffFreq_1','refCutoffFreq_2','refCutoffFreq_3','refCutoffFreq_4','refCutoffFreq_5','refCutoffFreq_6','refCutoffFreq_7','refCutoffFreq_8',
    'freezeStartTime', 'freezeEndTime', 'startBpAutozero', 'endBpAutozero']
    parValues = ( ['0','0','0','0','0','ZERO(192,0.)', '0'] +
                (['0']*88)*2 + # References and FF References
                (['0']+['ZERO(192,0.)']*4+['0']*3)*8 +  # Mode Controller
                (['ZERO(192,0.)']*5+['0']*3)*8 +  # VS Controller
                 ['DIAGONAL(ADD(ZERO(192,0.),1.))','ADD(ZERO(192,0.),1.)','ADD(ZERO(192,0.),-1.)','ZERO(192,0.)'] +
                 ['0']*8 + ['-1']*4)  # Start and end of freeze and Bp Autozero time
    parts = list(MARTE_GENERIC.parts)
    parts.append({'path':'.PARAMS', 'type':'structure'})
    parts.append({'path':'.PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(parNames)})
    for i in range(len(parNames)):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text', 'value':parNames[i]})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric','valueExpr':parValues[i]})

    for i in range(len(parNames), 512):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric'})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':0})
    for i in range(192):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text', 'value':'inFF_%03d'%(i+1)})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})
    del(parNames,parValues,i)
