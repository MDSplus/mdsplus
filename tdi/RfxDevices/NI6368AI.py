from MDSplus import *
from numpy import *
from threading import *
import time
import os
import errno
from ctypes import *
class NI6368AI(Device):
    print 'NI6368AI'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """NI PXI-6368 X-series multi functional data acquisition card"""

    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':INPUT_MODE', 'type':'text', 'value':'RSE'},
        {'path':':CLOCK_MODE', 'type':'text', 'value':'INTERNAL'},
        {'path':':CLOCK_FREQ', 'type':'numeric', 'value':1000},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':1000},
        {'path':':SEG_LENGTH', 'type':'numeric', 'value':10000},
        {'path':':CLOCK_SOURCE', 'type':'numeric'},
        {'path':':ACQ_MODE', 'type':'text','value':'TRANSIENT REC.'},
        {'path':':TRIG_MODE', 'type':'text','value':'EXTERNAL'},
        {'path':':TRIG_SOURCE', 'type':'numeric','value':0},
        {'path':':USE_TIME', 'type':'text', 'value':'YES'},
        {'path':':START_TIME', 'type':'numeric','value':0},
        {'path':':END_TIME', 'type':'numeric','value':1},
        {'path':':START_IDX', 'type':'numeric', 'value':0},
        {'path':':END_IDX', 'type':'numeric'}]

    for i in range(0,16):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:STATE'%(i+1), 'type':'text', 'value':'ENABLED'})
        parts.append({'path':'.CHANNEL_%d:RANGE'%(i+1), 'type':'numeric', 'value':10.})
        parts.append({'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal'})
        parts.append({'path':'.CHANNEL_%d:DATA_RAW'%(i+1), 'type':'signal'})
        parts.append({'path':'.CHANNEL_%d:CALIB_PARAM'%(i+1), 'type':'numeric'})
    del i

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','READY',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})

    
#File descriptor
    ai_fd = 0
    boardId = 0

    class XSERIES_DEV_INFO(Structure):
        _fields_ = [("product_id", c_uint),
                    ("device_name", c_char * 20),
                    ("is_simultaneous", c_ubyte),
                    ("is_pcie", c_ubyte),
                    ("adc_number", c_uint),
                    ("dac_number", c_uint),
                    ("port0_length", c_uint),
                    ("serial_number", c_uint)]


#Driver constants

    XSERIES_AI_CHANNEL_TYPE_LOOPBACK        = c_int(0)
    XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL    = c_int(1)
    XSERIES_AI_CHANNEL_TYPE_NRSE            = c_int(2)
    XSERIES_AI_CHANNEL_TYPE_RSE             = c_int(3)
    XSERIES_AI_CHANNEL_TYPE_INTERNAL        = c_int(5)

    XSERIES_SAMPLE_INTERVAL_COUNTER_SI_SRC  = c_int(0)
    XSERIES_SAMPLE_INTERVAL_COUNTER_TB3     = c_int(1)

    XSERIES_AI_START_TRIGGER_SW_PULSE       = c_int(0)
    XSERIES_AI_START_TRIGGER_PFI0           = c_int(1)
    XSERIES_AI_START_TRIGGER_PFI1           = c_int(2)

    XSERIES_AI_START_TRIGGER_LOW            = c_int(31)

    XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE = c_int(0)
    XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE  = c_int(0)
    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_TB3 = c_int(0)

    XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING  = c_int(0)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0            = c_int(1)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI1            = c_int(2)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_LOW             = c_int(31)

    XSERIES_AI_EXTERNAL_GATE_DISABLED               = c_int(0)


#Dictionaries and maps
    inputModeDict = {'RSE':XSERIES_AI_CHANNEL_TYPE_RSE, 'NRSE':XSERIES_AI_CHANNEL_TYPE_NRSE, 'DIFFERENTIAL':XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL, 'LOOPBACK':XSERIES_AI_CHANNEL_TYPE_LOOPBACK, 'INTERNAL':XSERIES_AI_CHANNEL_TYPE_INTERNAL}
    enableDict = {'ENABLED':True , 'DISABLED':False}
    gainDict = {10.:c_int(0), 5.:c_int(1), 2.:c_int(2), 1.:c_int(3), 0.5:c_int(4), 0.2:c_int(5),0.1:c_int(6)}
    diffChanMap    = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
    nonDiffChanMap = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
#saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
      global ni6368AiFds
      global ni6368DevFds
      global ni6368Nids
      try:
        ni6368AiFds
      except:
        ni6368AiFds = []
        ni6368Nids = []
      try:
        idx = ni6368Nids.index(self.getNid())
      except:
        print 'SAVE INFO: SAVING HANDLE'
        ni6368AiFds.append(self.ai_fd)
        ni6368Nids.append(self.getNid())
        return
      return

    def restoreInfo(self):
        global ni6368AiFds
        global ni6368Nids
        global niLib
        global niInterfaceLib

        try:
            niLib
        except:
            niLib = CDLL("libnixseries.so")
            niInterfaceLib = CDLL("libNiInterface.so")
        try:
            idx = ni6368Nids.index(self.getNid())
            self.ai_fd = ni6368AiFds[idx]
            print 'RESTORE INFO HANDLE FOUND'
        except:
            print 'RESTORE INFO HANDLE NOT FOUND'
            try: 
                boardId = self.board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                return 0
            try:
                fileName = '/dev/PXIe-6368.'+str(boardId)+'.ai';
                self.ai_fd = os.open(fileName, os.O_RDWR);
                print 'Open ai_fd: ', self.ai_fd
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ fileName)
                return 0
        return 1

    def closeInfo(self):
        global ni6368AiFds
        global ni6368Nids
        try:
            idx = ni6368Nids.index(self.getNid())
            self.ai_fd = ni6368AiFds[idx]
            ni6368AiFds.remove(self.ai_fd)
            ni6368Nids.remove(self.getNid())
            os.close(self.ai_fd)
        except:
            print 'CLOSE INFO: HANDLE NOT FOUND'
        return 1

################################### Worker Management
    def saveWorker(self):
      global ni6368Workers
      global ni6368WorkerNids
      try:
        ni6368Workers
      except:
        ni6368WorkerNids = []
        ni6368Workers = []
      try:
        print 'Find worker'
        idx = ni6368WorkerNids.index(self.getNid())
        print 'Find worker ', idx
        ni6368Workers[idx] = self.worker
        print 'Find worker ', self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        ni6368WorkerNids.append(self.getNid())
        ni6368Workers.append(self.worker)
        return
      return

    def restoreWorker(self):
      global ni6368WorkerNids
      global ni6368Workers
      
      try:
        print "restoreWorker"
        idx = ni6368WorkerNids.index(self.getNid())
        print "restoreWorker idx ", idx
        self.worker = ni6368Workers[idx]
        print "restoreWorker worker ", self.worker
      except:
        print 'Cannot restore worker!!'

