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

from MDSplus import mdsExceptions, Data
try:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals(), level=1).MARTE_GENERIC
except:
    MARTE_GENERIC = __import__('MARTE_GENERIC', globals()).MARTE_GENERIC

class MARTE_EDA1(MARTE_GENERIC):
    parNames = ['ccType', 'psConfiguration','ccKp','ccTEnd','tokccTStart','tokccVMax','rfpccIpStar','rfpccDeltaIpStar','rfpccDeltaTRampDown',
    	'rfpccPOhmMax','rfpccTauz','rfpccTaup','rfpccDeltaTBumpless','aaGain','invAAGain','rfpcc2VrtStar','rfpcc2PCATMaxOnTime','bvGain',
	'maxPVATCurr','decouplerGain','compResGain','Kp','Ki','tStartEquilIntegralAction','minIpCurr','equilNonlinearFactorSaturation',
	'fsMMFRT','fsBvRT','ffwdOn','fdbkOn','voltageControl','equiFlux','strainAlarmLatchTime','tokccVLoopThreshold ','tokccVRogThreshold ',
        'EllipticityControl_ON', 'TStartEllipticityControl','TEndEllipticityControl','IfsCos2_distr','ElliIfsCos2RefSat', 'ElliFBON',
        'TStartEllipticityIntegralAction','ElliIfsCos2IntSat','KiElli','KpElli','KdElli','ElliFFON','ElliFFGAIN','LeadLagElliON','TriangularityControl_ON',
        'TStartTriangularityControl', 'TEndTriangularityControl','IfsCos3_distr','TriangIfsCos3RefSat','TriangFBON','TStartTriangularityIntegralAction',
        'TriangIfsCos3IntSat','KiTriang','KpTriang','KdTriang','TriangFFON','TriangFFGAIN','LeadLagTriangON','VerticalShiftControl_ON','TStartVerticalShiftControl',
        'TEndVerticalShiftControl','VertShiftFBON','VertShiftSel','TStartVertShiftIntegralAction','VertShiftIntSat','KiVertShift','KpVertShift','KdVertShift','VertShiftFFON',
        'VertShiftFFGAIN','LeadLagVertShiftON','ResAddFcoil','I2tResFcoil','DNSTT0','DNSTT1','Tok2DnEnable','LqgControlOn','TStartLQGControl','TEndLQGControl',
        'LqgComResOn','LqgFbkON','LqgRefOn','LqgFbkKproGain','LqgFfwOn','LqgFfwGain', 'ToffFbkShiftH', 'condTriggerTimes', 'condTriggerThresholds', 'DisruptionDetectionEnable',
        'TokTstartCheckMode','TokThresholdBpmode','TokIpLowQ','TokTunIpFR','TokVpcatRampUp','TokTunVpcat','IFS_DN','LQGIpfilterOn','LQGContIpnormOn','LQGEquiIpNormOn','TokFastRampUpIpOn']

    parValues = (['0']*26+['ZERO(8,0.)']*2 + ['0']*8 +
        ['0.3','1.5','Float32Array([0.9239,0.3827,-0.3827,-0.9239,-0.9239,-0.3827,0.3827,0.9239])','500','0','0.3','300','1500','7.5','0','0','5.649E-5','0','0'] +
        ['0.3','1.5','Float32Array([0.8315,-0.1951,-0.9808,-0.5556,0.5556,0.9808,0.1951,-0.8315])','500','0','0.3','300','150' ,'7.5','0','0','5950'    ,'0','0'] + ['0']*12 +
        ['Float32Array([.15,.3,.15,.3,.15,.3,.3,.3])','Float32Array([9000E3,2250E3,6000E3,2250E3,6000E3,1500E3,1500E3,2250E3])','.3','.5','0.','0.','.55','.8','0','1','0','1.','0','1.','0.'] +
        ['Float32Array([0.,0.,0.,0.])']*2 + ['0','10.2','100E-9','160000','.95','1000','1','Int32Array([-2746,-889,4100,0,-1692,-1157,-206,426])','1','1','1','0'])
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

    for i in range(len(parNames), 256):
      parts.append({'path':'.PARAMS:PAR_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.PARAMS:PAR_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:TYPE'%(i+1), 'type':'text'})
      parts.append({'path':'.PARAMS:PAR_%03d:DIMS'%(i+1), 'type':'numeric'})
      parts.append({'path':'.PARAMS:PAR_%03d:DATA'%(i+1), 'type':'numeric'})

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    waveParNames = ['bvAddRef','deltaIpRef','ipRef','normIpRef','iFSAddRef_1','iFSAddRef_2','iFSAddRef_3','iFSAddRef_4','iFSAddRef_5',
        'iFSAddRef_6','iFSAddRef_7','iFSAddRef_8','deltaIFSAddRef_1','deltaIFSAddRef_2','deltaIFSAddRef_3','deltaIFSAddRef_4',
	'deltaIFSAddRef_5','deltaIFSAddRef_6','deltaIFSAddRef_7','deltaIFSAddRef_8','deltaHRef','pmatRef','pcatRef','tfatRef',
	'fRef','btwRef','qRef', 'RplaCos2Ref', 'RplaCos3Ref', 'avgRplasmaRef', 'REF_DELTA_V']
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(waveParNames)})
    for i in range(len(waveParNames)):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text', 'value':waveParNames[i]})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric', 'valueExpr':'Float32Array([0.,1.])'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric', 'valueExpr':'Float32Array([0.,0.])'})

    for i in range(len(waveParNames), 64):
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:X'%(i+1), 'type':'numeric'})
      parts.append({'path':'.WAVE_PARAMS:WAVE_%03d:Y'%(i+1), 'type':'numeric'})
    del(parNames,parValues,i)
