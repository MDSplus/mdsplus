# -*- coding: iso-8859-1 -*-
from MDSplus import *
import time
import MARTE_GENERIC

class MARTE_EDA1(MARTE_GENERIC.MARTE_GENERIC):
    print 'MARTe EDA1'
    parNames = ['ccType', 'psConfiguration','ccKp','ccTEnd','tokccTStart','tokccVMax','rfpccIpStar','rfpccDeltaIpStar','rfpccDeltaTRampDown',
    	'rfpccPOhmMax','rfpccTauz','rfpccTaup','rfpccDeltaTBumpless','aaGain','invAAGain','rfpcc2VrtStar','rfpcc2PCATMaxOnTime','bvGain',
	'maxPVATCurr','decouplerGain','compResGain','Kp','Ki','tStartEquilIntegralAction','minIpCurr','equilNonlinearFactorSaturation',
	'fsMMFRT','fsBvRT','ffwdOn','fdbkOn','voltageControl','equiFlux','strainAlarmLatchTime','tokccVLoopThreshold ','tokccVRogThreshold ',
        'EllipticityControl_ON', 'TStartEllipticityControl','TEndEllipticityControl','IfsCos2_distr','ElliIfsCos2RefSat', 'ElliFBON',
        'TStartEllipticityIntegralAction','ElliIfsCos2IntSat','KiElli','KpElli','KdElli','ElliFFON','ElliFFGAIN','LeadLagElliON','TriangularityControl_ON',
        'TStartTriangularityControl', 'TEndTriangularityControl','IfsCos3_distr','TriangIfsCos3RefSat','TriangFBON','TStartTriangularityIntegralAction',
        'TriangIfsCos3IntSat','KiTriang','KpTriang','KdTriang','TriangFFON','TriangFFGAIN','LeadLagTriangON']
    parValues = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,Data.compile('zero(8, 0.)'),Data.compile('zero(8, 0.)'),0,0,0,0,0,0,0,
        0,0.3,1.5,Data.compile('[ 0.9239,0.3827,-0.3827,-0.9239,-0.9239,-0.3827,0.3827,0.9239]'),500, 0,0.3, 300, 1500,7.5,
        0, 0, 5.649E-5, 0,   0,0.3,1.5,Data.compile('[0.8315,-0.1951,-0.9808,-0.5556,0.5556,0.9808,0.1951,-0.8315]'),500,0,0.3,300,150,7.5,0,0,5950,0] 
    parts = []
    for i in range(len(MARTE_GENERIC.MARTE_GENERIC.parts)):
      parts.append(MARTE_GENERIC.MARTE_GENERIC.parts[i])
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

    print 'FATTI PARAMS'

    parts.append({'path':'.WAVE_PARAMS', 'type':'structure'})
    waveParNames = ['bvAddRef','deltaIpRef','ipRef','normIpRef','iFSAddRef_1','iFSAddRef_2','iFSAddRef_3','iFSAddRef_4','iFSAddRef_5',
        'iFSAddRef_6','iFSAddRef_7','iFSAddRef_8','deltaIFSAddRef_1','deltaIFSAddRef_2','deltaIFSAddRef_3','deltaIFSAddRef_4',
	'deltaIFSAddRef_5','deltaIFSAddRef_6','deltaIFSAddRef_7','deltaIFSAddRef_8','deltaHRef','pmatRef','pcatRef','tfatRef',
	'fRef','btwRef','qRef', 'RplaCos2Ref', 'RplaCos3Ref']  
    parts.append({'path':'.WAVE_PARAMS:NUM_ACTIVE', 'type':'numeric', 'value':len(waveParNames)})
    print i, ' WAVES'
    for i in range(len(waveParNames)):
      print 'WAVE', i
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
    print 'PARTS FATTE!!!'

