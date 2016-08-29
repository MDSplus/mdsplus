from MDSplus import *
from numpy import *
from threading import *
import time
import os
import errno
from ctypes import *
class NI6259AI(Device):
    print 'NI6259AI'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
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
    del i
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
    AI_START_SELECT_PFI0 = c_int(1)
    AI_START_SELECT_PFI1 = c_int(2)

    AI_START_SELECT_RTSI0 = c_int(11)
    AI_START_SELECT_RTSI1 = c_int(12)

    AI_START_SELECT = c_int(9)
    AI_START_POLARITY = c_int(10)
    AI_START_POLARITY_RISING_EDGE = c_int(0)

    AI_REFERENCE_SELECT_PULSE  = c_int(0)
    AI_REFERENCE_SELECT_PFI1 = c_int(2)
    AI_REFERENCE_SELECT = c_int(11)
    AI_REFERENCE_POLARITY = c_int(12)
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
#saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
      global ni6259Fds
      global ni6259Nids

      try:
        ni6259Fds
      except:
        ni6259Fds = []
        ni6259Nids = []
      try:
        idx = ni6259Nids.index(self.getNid())
      except:
        #print 'SAVE INFO: SAVING HANDLE'
        ni6259Fds.append(self.fd)
        ni6259Nids.append(self.getNid())
        return
      return

    def restoreInfo(self):
        global ni6259Fds
        global ni6259Nids
        global niLib
        global niInterfaceLib
        try:
            niLib
        except:
            niLib = CDLL("libni6259.so")
            niInterfaceLib = CDLL("libNiInterface.so")
        try:
            idx = ni6259Nids.index(self.getNid())
            self.fd = ni6259Fds[idx]
            #print 'RESTORE INFO HANDLE FOUND'
        except:
            #print 'RESTORE INFO HANDLE NOT FOUND'
            try: 
                boardId = self.board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                return 0
            try:
                fileName = '/dev/pxi6259.'+str(boardId)+'.ai';
                self.fd = os.open(fileName, os.O_RDWR);
                #print 'Open fd: ', self.fd
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ fileName)
                return 0
	"""
        try: 
	    if( niLib.pxi6259_reset_ai(self.fd) ):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                return 0
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
            return 0
	"""
        return 1

    def closeInfo(self):
        global ni6259Fds
        global ni6259Nids
        try:
            idx = ni6259Nids.index(self.getNid())
            self.fd = ni6259Fds[idx]
            ni6259Fds.remove(self.fd)
            ni6259Nids.remove(self.getNid())

            try: 
	        if( niLib.pxi6259_reset_ai(self.fd) ):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                    return 0
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                return 0

            os.close(self.fd)
        except:
            print ' '
#            print 'CLOSE INFO: HANDLE NOT FOUND'
        return 1

################################### Worker Management
    def saveWorker(self):
      global ni6259Workers
      global ni6259WorkerNids
      try:
        ni6259Workers
      except:
        ni6259WorkerNids = []
        ni6259Workers = []
      try:
        #print 'Find worker'
        idx = ni6259WorkerNids.index(self.getNid())
        #print 'Find worker ', idx
        ni6259Workers[idx] = self.worker
        #print 'Find worker ', self.worker
      except:
        #print 'SAVE WORKER: NEW WORKER'
        ni6259WorkerNids.append(self.getNid())
        ni6259Workers.append(self.worker)
        return
      return

    def restoreWorker(self):
      global ni6259WorkerNids
      global ni6259Workers
      
      try:
        #print "restoreWorker"
        idx = ni6259WorkerNids.index(self.getNid())
        #print "restoreWorker idx ", idx
        self.worker = ni6259Workers[idx]
        #print "restoreWorker worker ", self.worker
      except:
        print 'Cannot restore worker!!'