########################AsynchStore class
    class AsynchStore(Thread):
        stopReq = False
        stopAcq = c_void_p(0)
        #chanFd = []
        #chanNid = []
        #saveList = c_void_p(0)
            

        def configure(self, device, niLib, niInterfaceLib, ai_fd, chanMap, hwChanMap, treePtr):
            self.device = device
            self.niLib = niLib
            self.niInterfaceLib = niInterfaceLib
            self.ai_fd = ai_fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr


        def run(self):
            import os
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data();
            acqMode = self.device.acq_mode.data()
            sampleToSkip = self.device.start_idx.data()
            transientRec = False
            startTime = float( self.device.start_time.data() )
            trigSource = self.device.trig_source.data()
 
            try:
                #In multi trigger acquisition acquired data from trigger_time[i]+start_time up to trigger_time[i] + end_time
                #Time value to be associted at the first sample of the i-esima acquisition must be trigger_time[i]+start_time
                numTrigger = len(trigSource) - 1
                trigTime = 0.
                timesIdx0 = trigSource
            except:
                #Single trigger acquire data from start time up to end time
                #Time value to be associted at the first sample must be start time 
                numTrigger = 1
                timesIdx0 = []
                timesIdx0.append(0)     

            if( acqMode == 'TRANSIENT REC.'):
                transientRec = True


            chanFd = []
            chanNid = []
            saveList = c_void_p(0)


            self.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));

            coeff_array = c_float*4
            coeff = coeff_array();
                
            for chan in range(len(self.chanMap)):
                try:
                    #print 'CHANNEL', self.chanMap[chan]+1

                    currFd = os.open('/dev/PXIe-6368.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)
                    chanNid.append( getattr(self.device, 'channel_%d_data_raw'%(self.chanMap[chan]+1)).getNid() )    
                    #print "chanFd "+'channel_%d_data_raw'%(self.chanMap[chan]+1), chanFd[chan], " chanNid ", chanNid[chan]   
                    gain = getattr(self.device, 'channel_%d_range'%(self.chanMap[chan]+1)).data()
                    gain_code = self.device.gainDict[gain]
                    status = self.niInterfaceLib.getCalibrationParams(currFd, gain_code, coeff)
                    status = 0
                    if( status < 0 ):
                        Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536'%(str(self.chanMap[chan])) )
                        coeff[0] = coeff[2] = coeff[3] = 0  
                        coeff[1] = c_float( gain / 65536. )

                    getattr(self.device, 'channel_%d_calib_param'%(self.chanMap[chan]+1)).putData(Float32Array(coeff))

                except:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open Channel '+ str(self.chanMap[chan] + 1) )
                    self.closeAll(chanFd, saveList)
                    return 


            if(bufSize > segmentSize):
                segmentSize = bufSize
            else:
                c = segmentSize/bufSize
                if (segmentSize % bufSize > 0):
                    c = c + 1
                segmentSize = c * bufSize

            if( not transientRec ):
                numSamples  = -1
                print "-- CONTINUOUS ", numSamples 
                
            else:
                self.niInterfaceLib.setStopAcqFlag(self.stopAcq);
                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data() - 1
                except:
                    numSamples = 0

            #print "--+++ NUM SAMPLES ", numSamples
            #print "--+++ Segment Size ", segmentSize
                        
            self.niInterfaceLib.startSave(byref(saveList))

            chanNid_c = (c_int * len(chanNid) )(*chanNid)
            chanFd_c = (c_int * len(chanFd) )(*chanFd)
             
            trigCount = 0

            time.sleep(1)

            print "Start device in acquisition thread %d"%(self.ai_fd)
            status = self.niLib.xseries_start_ai(c_int(self.ai_fd))
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
           
            while not self.stopReq:
                status = self.niInterfaceLib.xseriesReadAndSaveAllChannels(c_int(len(self.chanMap)), chanFd_c, c_int(bufSize), c_int(segmentSize), c_int(sampleToSkip), c_int(numSamples), c_float( timesIdx0[trigCount] + startTime ), chanNid_c, self.device.clock_source.getNid(), self.treePtr, saveList, self.stopAcq)
   ##Check termination
                trigCount += 1
                print "Trigger count %d num %d num smp %d status %d" %(trigCount , numTrigger, numSamples, status) 
                if ( (numSamples > 0 and trigCount == numTrigger)  or ( status < 0 ) ):
                    self.stopReq = True

            #if( transientRec and status == -1 ):
            if( status < 0 ):
                if( status == -1 ):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Module is not in stop state')
                if( status == -2 ):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'DMA overflow')


            status = self.niLib.xseries_stop_ai(c_int(self.ai_fd))

            print "OK"

            """
            for chan in range(len(self.chanMap)):
                    os.close(chanFd[chan])
            print 'ASYNCH WORKER TERMINATED'
            self.niInterfaceLib.stopSave(saveList)
            self.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            self.device.closeInfo()
            """

            self.closeAll(chanFd, saveList)

            print "OK"

            return

        def stop(self):
            self.stopReq = True
            self.niInterfaceLib.setStopAcqFlag(self.stopAcq);

        def closeAll(self, chanFd, saveList):
            for chan in range(len(self.chanMap)):
                try:
                    os.close(chanFd[chan])
                except:
                    print 'Exception'
                    pass
            print 'ASYNCH WORKER TERMINATED'
            self.niInterfaceLib.stopSave(saveList)
            self.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            self.device.closeInfo()

#############End Inner class AsynchStore      
    
##########init############################################################################ 
   
    def init(self, arg):

        print 'OK Init'

        global niLib
        global niInterfaceLib

        if self.restoreInfo() == 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
            return 0

        aiConf = c_void_p(0)

        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            return 0


        print 'OK Init'

        dev_fd = 0;

        fileName = '/dev/PXIe-6368.'+str(self.boardId);
        dev_fd = os.open(fileName, os.O_RDWR);
        print 'Open ai_fd: ', self.ai_fd

        device_info = self.XSERIES_DEV_INFO(0,"",0,0,0,0,0,0)

        # get card info 
        status = niInterfaceLib._xseries_get_device_info(c_int(dev_fd), byref(device_info));
        if status:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading card information')
            return 0

        os.close(dev_fd)
        
        print 'OK xseries_get_device_info'

        #Stop the segment TODO is this necessary since the reset is next 
        niLib.xseries_stop_ai(c_int(self.ai_fd))

        #reset AI segment
        status = niLib.xseries_reset_ai(c_int(self.ai_fd))
        if ( status ):
            errno = niInterfaceLib.getErrno();
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset AI segment: (%d) %s' % (errno, os.strerror( errno )) )
            return 0

#Acquisition management
        try:
            acqMode = self.acq_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve acquisition mode management')          
            return 0

