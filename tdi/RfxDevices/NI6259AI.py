from MDSplus import *
from numpy import *
from threading import *
import time
import os
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
        parts.append({'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal'})
    del i
    parts.append({'path':':NUM_SAMPLES', 'type':'numeric'})
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','READY',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})
    
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
        print 'SAVE INFO: SAVING HANDLE'
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
            print 'RESTORE INFO HANDLE TROVATO'
        except:
            print 'RESTORE INFO HANDLE NON TROVATO'
            try: 
                boardId = self.board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                return 0
            try:
                fileName = '/dev/pxi6259.'+str(boardId)+'.ai';
                self.fd = os.open(fileName, os.O_RDWR);
                print 'Open fd: ', self.fd
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ fileName)
                return 0
        return 1
###################################Worker Management
    def saveWorker(self):
      global ni6259Workers
      global ni6259WorkerNids
      try:
        ni6259Workers
      except:
        ni6259WorkerNids = []
        ni6259Workers = []
      try:
        idx = ni6259WorkerNids.index(self.getNid())
        ni6259Workers[idx] = self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        ni6259WorkerNids.append(self.getNid())
        ni6259Workers.append(self.worker)
        return
      return

    def restoreWorker(self):
      global ni6259WorkerNids
      global ni6259Workers
      
      try:
        idx = ni6259WorkerNids.index(self.getNid())
        self.worker = ni6259Workers[idx]
      except:
        print 'Cannot restore worker!!'

########################AsynchStore class
    class AsynchStore(Thread):
        stopReq = False
       
        def configure(self, device, niLib, niInterfaceLib, fd, chanMap, hwChanMap, treePtr):
            self.device = device
            self.niLib = niLib
            self.niInterfaceLib = niInterfaceLib
            self.fd = fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr


        def run(self):
            import os
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data();
            chanFd = []
            for chan in range(len(self.chanMap)):
                try:
                    print '/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]])
                    currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    print 'APERTO', currFd
                    chanFd.append(currFd)
                except:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open Channel '+ str(self.chanMap[chan]))
                    return
            status = self.niLib.pxi6259_start_ai(c_int(self.fd))
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
            try:
                numSamples = self.device.num_samples.data()
            except:
                numSamples = -1
            print 'BUF SIZE ', bufSize
            print 'SEGMENT SIZE ', segmentSize
            if(bufSize > segmentSize):
                segmentSize = bufSize
            else:
                c = segmentSize/bufSize
                if (segmentSize % bufSize > 0):
                    c = c+1
                segmentSize = c*bufSize
            print 'BUF SIZE ', bufSize
            print 'SEGMENT SIZE ', segmentSize
            while not self.stopReq:
                allFinished = True
                for chan in range(len(self.chanMap)):
                    if(numSamples < 0 or counters[chan] < numSamples):
                        print 'START READ'
                        readSamples = self.niInterfaceLib.readAndSave(c_int(chanFd[chan]), c_int(bufSize), c_int(segmentSize), c_int(counters[chan]), getattr(self.device, 'channel_%d_data'%(chan+1)).getNid(), self.device.clock_source.getNid(), self.treePtr)
                        print 'Channel ', chan, 'READ SAMPLES: ', readSamples
                        counters[chan] = counters[chan] + readSamples
                        allFinished = False
    ##Check termination
                if allFinished:
                    self.stopReq = True
    ##endwhile                
            status = self.niLib.pxi6259_stop_ai(c_int(self.fd))
            for chan in range(len(self.chanMap)):
                os.close(chanFd[chan])
            print 'ASYNCH WORKER TERMINATED'
            return
        def stop(self):
            self.stopReq = True
 #############End Inner class AsynchStore      
    
##########init############################################################################    
    def init(self,arg):
        global niLib
        global niInterfaceLib
        self.restoreInfo()
        niInterfaceLib.pxi6259_create_ai_conf_ptr.restype = c_void_p
        aiConf = niInterfaceLib.pxi6259_create_ai_conf_ptr()
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
                polarity = self.polarityDict[getattr(self, 'channel_%d_polarity'%(chan+1)).data()]
                print 'POLARITY: ', polarity
                gain = self.gainDict[getattr(self, 'channel_%d_range'%(chan+1)).data()]
                print 'GAIN: ', gain
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan + 1))
                return 0
            if(enabled):
                if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
                    print 'INGRESSO DIFFERENZIALE'
                    currChan = self.diffChanMap[chan]
                else:
                    print 'INGRESSO NON DIFFERNZIALE'
                    currChan = chan
                #print 'POLARITY: ' + str(polarity) + ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = niLib.pxi6259_add_ai_channel(aiConf, c_byte(currChan), polarity, gain, inputMode, c_byte(0))
                print 'STATUS: ', status
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot add channel '+str(currChan + 1))
                    return 0
                print 'CANALE '+ str(currChan+1) + ' CONFIGURATO'
        #endfor
        try:
            nSamples = self.num_samples.data()
        except:
            nSamples = -1
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                print 'FREQUENZA: ', frequency
                divisions = int(20000000/frequency)
                status = niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(divisions), c_int(3), self.AI_SAMPLE_SELECT_SI_TC, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Sample Clock')
                    return 0
                if nSamples > 0:
                    clockSource = Range(Float64(0), Float64(nSamples * divisions/20000000.) , Float64(divisions/20000000.))
                else:
                    clockSource = Range(Float64(0), Float64(3600), Float64(divisions/20000000.))
                print 'CLOCK: ', clockSource
                self.clock_source.putData(clockSource)
                print 'SCRITTO'
            else:
                print 'CLOCK ESTERNO'
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
        status = niLib.pxi6259_load_ai_conf(c_int(self.fd), aiConf)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load configuration')
            return 0
        if nSamples > 0:
            status = niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Number of Samples')
                return 0
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
                chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan + 1))
                return 0
        treePtr = c_void_p(0);
        status = niInterfaceLib.openTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            self.worker.configure(self, niLib, niInterfaceLib, self.fd, chanMap, self.diffChanMap, treePtr)
        else:
            self.worker.configure(self, niLib, niInterfaceLib, self.fd, chanMap, self.nonDiffChanMap, treePtr)
        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,arg):
      self.restoreWorker()
      self.worker.stop()
      return 1
               


            
        