########################AsynchStore class
    class AsynchStore(Thread):
        stopReq = False
        #stopAcq = c_void_p(0)            

        def configure(self, device, niLib, niInterfaceLib, fd, chanMap, hwChanMap, treePtr, stopAcq):
            self.device = device
            self.niLib = niLib
            self.niInterfaceLib = niInterfaceLib
            self.fd = fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr
            self.stopAcq = stopAcq
                


        def run(self):
            import os
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data();
            acqMode = self.device.acq_mode.data()
            transientRec = False

            sampleToSkip = self.device.start_idx.data()
            endIdx = self.device.end_idx.data()

            startTime = float( self.device.start_time.data() )
            trigSource = self.device.trig_source.data()
	    clockSource = self.device.clock_source.evaluate()

	    frequency = float( clockSource.getDelta() )


            if( acqMode == 'TRANSIENT REC.'):
                transientRec = True

            chanFd = []
            chanNid = []

            coeff_array = c_float*4
            coeff = coeff_array();


            #self.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));

            for chan in range(len(self.chanMap)):
                try:
                    #print 'CHANNEL', self.chanMap[chan]+1
                    #print '/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]])
                    currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)

                    chanNid.append( getattr(self.device, 'channel_%d_data'%(self.chanMap[chan]+1)).getNid() )
                    #print "chanFd "+'channel_%d_data'%(self.chanMap[chan]+1), chanFd[chan], " chanNid ", chanNid[chan]  


                    gain = getattr(self.device, 'channel_%d_range'%(self.chanMap[chan]+1)).data()
                    gain_code = self.device.gainDict[gain]

                    #status = self.niInterfaceLib.pxi6259_getCalibrationParams(currFd, gain_code, coeff)
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
                print "PXI 6259 CONTINUOUS ", numSamples 

            else:
                self.niInterfaceLib.setStopAcqFlag(self.stopAcq);
                
                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data()
                except:
                    numSamples = 0

	            print "PXI 6259 NUM SAMPLES ", numSamples
                

            status = self.niLib.pxi6259_start_ai(c_int(self.fd))
       
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return


            saveList = c_void_p(0)
            self.niInterfaceLib.startSave(byref(saveList))
            count = 0

            chanNid_c = (c_int * len(chanNid) )(*chanNid)
            chanFd_c = (c_int * len(chanFd) )(*chanFd)

	    #timeAt0 = trigSource + startTime
            timeAt0 = startTime           
             
            while not self.stopReq:
                status = self.niInterfaceLib.pxi6259_readAndSaveAllChannels(c_int(len(self.chanMap)), chanFd_c, c_int(bufSize), c_int(segmentSize), c_int(sampleToSkip), c_int(numSamples), c_float( timeAt0 ), c_float(frequency), chanNid_c, self.device.clock_source.getNid(), self.treePtr, saveList, self.stopAcq)


   ##Check termination
                if ( numSamples > 0 or (transientRec and status == -1) ):
                    self.stopReq = True
                

            if( transientRec and status == -1 ):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PXI 6259 Module is not in stop state')

            status = self.niLib.pxi6259_stop_ai(c_int(self.fd))

            for chan in range(len(self.chanMap)):
                os.close(chanFd[chan])
            #print 'ASYNCH WORKER TERMINATED'
            self.niInterfaceLib.stopSave(saveList)
            self.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            self.device.closeInfo()
            return

        def stop(self):
            self.stopReq = True
            self.niInterfaceLib.setStopAcqFlag(self.stopAcq);



#############End Inner class AsynchStore      
    
##########init############################################################################    
    def init(self,arg):

        print '================= PXI 6259 Init ==============='

        global niLib
        global niInterfaceLib
        self.restoreInfo()
        aiConf = c_void_p(0)
        niInterfaceLib.pxi6259_create_ai_conf_ptr(byref(aiConf))
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            return 0
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            numChannels = 16
        else:
            numChannels = 32

        activeChan = 0;
        for chan in range(0, numChannels):
            getattr(self, 'channel_%d_data'%(chan+1)).setCompressOnPut(False)
            try:
                enabled = self.enableDict[getattr(self, 'channel_%d_state'%(chan+1)).data()]
                polarity = self.polarityDict[getattr(self, 'channel_%d_polarity'%(chan+1)).data()]
                gain = self.gainDict[getattr(self, 'channel_%d_range'%(chan+1)).data()]
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan + 1))
                return 0
            if(enabled):
                if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
                    currChan = self.diffChanMap[chan]
                else:
                    currChan = chan
                #print 'POLARITY: ' + str(polarity) + ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = niLib.pxi6259_add_ai_channel(aiConf, c_byte(currChan), polarity, gain, inputMode, c_byte(0))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot add channel '+str(currChan + 1))
                    return 0
                print 'PXI 6259 CHAN '+ str(currChan+1) + ' CONFIGURED'
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
            return 0