#trigger mode        
        try:
            trigMode = self.trig_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid triger mode definition')
            return 0

#trigger source    
        try:
            trigSource = ( self.trig_source.data() )
        except:
            if(trigMode == 'EXTERNAL'):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')          
                return 0
            else:
                trigSource = 0.
                self.trig_source.putData( Float32(trigSource) )
        print 'Trigger source: ', trigSource

        try:
            numTrigger = len(trigSource) - 1 
            """
            Trigger time must be set to 0. in multi trigger
            acquisition for correct evaluation of the number
            of samples to acquire for each trigger
            """
            trigTime = 0.
        except:
            numTrigger = 1
            trigTime = trigSource     
        print 'Trigger number: ', numTrigger




#clock mode      
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if( frequency > 2000000.  ):
                    print 'Frequency out of limits'
                    frequency = 2000000.
                    self.clock_source.putData(frequency)
                  
                clockSource = Range(None,None, Float64(1./frequency))
                print 'CLOCK: ', clockSource
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid clock definition')
            return 0

#Time management

        try:
            useTime = self.use_time.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve time or samples management')          
            return 0


        if acqMode == 'TRANSIENT REC.':
            if useTime == 'YES':
                try:
                    startTime = float( self.start_time.data() )
                    endTime = float( self.end_time.data() )
                    print 'startTime  = ', startTime 
                    print 'endTime    = ', endTime
                    print 'trigTime = ', trigTime

                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')          
                    return 0

                startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigTime), clockSource), startTime) 
                endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigTime), clockSource), endTime)


                print 'endIdx   = ', Int32(int(endIdx))
                self.end_idx.putData(Int32(int(endIdx)))
                

                print 'startIdx = ', Int32(int(startIdx))
                self.start_idx.putData(Int32(int(startIdx)))
                      
            else:
                endIdx = self.end_idx.data()
                startIdx = self.start_idx.data()
            
            nSamples = endIdx - startIdx

            postTrigger = nSamples + startIdx
            preTrigger = nSamples - endIdx

            print 'nSamples     = ', Int32(int(nSamples))
            print 'seg_length   = ', self.seg_length.data()


        else: #Continuous Acquisition
            if useTime == 'YES':
                try:
                    startTime = float( self.start_time.data() )
                    print 'startTime  = ', startTime 
                    print 'trigTime = ', trigTime

                    startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigTime), clockSource), startTime)

                except:
                    startIdx = 0
                self.start_idx.putData(Int32(int(startIdx)))
            else:
                startIdx = self.start_idx.data()
            nSamples = -1

		
        if acqMode == 'TRANSIENT REC.':
            if startIdx >= 0 :
                niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(startIdx + nSamples), (numTrigger))
                #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)

            else:
                print 'preTrigger    = ', Int32(int(preTrigger))
                print 'postTrigger   = ', Int32(int(postTrigger))
                print 'Acquire only post trigger'
                nSamples = postTrigger
                self.start_idx.putData(Int32(int(0)))
                niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(postTrigger), (numTrigger))
                #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)
        else:
            niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)


        """
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Number of Samples')
            return 0
        """

        #XSERIES_AI_DMA_BUFFER_SIZE = 0
        status = niLib.xseries_set_ai_attribute(aiConf, c_int(0) , c_int(1000));
        if(status != 0):
            errno = niInterfaceLib.getErrno();
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Set DMA buffer size : (%d) %s' % (errno, os.strerror( errno )) )
            return 0


#SET trigger         

        #disable external gate 
        status = niLib.xseries_set_ai_external_gate(aiConf, self.XSERIES_AI_EXTERNAL_GATE_DISABLED, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE)
        if( status != 0 ):
            errno = niInterfaceLib.getErrno();
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable external gate!: (%d) %s' % (errno, os.strerror( errno )) )
            return 0

        if(trigMode == 'EXTERNAL'):            
            #if(acqMode == 'TRANSIENT REC.'):
             print "#select start trigger External (START1 signal)" 
             status = niLib.xseries_set_ai_start_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_PFI1, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE ,1)
             #test
             #status = niLib.xseries_set_ai_reference_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_PFI1, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE ,1)


             if( status != 0 ):
                 errno = niInterfaceLib.getErrno();
                 Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger: (%d) %s' % (errno, os.strerror( errno )) )
                 return 0
                
        else:
            print "#select start trigger Internal (START1 signal)"
            #status = niLib.xseries_set_ai_start_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_SW_PULSE, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE, 0)
            status = niLib.xseries_set_ai_start_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_LOW, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE, 0)
            if( status != 0 ):
                errno = niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set auto start trigger: (%d) %s' % (errno, os.strerror( errno )) )
                return 0

