from MDSplus import *
import numpy as np
from threading import Lock
import socket

class TRIG_HUB(Device):
    NUM_INPUTS = 6       
    SAMPLE_NS = 8
    lock = Lock()
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':DEVICE_IP', 'type': 'text'},
        {'path': ':RP_SIG_PATH', 'type': 'any'},
        {'path': ':RAW_TIMES', 'type': 'signal'}, # For every trigger, the straight computation of relative delays starting from 0
        {'path': ':CORR_TIMES', 'type': 'signal'},# The above, including delay corrections  
        {'path': ':TRUE_TIMES', 'type': 'signal'},# The relative delays aligned to the root HUB
        {'path': ':TIME_OFFSETS', 'type': 'numeric', 'value':0}, # Time offsets from root HUB
        {'path': ':TIMES', 'type': 'numeric', 'value':0}, # Segment start time from root HUB
        

        {'path': ':PARENT_IDX', 'type': 'numeric', 'value': -1}, # -1 means Hub Root
        {'path': ':CHILD0_IDX', 'type': 'numeric', 'value': -1}, # -1 means No child
        {'path': ':CHILD1_IDX', 'type': 'numeric', 'value': -1}, # -1 means No child
        {'path': ':CHILD2_IDX', 'type': 'numeric', 'value': -1}, # -1 means No child
        {'path': ':CHILD3_IDX', 'type': 'numeric', 'value': -1}, # -1 means No child
    ]

    for i in range(NUM_INPUTS): 
        parts.extend([       
            {'path': '.IN_%d' % (i),'type': 'structure'},
            {'path': '.IN_%d:DELAY' % (i), 'type': 'numeric', 'value': 0},
            {'path': '.IN_%d:HUB_PATH' % (i), 'type': 'any'}, # Only for children HUBs
            {'path': '.IN_%d:IN_SIG' % (i), 'type': 'any'}, # Only for redPitaya
            {'path': '.IN_%d:OUT_SIG' % (i), 'type': 'signal'}, # Only for redPitaya
        ])
    parts.append( {'path': ':CONF_COMMAND', 'type': 'text'})
 
    parts.extend([
        {'path': ':START_UPD', 'type': 'action',
         'valueExpr': "Action(Dispatch('RP_SERVER','PULSE_ON',50,None),Method(None,'startUpdate',head))",
         'options': ('no_write_shot',)},
        {'path': ':STOP_UPD', 'type': 'action',
         'valueExpr': "Action(Dispatch('RP_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stopUpdate',head))",
         'options': ('no_write_shot',)},
    ])
    del(i)

    def getTriggerRelTimes(self, trigSignal):
        #125 MHz Sampling Speed -> 8ns period
        mask = int(0)
        times = np.zeros(NUM_INPUTS, dtype = int)
        firstTimeIdx = -1
        for sample in range(len(trigSignal)):
            if (trigSignal[sample] & ~mask) != 0:
                newBits = trigSignal[sample] & ~mask
                mask = mask | newBits
                for idx in range(NUM_INPUTS):
                    if ((newBits >> idx)&0x1)!=0:  #A new trigger has been recorded
                        if firstTimeIdx < 0:
                            firstTimeIdx = sample #keep track of the index within the signal of the first received trigger (there may be pre trigger samples)
                        times[idx] = (sample - firstTimeIdx) * SAMPLE_NS # Record time offset in ns from thre first recorded trigger
        return times

    def configure(self):
        cmd = self.conf_command.data()
        if len(cmd) != 80:
            print('The lengh of the command string  shall be 80 chars')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            ipAddr = self.device_ip.data().split()
        except:
            print('Missing device IP')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        ip = ipAddr[0]
        if len(ipAddr)>1:
            port = int(ipAddr[1])
        else:
            port = 5500
        try:
            s = sockte(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(ip, port)
            s.sendall(b'25')
            s.sendall(bytes(cmd, 'utf-8'))
        except:
            print('Cannot communicate to device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            s.sendall(b'30')
            chunks = []
            bytesRec = 0
            while bytesRec < 80:
                chunk = self.sock.recv(80 - bytesRec)
                if chunk == b'':
                    raise RuntimeError("socket connection broken")
                chunks.append(chunk)
                bytesRec = bytesRec + len(chunk)
            if b''.join(chunks).decode() != cmd:
                print('Invalid message readout: ', b''.join(chunks).decode(), cmd)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            print('Cannot read from device')
            raise mdsExceptions.TclFAILED_ESSENTIAL



    def update(self):
        TRIG_HUB.lock.acquire()
        try:
            numSavedTriggers = len(self.raw_times.data())
        except:
            numSavedTriggers = 0
        try:
            rpSigNode = self.rp_sig_path.getData()
        except:
            print('Cannot get SetPitaya Raw Signal node')
            TRIG_HUB.lock.release()
            raise mdsExceptions.TclFAILED_ESSENTIAL
        numRpSegments = rpSigNode.getNumSegments()
        for i in range(numSavedTriggers, numRpSegments):
            currTrigSignal = rpSigNode.getSegment(i).data()
            rawTimes = self.getTriggerRelTimes(currTrigSignal)
            delays = np.zeros(NUM_INPUTS, dtype = int)
            for inIdx in range(NUM_INPUTS):
                delays[inIdx] = self.__getattr__('in_%d_delay' % (inIdx)).data()
            corrTimes = rawTimes - delays
            self.raw_times.putRow(rawTimes, i)
            self.corr_times.putRow(corrTimes, i)
            if self.parent_idx.data() < 0:
                self.true_times.putRow(corrTimes, i) #For the root HUB no offset is applied
                startTime = rpSigNode.getSegmentStart(i).data()
                for child in range(4):
                    childIdx =self.__getattr__('child_%d' % (child)).data()
                    if childIdx < 0: #children finished
                        break
                    currChildHub = self.__getattr__('in_%d_hub_path' % (child)).getData() #It must be a child HUB
                    currOffset = corrTimes[childIdx]
                    currChildHub.time_offsets.putRow(currOffset, i)
                    currChildHub.times.putRow(startTime, i)

        # second step, affecting only non root HUBs            
        if self.parent_idx.data() >= 0: 
            try:
                numTrueTimes = len(self.true_times.data())
            except:
                numTrueTimes = 0
            try:
                numCorrTimes = len(self.corr_times.data())
            except:
                numCorrTimes = 0
            try:
                numOffsets = len(self.time_offsets.data())
            except:
                numOffsets = 0
            if numCorrTimes > 0 and numOffsets > numTrueTimes:
                offsets = self.time_offsets.data() # 1D Array
                corrTimes = self.corr_times.data() # 2D Array
                times = self.times.data() # 1D Array, t will hase as many elements as offsets
                for timeIdx in range(numTrueTimes, numOffsets):
                    if timeIdx < numCorrTimes: #We need to already have the corresponding corrected time array
                        trueTimesInst = corrTimes[timeIdx] + offsets[timeIdx] # 1D Array + Scalar -> 1D Array
                        self.true_times.putRow(trueTimesInst, timeIdx)
                        #propagate offsets to children HUBs
                        for child in range(4):
                            childIdx =self.__getattr__('child_%d' % (child)).data()
                            if childIdx < 0: #children finished
                                break
                            currOffsetInts = trueTimesInst[childIdx]
                            currChildHub = self.__getattr__('in_%d_hub_path' % (child)).getData() #It must be a HUB path
                            currChildHub.time_offsets.putRow(currOffsetInst, timeIdx) #The index is the same of the index in trueTimes here
                            currChildHub.times.putRow(times[timeIdx], timeIdx)
                     
            # third step, valid for all: add adjusted segment for inputs from RP
            times = self.times.data() #1 D Array
            trueTimes = self.trueTimes.data() #2 D Array
            if len(times) != len(trueTimes):
                print('INTERNAL ERROR different length in times and trueTimes: ', len(times), len(trueTimes))
                TRIG_HUB.lock.release()
                raise mdsExceptions.TclFAILED_ESSENTIAL
 
            for chanIdx in range(NUM_INPUTS):
                try:
                    inSigNode = self.__getattr__('in_%d_in_sig' % (chanIdx))
                except:
                    continue # Not RP
                inSegments = inSigNode.getNumSegments()
                if len(trueTimes) > inSegments:
                    print('INTERNAL ERROR different length for RP signal segments and trueTimes: ', numSegments, len(trueTimes))
                    TRIG_HUB.lock.release()
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                outSegments = self.__getattr__('in_%d_out_sig' % (chanIdx)).getNumSegments()
                for segIdx in range(outSegments, inSegments):
                    if segIdx >= len(trueTimes):
                        break # No more true time information
                    currSigData = inSigNode.getSegment(segIdx).data()
                    currStart = times[segIdx] + trueTimes[segIdx][chanIdx]
                    currEnd = currStart + len(currSigData) * SAMPLE_NS * 1E-9
                    currDelta = SAMPLE_NS * 1E-9
                    dim = Range(Float64(currStart), Float64(currEnd),Float64(SAMPLE_NS * 1E-9))
                    self.__getattr__('in_%d_out_sig' % (chanIdx)).makeSegment(Float64(currStart), Float64(currEnd), dim, Int32Array(currSigData))
            TRIG_HUB.lock.release()



