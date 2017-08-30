from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64
from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p
import os
import time
import sys, traceback

class NI6259AI(Device):
    """NI PXI-6259 M-series multi functional data acquisition card"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':INPUT_MODE', 'type':'text', 'value':'RSE'},
        {'path':':CLOCK_MODE', 'type':'text', 'value':'INTERNAL'},
        {'path':':CLOCK_FREQ', 'type':'numeric', 'value':1000},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':1000},
        {'path':':SEG_LENGTH', 'type':'numeric', 'value':10000},
        {'path':':CLOCK_SOURCE', 'type':'numeric'}]
    for i in range(0,32):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:STATE'%(i+1), 'type':'text', 'value':'ENABLED'})
        parts.append({'path':'.CHANNEL_%d:POLARITY'%(i+1), 'type':'text', 'value':'BIPOLAR'})
        parts.append({'path':'.CHANNEL_%d:RANGE'%(i+1), 'type':'text', 'value':'10V'})
        parts.append({'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
    del(i)
    parts.append({'path':':END_IDX', 'type':'numeric'})
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})

    parts.append({'path':':START_IDX', 'type':'numeric', 'value':0})
    parts.append({'path':':ACQ_MODE', 'type':'text','value':'TRANSIENT REC.'})
    parts.append({'path':':TRIG_MODE', 'type':'text','value':'INTERNAL'})
    parts.append({'path':':TRIG_SOURCE', 'type':'numeric','value':0})

    parts.append({'path':':USE_TIME', 'type':'text', 'value':'YES'})
    parts.append({'path':':START_TIME', 'type':'numeric','value':0})
    parts.append({'path':':END_TIME', 'type':'numeric','value':1})


#File descriptor
    fd = 0

#Driver constants
    AI_CHANNEL_TYPE_CALIBRATION = c_int(0)
    AI_CHANNEL_TYPE_DIFFERENTIAL = c_int(1)
    AI_CHANNEL_TYPE_NRSE = c_int(2)
    AI_CHANNEL_TYPE_RSE = c_int(3)
    AI_POLARITY_BIPOLAR = c_int(0)
    AI_POLARITY_UNIPOLAR = c_int(1)
    AI_SAMPLE_SELECT_PFI0 = c_int(1)
    AI_SAMPLE_SELECT_SI_TC = c_int(0)
    AI_SAMPLE_POLARITY_RISING_EDGE = c_int(0)
    AI_CONVERT_SELECT_SI2TC = c_int(0)
    AI_CONVERT_POLARITY_RISING_EDGE = c_int(1)

    AI_START_SELECT_PULSE  = c_int(0)
    AI_START_SELECT_PFI1 = c_int(2)

    AI_START_SELECT_RTSI0 = c_int(11)
    AI_START_SELECT_RTSI1 = c_int(12)

    #AI_START_SELECT = c_int(9)
    #AI_START_POLARITY = c_int(10)
    AI_START_SELECT = c_int(10)
    AI_START_POLARITY = c_int(11)

    AI_START_POLARITY_RISING_EDGE = c_int(0)

    AI_REFERENCE_SELECT_PULSE  = c_int(0)
    AI_REFERENCE_SELECT_PFI1 = c_int(2)
    #AI_REFERENCE_SELECT = c_int(11)
    #AI_REFERENCE_POLARITY = c_int(12)
    AI_REFERENCE_SELECT = c_int(12)
    AI_REFERENCE_POLARITY = c_int(13)
    AI_REFERENCE_POLARITY_RISING_EDGE = c_int(0)

    PXI6259_AI_START_TRIGGER = c_int(3)
    PXI6259_RTSI1 = c_int(3)
    PXI6259_RTSI2 = c_int(4)

#Dictionaries and maps
    inputModeDict = {'RSE':AI_CHANNEL_TYPE_RSE, 'NRSE':AI_CHANNEL_TYPE_NRSE, 'DIFFERENTIAL':AI_CHANNEL_TYPE_DIFFERENTIAL}
    enableDict = {'ENABLED':True , 'DISABLED':False}
    gainDict = {'10V':c_byte(1), '5V':c_byte(2),'2V':c_byte(3), '1V':c_byte(4),'500mV':c_byte(5),'200mV':c_byte(6),'100mV':c_byte(7)}
    polarityDict = {'UNIPOLAR':AI_POLARITY_UNIPOLAR, 'BIPOLAR':AI_POLARITY_BIPOLAR}
    diffChanMap = [0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23]
    nonDiffChanMap = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]
    niLib = None
    niInterfaceLib = None
    ni6259Fds = {}
    workers = {}

#saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        NI6259AI.ni6259Fds[self.nid] = self.fd

    def restoreInfo(self):
        if NI6259AI.niLib is None:
            NI6259AI.niLib = CDLL("libpxi6259.so")
        if NI6259AI.niInterfaceLib is None:
            NI6259AI.niInterfaceLib = CDLL("libNiInterface.so")

        if self.nid in NI6259AI.ni6259Fds.keys():
            self.fd = NI6259AI.ni6259Fds[self.nid]
        else:
            try:
                boardId = self.board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                fileName = '/dev/pxi6259.'+str(boardId)+'.ai';
                self.fd = os.open(fileName, os.O_RDWR);
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        try:
            if( niLib.pxi6259_reset_ai(self.fd) ):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        return

    def closeInfo(self):
        if self.nid in NI6259AI.ni6259Fds.keys():
            self.fd = NI6259AI.ni6259Fds[self.nid]
            del(NI6259AI.ni6259Fds[self.nid])
            try:
                if NI6259AI.niLib.pxi6259_reset_ai(self.fd):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ self.fd)
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ self.fd)
                raise mdsExceptions.TclFAILED_ESSENTIAL
            os.close(self.fd)
        return

################################### Worker Management
    def saveWorker(self):
        NI6259AI.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in NI6259AI.workers.keys():
            self.worker = NI6259AI.workers[self.nid]
        else:
            print( 'Cannot restore worker!!')

########################AsynchStore class
    class AsynchStore(Thread):
        def configure(self, device, fd, chanMap, hwChanMap, treePtr, stopAcq):
            self.device = device
            self.fd = fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr
            self.stopAcq = stopAcq
            self.stopReq = False

        def run(self):
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            #counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data();
            acqMode = self.device.acq_mode.data()
            transientRec = False

            sampleToSkip = self.device.start_idx.data()
            #endIdx = self.device.end_idx.data()

            startTime = float( self.device.start_time.data() )
            #trigSource = self.device.trig_source.data()
            clockSource = self.device.clock_source.evaluate()

            frequency = float( clockSource.getDelta() )


            if( acqMode == 'TRANSIENT REC.'):
                transientRec = True

            chanFd = []
            chanNid = []

            coeff_array = c_float*4
            coeff = coeff_array();


            #NI6259AI.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));

            for chan in range(len(self.chanMap)):
                try:
                    #print 'CHANNEL', self.chanMap[chan]+1
                    #print '/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]])
                    currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)

                    chanNid.append( getattr(self.device, 'channel_%d_data'%(self.chanMap[chan]+1)).getNid() )
                    #print "chanFd "+'channel_%d_data'%(self.chanMap[chan]+1), chanFd[chan], " chanNid ", chanNid[chan]


                    gain = getattr(self.device, 'channel_%d_range'%(self.chanMap[chan]+1)).data()
                    self.device.gainDict[gain]

                    #status = NI6259AI.niInterfaceLib.pxi6259_getCalibrationParams(currFd, gain_code, coeff)
                    status = 0
                    if( status < 0 ):
                        Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536'%(str(self.chanMap[chan])) )
                        coeff[0] = coeff[2] = coeff[3] = 0
                        coeff[1] = c_float( gain / 65536. )

                    #print "coeff ", coeff

                except:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open Channel '+ str(self.chanMap[chan]))
                    return

            if( not transientRec ):

                if(bufSize > segmentSize):
                    segmentSize = bufSize
                else:
                    c = segmentSize/bufSize
                    if (segmentSize % bufSize > 0):
                        c = c+1
                    segmentSize = c*bufSize

                numSamples  = -1
                print("PXI 6259 CONTINUOUS ", numSamples)

            else:
                NI6259AI.niInterfaceLib.setStopAcqFlag(self.stopAcq);

                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data()
                except:
                    numSamples = 0

                    print("PXI 6259 NUM SAMPLES ", numSamples)


            status = NI6259AI.niLib.pxi6259_start_ai(c_int(self.fd))

            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return


            saveList = c_void_p(0)
            NI6259AI.niInterfaceLib.startSave(byref(saveList))
            #count = 0

            chanNid_c = (c_int * len(chanNid) )(*chanNid)
            chanFd_c = (c_int * len(chanFd) )(*chanFd)

            #timeAt0 = trigSource + startTime
            #print("PXI 6259 TIME AT0 ", numSamples)            
            timeAt0 = startTime

            while not self.stopReq:
                status = NI6259AI.niInterfaceLib.pxi6259_readAndSaveAllChannels(c_int(len(self.chanMap)), chanFd_c, c_int(bufSize), c_int(segmentSize), c_int(sampleToSkip), c_int(numSamples), c_float( timeAt0 ), c_float(frequency), chanNid_c, self.device.clock_source.getNid(), self.treePtr, saveList, self.stopAcq)

   ##Check termination
                if ( numSamples > 0 or (transientRec and status == -1) ):
                    self.stopReq = True


            if( transientRec and status == -1 ):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PXI 6259 Module is not in stop state')

            status = NI6259AI.niLib.pxi6259_stop_ai(c_int(self.fd))

            for chan in range(len(self.chanMap)):
                os.close(chanFd[chan])
            #print 'ASYNCH WORKER TERMINATED'
            NI6259AI.niInterfaceLib.stopSave(saveList)
            NI6259AI.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            self.device.closeInfo()
            return

        def stop(self):
            self.stopReq = True
            NI6259AI.niInterfaceLib.setStopAcqFlag(self.stopAcq);



#############End Inner class AsynchStore

##########init############################################################################
    def init(self):

        print('================= PXI 6259 Init ===============')

        self.restoreInfo()
        aiConf = c_void_p(0)
        NI6259AI.niInterfaceLib.pxi6259_create_ai_conf_ptr(byref(aiConf))
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            numChannels = 16
        else:
            numChannels = 32

        activeChan = 0;
        for chan in range(0, numChannels):
                    
            #Empy the node which will contain  the segmented data   
            getattr(self, 'channel_%d_data'%(chan+1)).deleteData()

            getattr(self, 'channel_%d_data'%(chan+1)).setCompressOnPut(False)
            try:
                enabled = self.enableDict[getattr(self, 'channel_%d_state'%(chan+1)).data()]
                polarity = self.polarityDict[getattr(self, 'channel_%d_polarity'%(chan+1)).data()]
                gain = self.gainDict[getattr(self, 'channel_%d_range'%(chan+1)).data()]
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan + 1))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            if(enabled):
                if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
                    currChan = self.diffChanMap[chan]
                else:
                    currChan = chan
                #print 'POLARITY: ' + str(polarity) + ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = NI6259AI.niLib.pxi6259_add_ai_channel(aiConf, c_byte(currChan), polarity, gain, inputMode, c_byte(0))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot add channel '+str(currChan + 1))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                print('PXI 6259 CHAN '+ str(currChan+1) + ' CONFIGURED')
                activeChan = activeChan + 1
        #endfor

        """
        try:
            nSamples = self.num_samples.data()
        except:
            nSamples = -1
        """

#Acquisition management
        try:
            acqMode = self.acq_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve acquisition mode management')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#trigger mode
        try:
            trigMode = self.trig_mode.data()
            print(trigMode)
            if(trigMode == 'EXTERNAL_PFI1' or  trigMode == 'EXTERNAL_RTSI1' or trigMode == 'EXT_PFI1_R_RTSI1'):
                #print "AI_START_SELECT ", self.AI_START_SELECT
                #print "aiConf ", aiConf
                #print "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                #print "niLib ", NI6259AI.niLib
                #print "AI_START_POLARITY ", self.AI_START_POLARITY
                #print "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE

                if(acqMode == 'TRANSIENT REC.'):
                    """
                    status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
                    print "status ", status
                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PFI1)
                        print "status ", status
                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
                        print "status ", status
                    if( status != 0 ):
                        print "status ", status
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    """
                    if( trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXT_PFI1_R_RTSI1' ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)
                    else:
                        print("1 OK AI_START_SELECT_RTSI1")
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)
                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                        print "status ", status
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL                    
                    
                else:
                    if( trigMode == 'EXT_PFI1_R_RTSI1' ):
                         status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)
                    else:
                         print("2 OK AI_START_SELECT_RTSI1")
                         status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)

                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL


                if( trigMode == 'EXT_PFI1_R_RTSI1' ):
                    status = NI6259AI.niLib.pxi6259_export_ai_signal( aiConf, self.PXI6259_AI_START_TRIGGER,  self.PXI6259_RTSI1 )
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot route PFI1 signal to RTSI1')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                #print "AI_START_SELECT ", self.AI_START_SELECT
                #print "aiConf ", aiConf
                #print "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                #print "niLib ", NI6259AI.niLib
                #print "AI_START_POLARITY ", self.AI_START_POLARITY
                #print "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE
                #print "acqMode ", acqMode

                if(acqMode == 'TRANSIENT REC.'):
                    #status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
                    #if( status == 0 ):
                    status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PULSE)
                    #if( status == 0 ):
                    #    status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL

        except:
            traceback.print_exc(file=sys.stdout)
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid triger mode definition')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#trigger source
        try:
            if(trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXTERNAL_RTSI1'):
                trigSource = self.trig_source.data()
            else:
                trigSource = 0
            print('PXI 6259 Trigger source: ', trigSource)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#clock mode
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if( ( activeChan == 1 and frequency > 1250000 ) or ( activeChan > 1 and frequency > 1000000./activeChan ) ):
                    print('PXI 6259 Frequency out of limits')
                    if( activeChan == 1 ):
                        frequency = 1250000.
                    else:
                        frequency = 1000000./activeChan
                    self.clock_source.putData(frequency)


                divisions = int(20000000./frequency)
                status = NI6259AI.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(divisions), c_int(3), self.AI_SAMPLE_SELECT_SI_TC, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Sample Clock')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                """
                if nSamples > 0:
                    clockSource = Range(Float64(0), Float64(nSamples * divisions/20000000.) , Float64(divisions/20000000.))
                else:
                    clockSource = Range(Float64(0), Float64(3600), Float64(divisions/20000000.))
                """
                clockSource = Range(None, None, Float64(divisions/20000000.))
                print('PXI 6259 CLOCK: ', clockSource)
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
                status = NI6259AI.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(16), c_int(3), self.AI_SAMPLE_SELECT_PFI0, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot configure device clock')
                    raise mdsExceptions.TclFAILED_ESSENTIAL

            status = NI6259AI.niLib.pxi6259_set_ai_convert_clk(aiConf, c_int(20), c_int(3), self.AI_CONVERT_SELECT_SI2TC, self.AI_CONVERT_POLARITY_RISING_EDGE)
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Convert Clock')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid clock definition')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#Time management
        if acqMode == 'TRANSIENT REC.':
            try:
                useTime = self.use_time.data()

            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve time or samples management')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            if useTime == 'YES':
                try:
                    startTime = self.start_time.data()
                    endTime = self.end_time.data()

                    print('PXI 6259 startTime = ', startTime)
                    print('PXI 6259 endTime   = ', endTime)
                    print('PXI 6259 trigSource   = ', trigSource)


                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')
                    raise mdsExceptions.TclFAILED_ESSENTIAL


                startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), startTime)

                endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), endTime)


                """
                if endTime > 0:
                    endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), endTime + trigSource)
                else:
                    endIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + endTime), clockSource), trigSource)

                print 'endIdx   = ', Int32(int(endIdx))
                self.end_idx.putData(Int32(int(endIdx)))

                if startTime > 0:
                    startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), startTime + trigSource)
                else:
                    startIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + startTime), clockSource), trigSource)
                """
                print('PXI 6259 startIdx = ', Int32(int(startIdx + 0.5)))
                self.start_idx.putData(Int32(int(startIdx + 0.5)))

                print('PXI 6259 endIdx   = ', Int32(int(endIdx + 0.5)))
                self.end_idx.putData(Int32(int(endIdx + 0.5)))


                #self.prts.putData(Int32(int(preTrigger)))
                #self.num_samples.putData(Int32(int(postTrigger)))

            else:
                endIdx = self.end_idx.data()
                startIdx = self.start_idx.data()

                """
                postTrigger = nSamples;
                preTrigger = 0
                try:
                    preTrigger = self.prts.data()
                except:
                    preTrigger = 0
                    nSamples =  postTrigger + preTrigger
                """


            nSamples = endIdx - startIdx + 1

            postTrigger = nSamples + startIdx
            #preTrigger  = nSamples - endIdx

            if startIdx >= 0:
                    status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(startIdx + nSamples), 0, 0)
            else:
                if trigSource > startTime:
                    print('PXI 6259 Acquire only post trigger')
                    nSamples = postTrigger
                    startIdx = 0
                    self.start_idx.putData(Int32(int(0)))
                    self.start_time.putData(Float32(trigSource))
                    status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)

            """
            nSamples = endIdx - startIdx + 1
            postTrigger = nSamples + startIdx
            preTrigger = nSamples - endIdx
            """
            print('PXI 6259 nSamples   = ', Int32(int(nSamples)))
            self.seg_length.putData(Int32(int(nSamples)))

#           status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(postTrigger), c_int(preTrigger), 0)

        else: #Continuous Acquisition
            nSamples = -1
            #if nSamples > 0:
            #    status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)

        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Number of Samples')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        status = NI6259AI.niLib.pxi6259_load_ai_conf(c_int(self.fd), aiConf)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load configuration')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        """
        if acqMode == 'TRANSIENT REC.':
            status = niLib.pxi6259_start_ai(c_int(self.fd))
            #status = 0

            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
        """
        self.saveInfo()
        print("===============================================")
        return 1

##########StartStore
    def start_store(self):
        self.restoreInfo()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        chanMap = []
        stopAcq = c_void_p(0)
        NI6259AI.niInterfaceLib.getStopAcqFlag(byref(stopAcq));
        print(stopAcq)
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            numChannels = 16
        else:
            numChannels = 32
        for chan in range(0, numChannels):
            try:
                enabled = self.enableDict[getattr(self, 'channel_%d_state'%(chan+1)).data()]
                if enabled:
                    chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan + 1))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        treePtr = c_void_p(0)
        NI6259AI.niInterfaceLib.openTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            self.worker.configure(self, self.fd, chanMap, self.diffChanMap, treePtr, stopAcq)
        else:
            self.worker.configure(self, self.fd, chanMap, self.nonDiffChanMap, treePtr, stopAcq)
        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        return 1

    def stop_store(self):
      print("PXI 6259 stop_store")
      self.restoreWorker()
      if self.worker.isAlive():
          print("PXI 6259 stop_worker")
          self.worker.stop()
      return 1

    def readConfig(self):
      self.restoreInfo()
      try:
          NI6259AI.niInterfaceLib.readAiConfiguration(c_int(self.fd))
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read board configuration')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      return 1

    def trigger(self):
      self.restoreInfo()
      try:
            status = NI6259AI.niLib.pxi6259_start_ai(c_int(self.fd))
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                raise mdsExceptions.TclFAILED_ESSENTIAL
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception Cannot Start Acquisition')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      return 1