#SET clock       
        if(clockMode == 'INTERNAL'):
            period = int( 100000000/frequency ) #TB3 clock 100MHz 
            print "Internal CLOCK TB3 period ", period

            status = niLib.xseries_set_ai_scan_interval_counter(aiConf, self.XSERIES_SCAN_INTERVAL_COUNTER_TB3, self.XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE, c_int(period), c_int(2));
            if(status != 0):
                errno = niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set internal sample clock: (%d) %s' % (errno, os.strerror( errno )) )
                return 0


        else:
            print "Program the sample clock (START signal) to start on a rising edge"
            status = niLib.xseries_set_ai_sample_clock(aiConf, self.XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE, c_int(1))
            if( status == 0): 
                #Program the convert to be the same as START.
                status = niLib.xseries_set_ai_convert_clock(aiConf, self.XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)
            if(status == 0): 
                #Program the sample and convert clock timing specifications
                status = niLib.xseries_set_ai_scan_interval_counter(aiConf, self.XSERIES_SCAN_INTERVAL_COUNTER_TB3, self.XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE, c_int(50),  c_int(2));    
            if(status != 0):
                errno = niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot configure external device clock: (%d) %s' % (errno, os.strerror( errno )) )
                return 0


#Channel configuration
        numChannels = 16
        activeChan = 0;
        for chan in range(1, numChannels+1):
            try:
                getattr(self, 'channel_%d_data_raw'%(chan)).setCompressOnPut(False)
                enabled = self.enableDict[getattr(self, 'channel_%d_state'%(chan)).data()]
                gain = self.gainDict[getattr(self, 'channel_%d_range'%(chan)).data()]
                #data = Data.compile("2 * build_path($) / 65536. * build_path($)", getattr(self, 'channel_%d_range'%(chan)).getPath(),  getattr(self, 'channel_%d_data_raw'%(chan)).getPath() );

                data = Data.compile("NIanalogInputScaled( build_path($), build_path($) )", getattr(self, 'channel_%d_data_raw'%(chan)).getPath(),  getattr(self, 'channel_%d_calib_param'%(chan)).getPath() );

                data.setUnits("Volts")
                getattr(self, 'channel_%d_data'%(chan)).putData(data)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan))
                return 0
            if(enabled):
                #print ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = niLib.xseries_add_ai_channel(aiConf, c_short(chan-1), gain, inputMode, c_byte(1))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot add channel '+str(chan))
                    return 0
                #print 'CHAN '+ str(chan) + ' CONFIGURED'
                activeChan = chan
            #else:
                #print 'CHAN '+ str(chan) + ' DISABLED'
        

        #endfor        

        niLib.xseries_stop_ai(c_int(self.ai_fd))

        try:
            status = niInterfaceLib.xseries_set_ai_conf_ptr(c_int(self.ai_fd), aiConf)
            #status = niLib.xseries_load_ai_conf( c_int(self.ai_fd), aiConf)
            if(status != 0):
                errno = niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load ai configuration : (%d) %s' % (errno, os.strerror( errno )) )
                return 0
        except IOError as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception: cannot load ai configuration: (%d) %s' % (errno, os.strerror( errno )) )
            return 0
        
        """
        if(acqMode == 'TRANSIENT REC.'):
            status = niLib.xseries_start_ai(c_int(self.ai_fd))
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
        """

        self.saveInfo()
       
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

        numChannels = 16
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
        #if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
        self.worker.configure(self, niLib, niInterfaceLib, self.ai_fd, chanMap, self.diffChanMap, treePtr)
        #else:
        #    self.worker.configure(self, niLib, niInterfaceLib, self.ai_fd, chanMap, self.nonDiffChanMap, treePtr)

        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,arg):
      print "stop_store"
      self.restoreWorker()
      if self.worker.isAlive():
          print "stop_worker"
          self.worker.stop()
      return 1
               
    """
    def readConfig(self,arg):
      global niLib
      global niInterfaceLib
      self.restoreInfo()

      try:
          niInterfaceLib.readAiConfiguration(c_int(self.ai_fd))
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read board configuration')
          return 0
      return 1
    """  

    def trigger(self,arg):
      global niLib
      global niInterfaceLib
      self.restoreInfo()

      try:
            status = niLib.xseries_start_ai(c_int(self.ai_fd))
            if(status != 0):
                errno = niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition : (%d) %s' % (errno , os.strerror( errno )) )
                return
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception Cannot Start Acquisition')
          return 0

      return 1

