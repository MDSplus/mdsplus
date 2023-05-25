from MDSplus import *
import numpy as np


class SPIDER_FFT(Device):

    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':BURST_SIZE', 'type': 'numeric'},
        {'path': ':ACQ_FREQ', 'type': 'numeric'},
        {'path': ':TRIG_FREQ', 'type': 'numeric'},
        {'path': ':TRIG_TIME', 'type': 'numeric'},
    ]
    for i in range(8):
        parts.extend([
            {'path': '.SIGNAL_%d' % (i+1), 'type': 'structure'},
            {'path': '.SIGNAL_%d:IN_SIGNAL' % (i+1), 'type': 'numeric'},
            {'path': '.SIGNAL_%d:IN_RANGE' % (i+1), 'type': 'numeric','value': 1.},
            {'path': '.SIGNAL_%d:IN_GAIN' % (i+1), 'type': 'numeric','value': 1.},
            {'path': '.SIGNAL_%d:IN_OFFSET' % (i+1), 'type': 'numeric','value': 0.},
        ])
        for j in range(5):
            parts.extend([
                {'path': '.SIGNAL_%d.HARMONIC_%d' % (i+1,j+1), 'type': 'structure'},
                {'path': '.SIGNAL_%d.HARMONIC_%d:FREQUENCY' % (i+1,j+1), 'type': 'signal'},
                {'path': '.SIGNAL_%d.HARMONIC_%d:AMPLITUDE' % (i+1,j+1), 'type': 'signal'},
                {'path': '.SIGNAL_%d.HARMONIC_%d:PHASE' % (i+1,j+1), 'type': 'signal'},
            ])
    parts.extend([
        {'path': ':STORE_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('SPIDER_SERVER','STORE',50,None),Method(None,'store',head))",
         'options': ('no_write_shot',)},
    ])
  
    OUT_SEGMENT_SIZE = 10
    def store(self):
        try:
            burstSize = round(self.burst_size.data())
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Burst Size specified ')
            raise DevBAD_PARAMETER
        try:
            acqFreq = self.acq_freq.data()
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Sampling Frequency specified ')
            raise DevBAD_PARAMETER
        try:
            trigTime = float(self.trig_time.data())
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Trigger Time specified ')
            raise DevBAD_PARAMETER
        try:
            trigFreq = float(self.trig_freq.data())
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Trigger Frequency specified ')
            raise DevBAD_PARAMETER
          
        for sigIdx in range(8):
            print('Doing '+str(getattr(self, 'signal_%d_in_signal' % (sigIdx+1))))
            try:
                inNode = getattr(self, 'signal_%d_in_signal' % (sigIdx+1)).getData()
            except:
                print('No Data Available for signal '+str(sigIdx+1))
                continue
            if(not isinstance(inNode, TreeNode)):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'In Signal must be a tree node reference')
                raise DevBAD_PARAMETER
            try:
                inRange = getattr(self, 'signal_%d_in_range' % (sigIdx+1)).data()
            except: 
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Input Range specified for signal '+str(sigIdx + 1))
                raise DevBAD_PARAMETER
            try:
                inGain = getattr(self, 'signal_%d_in_gain' % (sigIdx+1)).data()
            except: 
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Input Gain specified for signal '+str(sigIdx + 1))
                raise DevBAD_PARAMETER
            try:
                inOffset = getattr(self, 'signal_%d_in_offset' % (sigIdx+1)).data()
            except: 
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'No Input Offset specified for signal '+str(sigIdx + 1))
                raise DevBAD_PARAMETER

            nSegments = inNode.getNumSegments()
            if(nSegments == 0):
                print('No Data Available for signal '+str(sigIdx+1))
                continue
            freqNodes = []
            phaseNodes = []
            amplNodes = []
            for i in range(5):
                freqNode = getattr(self, 'signal_%d_harmonic_%d_frequency' % (sigIdx+1, i+1))
                freqNode.deleteData()
                freqNodes.append(freqNode)
                phaseNode = getattr(self, 'signal_%d_harmonic_%d_phase' % (sigIdx+1, i+1))
                phaseNode.deleteData()
                phaseNodes.append(phaseNode)
                amplNode = getattr(self, 'signal_%d_harmonic_%d_amplitude' % (sigIdx+1, i+1))
                amplNode.deleteData()
                amplNodes.append(amplNode)
            outCount = 0
            outSegCount = 0
            for segIdx in range(nSegments):
                print('Doing Segment '+str(segIdx) +' of ' + str(nSegments))
                segData = inNode.getSegment(segIdx).data()
                segData = segData * inRange*inGain/8192. + inOffset
                numBursts = int(round(len(segData)/burstSize))
                outFreqSegs = []
                outAmplSegs = []
                outPhaseSegs = []
                for hIdx in range(5):
                    outFreqSegs.append([])
                    outAmplSegs.append([])
                    outPhaseSegs.append([])
                for burstIdx in range(numBursts):
                    fftData = np.fft.fft(segData[int(burstIdx*burstSize):int((burstIdx+1)*burstSize)])
                    fftAmp = np.abs(fftData)
                    fftAmp = fftAmp[:len(fftAmp)/2]  #keep only significant part
                    fftPhs = np.angle(fftData)
                    fftPhs = fftPhs[:len(fftPhs)/2]  #keep only significant part
                    maxIdxs = []
                    for hIdx in range(5):
                        maxIdx = np.argmax(fftAmp)
                        origOffset = 0
                        for currMax in maxIdxs:
                            if currMax > maxIdx:
                                origOffset = origOffset + 1  #keep trak of the shift that may occur removing the index
                        maxIdxs.append(maxIdx)
                        outFreqSegs[hIdx].append(acqFreq * float(maxIdx + origOffset) / burstSize)
                        outAmplSegs[hIdx].append(fftAmp[maxIdx])
                        outPhaseSegs[hIdx].append(fftPhs[maxIdx])
                        fftAmp = np.delete(fftAmp, maxIdx)
                        fftPhs = np.delete(fftPhs, maxIdx)
                    outCount = outCount + 1
                    if outCount > SPIDER_FFT.OUT_SEGMENT_SIZE:
                      startTime = Float64(trigTime + outSegCount * SPIDER_FFT.OUT_SEGMENT_SIZE / trigFreq)
                      endTime = Float64(trigTime + (outSegCount + 1) * SPIDER_FFT.OUT_SEGMENT_SIZE / trigFreq)
                      dim = Range(startTime, endTime, Float64(1./trigFreq))
                      for hIdx in range(5):
                          freqNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outFreqSegs[hIdx]))
                          phaseNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outPhaseSegs[hIdx]))
                          amplNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outAmplSegs[hIdx]))
                          
                          outFreqSegs[hIdx] = []
                          outPhaseSegs[hIdx] = []
                          outAmplSegs[hIdx] = []
                      outSegCount = outSegCount + 1
                      outCount = 0
             #end segIdx in range(nSegments)
            if outCount > 0:   #if last segment not yet written
                startTime = Float64(trigTime + outSegCount * SPIDER_FFT.OUT_SEGMENT_SIZE / trigFreq)
                endTime = Float64(trigTime + (outSegCount * SPIDER_FFT.OUT_SEGMENT_SIZE  + outCount)/ trigFreq)
                dim = Range(startTime, endTime, Float64(1./trigFreq))
                for hIdx in range(5):
                    freqNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outFreqSegs[hIdx]))
                    phaseNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outPhaseSegs[hIdx]))
                    amplNodes[hIdx].makeSegment(startTime, endTime, dim, Float64Array(outAmplSegs[hIdx]))