#trigger mode        
        try:
            trigMode = self.trig_mode.data()
            print trigMode
            #if(trigMode == 'EXTERNAL_PFI1' or  trigMode == 'EXTERNAL_RTSI1' ):
            if(trigMode == 'EXTERNAL' or  trigMode == 'EXTERNAL_RTSI1' ):
                #print "AI_START_SELECT ", self.AI_START_SELECT
                #print "aiConf ", aiConf
                #print "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                #print "niLib ", niLib
                #print "AI_START_POLARITY ", self.AI_START_POLARITY
                #print "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE
                
                if(acqMode == 'TRANSIENT REC.'):
                    """
                    status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
                    if( status == 0 ):
                        status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PFI1)
                    if( status == 0 ):                    
                        status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        return 0
                    """
                    # Sostituzione temporanea per gestire acquisizione a IPP trigger preso dal 6368 
                    #status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)


                    #if( trigMode == 'EXTERNAL_PFI1' ): 
                    if( trigMode == 'EXTERNAL' ): 
                         print "0 OK AI_START_SELECT_PFI0"
                         status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI0)
                    else:
                         print "1 OK AI_START_SELECT_RTSI1"
                         status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)

                    if( status == 0 ):                    
                        print "2 OK POLARITY_RISING_EDGE"
                        status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                    print "status ", status
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger: (%d) %s' % (errno, os.strerror( errno )) )
                        return 0                    
                else:
                    # Sostituzione etmporanea per gestire acquisizione a IPP trigger preso dal 6368  
                    # status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)

                    #if( trigMode == 'EXTERNAL_PFI1' ): 
                    if( trigMode == 'EXTERNAL' ): 
                         print "2 OK AI_START_SELECT_PFI0"
                         status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI0)
                    else:
                         print "2 OK AI_START_SELECT_RTSI1"
                         status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)

                    if( status == 0 ):                    
                        status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        return 0


                """  
                if( trigMode == 'EXTERNAL_PFI1' ): 
                    status = niLib.pxi6259_export_ai_signal( aiConf, self.PXI6259_AI_START_TRIGGER,  self.PXI6259_RTSI1 )
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot route PFI1 signal to RTSI1')
                        return 0
                """

	    else:
                #print "AI_START_SELECT ", self.AI_START_SELECT
                #print "aiConf ", aiConf
                #print "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                #print "niLib ", niLib
                #print "AI_START_POLARITY ", self.AI_START_POLARITY
                #print "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE
		#print "acqMode ", acqMode

                if(acqMode == 'TRANSIENT REC.'):
                    #status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
		    #print "acqMode ", acqMode
                    #if( status == 0 ):                    
                    status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PULSE)
 		    #print "acqMode ", acqMode
                    #if( status == 0 ):                    
                    #    status = niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
  	            #    print "acqMode ", acqMode
                    if( status != 0 ):
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        return 0
	
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid triger mode definition')
            return 0

#trigger source    
        try:
            #if(trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXTERNAL_RTSI1'):
            if(trigMode == 'EXTERNAL' or trigMode == 'EXTERNAL_RTSI1'):
                trigSource = self.trig_source.data()
            else:
                trigSource = 0
            print 'PXI 6259 Trigger source: ', trigSource
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')          
            return 0

#clock mode      
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if( ( activeChan == 1 and frequency > 1250000 ) or ( activeChan > 1 and frequency > 1000000./activeChan ) ):
                    print 'PXI 6259 Frequency out of limits'
                    if( activeChan == 1 ):
                        frequency = 1250000.
                    else:
                        frequency = 1000000./activeChan
                    self.clock_source.putData(frequency)
                  
    
                divisions = int(20000000./frequency)
                #print 'PXI 6259 Division = ', divisions
                status = niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(divisions), c_int(3), self.AI_SAMPLE_SELECT_SI_TC, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Sample Clock')
                    return 0
                """
                if nSamples > 0:
                    clockSource = Range(Float64(0), Float64(nSamples * divisions/20000000.) , Float64(divisions/20000000.))
                else:
                    clockSource = Range(Float64(0), Float64(3600), Float64(divisions/20000000.))
                """
                clockSource = Range(None, None, Float64(divisions/20000000.))
                print 'PXI 6259 CLOCK: ', clockSource
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
                status = niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(16), c_int(3), self.AI_SAMPLE_SELECT_PFI0, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot configure device clock')
                    return 0

            status = niLib.pxi6259_set_ai_convert_clk(aiConf, c_int(20), c_int(3), self.AI_CONVERT_SELECT_SI2TC, self.AI_CONVERT_POLARITY_RISING_EDGE)
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Convert Clock')
                return 0
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid clock definition')
            return 0

#Time management
        if acqMode == 'TRANSIENT REC.':
            try:
                useTime = self.use_time.data()

            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve time or samples management')          
                return 0

            if useTime == 'YES':
                try:
                    startTime = self.start_time.data()
                    endTime = self.end_time.data()

                    print 'PXI 6259 startTime = ', startTime 
                    print 'PXI 6259 endTime   = ', endTime
                    print 'PXI 6259 trigSource   = ', trigSource


                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')          
                    return 0


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
                print 'PXI 6259 startIdx = ', Int32(int(startIdx + 0.5))
                self.start_idx.putData(Int32(int(startIdx + 0.5)))

                print 'PXI 6259 endIdx   = ', Int32(int(endIdx + 0.5))
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
            preTrigger  = nSamples - endIdx

            if startIdx >= 0:
            	status = niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(startIdx + nSamples), 0, 0)
            else:
		if trigSource > startTime: 
                    print 'PXI 6259 Acquire only post trigger'
                    nSamples = postTrigger
		    startIdx = 0
                    self.start_idx.putData(Int32(int(0)))
                    self.start_time.putData(Float32(trigSource))
            	status = niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)

            #print 'numSamples     = ', Int32(int(numSamples))

            """
            nSamples = endIdx - startIdx + 1
            postTrigger = nSamples + startIdx
            preTrigger = nSamples - endIdx
            """
            print 'PXI 6259 nSamples   = ', Int32(int(nSamples))
            self.seg_length.putData(Int32(int(nSamples)))
	             
#           status = niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(postTrigger), c_int(preTrigger), 0)

        else: #Continuous Acquisition
            nSamples = -1
            #if nSamples > 0:
            #    status = niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)


        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Number of Samples')
            return 0

        status = niLib.pxi6259_load_ai_conf(c_int(self.fd), aiConf)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load configuration')
            return 0


        """
        if acqMode == 'TRANSIENT REC.':
            status = niLib.pxi6259_start_ai(c_int(self.fd))
            #status = 0
       
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
        """

        self.saveInfo()
        
        print "==============================================="
        return 1

##########StartStore
    def start_store(self, arg):
        global niLib
        global niInterfaceLib
        self.restoreInfo()
        self.worker = self.AsynchStore()        
        self.worker.daemon = True 
        self.worker.stopReq = False
        chanMap = []

        stopAcq = c_void_p(0)
        niInterfaceLib.getStopAcqFlag(byref(stopAcq));
        print stopAcq

        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            return 0
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
                return 0
        treePtr = c_void_p(0)
        status = niInterfaceLib.openTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            self.worker.configure(self, niLib, niInterfaceLib, self.fd, chanMap, self.diffChanMap, treePtr, stopAcq)
        else:
            self.worker.configure(self, niLib, niInterfaceLib, self.fd, chanMap, self.nonDiffChanMap, treePtr, stopAcq)

        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,arg):
      print "PXI 6259 stop_store"
      self.restoreWorker()
      if self.worker.isAlive():
          print "PXI 6259 stop_worker"
          self.worker.stop()
      return 1
               

    def readConfig(self,arg):
      global niLib
      global niInterfaceLib
      self.restoreInfo()

      try:
          niInterfaceLib.readAiConfiguration(c_int(self.fd))
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read board configuration')
          return 0

      return 1
      

    def trigger(self,arg):
      global niLib
      global niInterfaceLib
      self.restoreInfo()

      try:
            status = niLib.pxi6259_start_ai(c_int(self.fd))
            #status = 0
           
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception Cannot Start Acquisition')
          return 0

      return 1

