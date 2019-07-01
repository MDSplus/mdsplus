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

from MDSplus import mdsExceptions, Device, Data, Window, Range, Dimension, TreePath, Signal
from MDSplus import mdsExceptions, Int32, Float32, Float64, Int16Array, Float64Array
from numpy import ndarray
from threading import Thread, Condition
from ctypes import CDLL, c_int, c_short, c_long, byref, Structure, c_void_p, c_char_p
from time import sleep

class CAENDT5724(Device):
    """CAEN DT5724 4 Channels 14 Bit 100MS/S Digitizer"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
      {'path':':COMMENT', 'type':'text'},
      {'path':':TRIG_MODE', 'type':'text', 'value':'OVER THRESHOLD'},
      {'path':':TRIG_SOFT', 'type':'text', 'value':'ENABLED'},
      {'path':':TRIG_EXT', 'type':'text', 'value':'ENABLED'},
      {'path':':TRIG_SOURCE', 'type':'numeric'},
      {'path':':CLOCK_MODE', 'type':'text', 'value':'250 MHz'},
      {'path':':CLOCK_SOURCE', 'type':'numeric'},
      {'path':':NUM_SEGMENTS', 'type':'numeric','value':1024},
      {'path':':USE_TIME', 'type':'text', 'value':'YES'},
      {'path':':PTS', 'type':'numeric','value':1024},
      {'path':':START_IDX', 'type':'numeric','value':0},
      {'path':':END_IDX', 'type':'numeric','value':1024},
      {'path':':START_TIME', 'type':'numeric','value':0},
      {'path':':END_TIME', 'type':'numeric','value':1E-6},
      {'path':':ACQ_MODE', 'type':'text','value':'TRANSIENT RECORDER'},
      {'path':':IRQ_EVENTS', 'type':'numeric','value':0}]
    for i in range(0,4):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:STATE'%(i+1), 'type':'text', 'value':'ENABLED'})
        parts.append({'path':'.CHANNEL_%d:TRIG_STATE'%(i+1), 'type':'text', 'value':'DISABLED'})
        parts.append({'path':'.CHANNEL_%d:OFFSET'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:DAC_OFFSET'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:THRESH_LEVEL'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:THRESH_SAMPL'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal'})
        parts.append({'path':'.CHANNEL_%d:SEG_RAW'%(i+1), 'type':'signal'})
    del i
    parts.append({'path':':INIT_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('PXI_SERVER','STORE',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('PXI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':NUM_CHANNELS', 'type':'numeric','value':0})

    cvV1718 = 0L                    # CAEN V1718 USB-VME bridge    
    cvV2718 = 1L                    # V2718 PCI-VME bridge with optical link       
    cvA2818 = 2L                    # PCI board with optical link                  
    cvA2719 = 3L                    # Optical link piggy-back                      
    cvA32_S_DATA = 0x0D             # A32 supervisory data access                  */
    cvD32 = 0x04		    # D32
    cvD64 = 0x08

    MEM_512kS = 524288
    MEM_4MS   = 4194304
    InternalFrequency = 100E6

    HANDLE_RESTORE = 1L
    HANDLE_OPEN  = 2L

    caenLib = None
    caenInterfaceLib = None

    caenHandles = {}
    caenCvs = {}
    caenReadCvs = {}
    caenWorkers = {}
    caenNids = {}

#Support Class for IRQ Wait
    class IRQWait(Thread):
      def configure(self, handle, cv, readCv):
        self.handle = handle
        self.cv = cv
        self.readCv = readCv
      def run(self):
        while 0 == 0:
          self.readCv.acquire()
          self.readCv.wait()
          self.readCv.release()
          #print 'waiting IRQ'
          status = CAENDT5724.caenLib.CAENVME_IRQWait(self.handle, c_long(0x01), c_long(1000000))
          #print 'IRQ Received'
          self.cv.acquire()
          self.cv.notify()
          self.cv.release()
    #end class IRQWait        
    
#Support class for continuous store
    class AsynchStore(Thread):

      cvV1718 = 0L                    # CAEN V1718 USB-VME bridge    
      cvV2718 = 1L                    # V2718 PCI-VME bridge with optical link       
      cvA2818 = 2L                    # PCI board with optical link                  
      cvA2719 = 3L                    # Optical link piggy-back                      
      cvA32_S_DATA = 0x0D             # A32 supervisory data access                 
      cvD32 = 0x04		      # D32
      cvD64 = 0x08


     
      #def configure(self, handle, acqMode, startIdx, endIdx, pts, actChans, nActChans, dt, trigTime, triggerSourceNid, segmentSamples, segmentSize, chanMask, nid, device, cv, readCv, useCounter, irqEvents):
      def configure(self, handle, acqMode, startIdx, endIdx, pts, actChans, nActChans, dt, triggerSourceNid, segmentSamples, segmentSize, chanMask, nid, device, cv, readCv, useCounter, irqEvents):


        self.handle   = handle
        self.startIdx = startIdx
        self.endIdx   = endIdx
        self.acqMode = acqMode
        self.pts      = pts
        self.actChans = actChans
        self.nActChans = nActChans
        self.dt = dt
        """
        self.trigTime = trigTime
        """
        self.segmentSamples = segmentSamples
        self.segmentSize = segmentSize
        self.chanMask = chanMask
        self.nid = nid
        self.device = device
        self.cv = cv
        self.readCv = readCv
        self.useCounter = useCounter
        self.irqEvents = irqEvents
        self.triggerSourceNid = triggerSourceNid
        self.saveList = c_void_p(0)


      def run(self):

        class DT5720Data(Structure):
          _fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_short * (self.segmentSamples * self.nActChans))]


        treePtr = c_void_p(0)
        status = CAENDT5724.caenInterfaceLib.openTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))

        CAENDT5724.caenInterfaceLib.startSave(byref(self.saveList))


        vmeAddress = 0
 
        currStartIdx    = self.segmentSamples - self.pts + self.startIdx
        currEndIdx      = self.segmentSamples - self.pts + self.endIdx
        currChanSamples = currEndIdx - currStartIdx
        numChannels     = self.device.num_channels.data()
        clockNid        = self.device.clock_source.getNid()
        triggNid        = self.device.trig_source.getNid()
        numTrigger      = 0


        channels = [] 
        chanNid  = []

        if self.acqMode == "TRANSIENT RECORDER":
           numTrigger = len(self.device.trig_source.getData())
	else:
           #continuous
           numTrigger = -1
        
        for chan in range(0,numChannels):
          channels.append([])
          chanNid.append( getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).getNid() )   

        chanNid_c = (c_int * len(chanNid) )(*chanNid)

        currSegmentIdx = 0
        segmentCounter = 0
        self.dtArray = []	

        while not self.stopReq:
          self.readCv.acquire()
          self.readCv.notify()
          self.readCv.release()
          self.cv.acquire()
          #print 'WAIT CONDITION'
          self.cv.wait()
          self.cv.release()
          #print 'CONDITION ISSUED'

	  # Read number of buffers 
	  actSegments = c_int(0)
	  status = CAENDT5724.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
	  if status != 0:
	     print 'Error reading number of acquired segments' 
             continue


          segmentCounter = CAENDT5724.caenInterfaceLib.readAndSaveSegments(self.handle, c_int(vmeAddress), c_int(numChannels), c_int(self.nActChans), c_int(self.segmentSamples), c_int(self.segmentSize), 
                                            c_int(self.startIdx), c_int(self.endIdx), c_int(self.pts),  c_int(self.useCounter),  c_int(self.chanMask), c_int(segmentCounter), 
                                            c_int(numTrigger), chanNid_c, clockNid, triggNid, treePtr, self.saveList)



          if self.acqMode == "TRANSIENT RECORDER" and segmentCounter == numTrigger :
            print 'Transient Recoder acquisition completed!!!!'
            return 0

          if self.stopReq :
            print 'ASYNCH STORE EXITED!!!!'
            return 0
          status = CAENDT5724.caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01));
        #endwhile self.stopReq == 0:

        return 0
        
      def stop(self):

        self.stopReq = True
        self.cv.acquire()
        self.cv.notify()
        self.cv.release()

        #need to wait a while
        sleep(0.5)

        CAENDT5724.caenInterfaceLib.stopSave(self.saveList)
        self.saveList = c_void_p(0)
        
    #end class AsynchStore

    def saveInfo(self):

      #CAENDT5724.caenNids
      CAENDT5724.caenHandles[self.getNid()] = self.handle
      CAENDT5724.caenCvs[self.getNid()]     = self.cv
      CAENDT5724.caenReadCvs[self.getNid()] = self.readCv
      # If worker is running stop it
      # Worker is saved by saveWorker
      try:
         CAENDT5724.caenWorkers[self.getNid()].stop()
         CAENDT5724.caenWorkers[self.getNid()].stopReq = True
      except:
         pass


    def restoreInfo(self):
      #global caenHandles
      #global caenCvs
      #global caenWorkers
      #global nids
 
        
      if CAENDT5724.caenLib is None:
        CAENDT5724.caenLib = CDLL("libCAENVME.so")

      try:
        CAENDT5724.caenLib
      except:
          print 'Error loading library libCAENVME.so' 
          return 0

      if CAENDT5724.caenInterfaceLib is None:
        CAENDT5724.caenInterfaceLib = CDLL("libCaenInterface.so")

      try:
        CAENDT5724.caenInterfaceLib
      except:
          print 'Error loading library libCaenInterface.so' 
          return 0
        
      try:
        #idx = caenNids.index(self.getNid())
        self.handle = CAENDT5724.caenHandles[self.getNid()]
        self.cv = CAENDT5724.caenCvs[self.getNid()]
        self.readCv = CAENDT5724.caenReadCvs[self.getNid()]
        #self.worker = CAENDT5724.caenWorkers[self.getNid()]
        return self.HANDLE_RESTORE
      except:
        try:
          boardId = self.board_id.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
          return 0
        self.handle = c_long(0)
        print 'HANDLE NOT FOUND INITIALIZE CAEN MODULE'
        #status = caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(boardId), byref(self.handle))
        #Device VMEDevice (V3718 card ) is 0, BOARID is istead  VMELink from 0 to 3 for the V3718 4 link card
        status = CAENDT5724.caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(boardId), c_int(0), byref(self.handle))
        if status != 0:
          print 'Error initializing CAENVME' 
          return 0
        
        self.cv     = Condition()
        self.readCv = Condition()
        IRQw = self.IRQWait()
        IRQw.daemon = True
        IRQw.configure(self.handle, self.cv, self.readCv)
        IRQw.start()
        
      return self.HANDLE_OPEN
 

################################### Worker Management
    def saveWorker(self):
      CAENDT5724.caenWorkers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        if self.getNid() in CAENDT5724.caenWorkers.keys():
           self.worker = CAENDT5724.caenWorkers[self.getNid()]
      except:
        print('Cannot restore worker!!')


################################# INIT ###############################
    def init(self):

      if self.restoreInfo() == 0 : 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open DT5724 Device' )
        return 0

      vmeAddress = 0
  
      #Module Reset
      data = c_int(0)
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF24), byref(data), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting DT5724 Device' )
        return 0
	
      #give some time
      sleep(0.1)

      #Module type
      devType = c_int(0)
      status = CAENDT5724.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8140), byref(devType), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading board info')
        return 0

      if (devType.value & 0x000000FF) != 0 :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid board type. Device must be DT5724 model')
        return 0

      if (devType.value & 0x0000FF00) >> 8 == 0x01 :
        self.chanMemory = self.MEM_512kS
      else:
        self.chanMemory = self.MEM_4MS
        
      print 'Channel Memory: ', self.chanMemory
 
      numChannels = devType.value >> 16
      print 'DevType code: ', devType.value
      print 'NUM CHANNELS: ', numChannels
      print 'Channel Memory: ', self.chanMemory
      self.num_channels.putData(numChannels)

      """      
      print "write decimation factor. Not Yet implemented"  	
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8044), byref(c_int(0x2)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing decimation' )
        return 0
      """

      #Number of segments
      segmentDict = {1:0, 2:1, 4:2, 8:3, 16:4, 32:5, 64:6, 128:7, 256:8, 512:9, 1024:10}
      try:
        nSegments=self.num_segments.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Number of Segments')
        return 0
      segmentCode = segmentDict[nSegments]
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x800c), byref(c_int(segmentCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      #print "Buffer Organization 0x800C ", segmentCode
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing number of segments' )
        return 0

      #Global Channel Configuration
      trigModeDict = {'OVER THRESHOLD':0, 'UNDER THRESHOLD':1}
      try:
        trigMode = self.trig_mode.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Trigger mode')
        return 0
      trigModeCode = trigModeDict[trigMode]
      conf = trigModeCode << 6
      conf = conf | 0x00000010
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8000), byref(c_int(conf)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing group configuration')
        return 0

      #Channel configurations
      trigEnableCode = 0L
      chanEnableCode = 0L
      enabledDict = {'ENABLED':1, 'DISABLED':0}
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):

        #Empy the node which will contain  the segmented data   
        getattr(self, 'channel_%d_seg_raw'%(chan+1)).deleteData()
	

        #Set threshold level
        threshold = getattr(self, 'channel_%d_thresh_level'%(chan+1)).data()
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1080 + chan * 0x100), byref(c_int(threshold)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold level')
          return 0

        #Set threshold samples
        threshSamples = getattr(self, 'channel_%d_thresh_sampl'%(chan+1)).data()
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1084 + chan * 0x100), byref(c_int(threshSamples)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold samples')
          return 0

        #Read FIRMWARE info
        """
        firmware = c_uint(0)
        status = CAENDT5724.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x108C + chan * 0x100), byref(firmware), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        print "firmware  AMC FPGA Addr ", hex(vmeAddress + 0x108C + chan * 0x100), hex((firmware.value >> 16) & 0x0000ffff), " Version ", hex((firmware.value >> 8) & 0x000000ff), ".", hex((firmware.value ) & 0x000000ff)
        """
        dac_offset = getattr(self, 'channel_%d_dac_offset'%(chan+1)).data()

	#Channel offset compensation
        try:        
          offset = getattr(self, 'channel_%d_offset'%(chan+1)).data()	  
        except:
          offset = 0;
	
        #Set offset
        offset = offset + dac_offset
        print 'Ch ', chan ,'Offset Volt = ',offset
        if(offset > 1.125):
          offset = 1.125
        if(offset < -1.125):
          offset = -1.125
        offset = (offset / 1.125) * 32767
        print 'Ch ', chan ,'Offset Val. =', int(offset)


        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1098 + chan * 0x100), byref(c_int(int(offset + 0x08000))), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing DAC offset')
          return 0

        #Enable channel
        state = getattr(self, 'channel_%d_state'%(chan+1)).data()
        chanEnableCode = chanEnableCode | (enabledDict[state] << chan)

        #Enable Trigger
        trigState = getattr(self, 'channel_%d_trig_state'%(chan+1)).data()
        trigEnableCode = trigEnableCode | (enabledDict[trigState] << chan)

        #END channel configuration loop
      
      #Set channel enabled mask
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8120), byref(c_int(chanEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel enable register')
        return 0


      #Set channel trigger mask
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel trigger enable register')
        return 0

      #Set trigger enabling
      trigExt = self.trig_ext.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigExt] << 30)
      trigSoft = self.trig_soft.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigSoft] << 31)
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing trigger configuration')
        return 0

      #Front Panel trigger out setting set TRIG/CLK to TTL
      data = 1
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x811C), byref(c_int(data)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

      #Configure trigger source
      """
      try:
        trigSource = self.trig_source.data()
        #Trigger source must be an array, consider only the first element as triggerSource time
        if len(self.trig_source.getShape()) > 0:
          trigSource = trigSource[0]
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')
        return 0
      """

      #Configure clock source
      # The clock source can be only INTERNAL
      clockMode = self.clock_mode.data()
      if clockMode == 'EXTERNAL':
        try:
          clockSource = self.clock_source()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Clock source')
          return 0
      else:
        clockSource = Range(None, None, Float64(1/self.InternalFrequency))
        self.clock_source.putData(clockSource)

      #Configure Post Trigger Samples
      try:
        pts = int( self.pts.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve PTS Samples')
        return 0
      segmentSize = self.chanMemory/nSegments 
      if pts > segmentSize:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'PTS Larger than segmentSize')
        return 0
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8114), byref(c_int(pts>>1)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

     
      #Time management compute endIdx and startIdx
      useTime=self.use_time.data()
      if useTime == 'YES':
        try:
	  #Start and End Index acquisition burst calculation is prfomend with trigger time set to 0
          trigSource = 0.
          startTime = self.start_time.data()
          endTime = self.end_time.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')
          return 0
        if endTime > 0:
          endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, segmentSize, trigSource), clockSource), Float64(endTime + trigSource))
        else:
          endIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, segmentSize, trigSource + endTime), clockSource), Float64(trigSource))
        self.end_idx.putData(Int32(int(endIdx + 0.5)))
        if startTime > 0:
          startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, segmentSize, trigSource), clockSource), startTime + trigSource)
        else:
          startIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, segmentSize, trigSource + startTime), clockSource), trigSource)
        self.start_idx.putData(Int32(int(startIdx + 0.5)))

      currStartIdx = int(segmentSize - pts + startIdx.data())
      if currStartIdx < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/pre-trigger samples')
          return 0

      currEndIdx = int(segmentSize - pts + endIdx.data())
      if currEndIdx >= segmentSize:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/post-trigger samples')
          return 0

      print 'startIdx      : ', int(startIdx)
      print 'endIdx        : ', int(endIdx)
      print 'SEGMENT SIZE  : ', int(segmentSize)
      print 'PTS           : ', pts
      print 'currStartIdx  : ', currStartIdx
      print 'currEndIdx    : ', currEndIdx

      acqMode = self.acq_mode.data()
      if acqMode == 'CONTINUOUS' or acqMode == 'CONTINUOUS WITH COUNTER' or acqMode == 'TRANSIENT RECORDER':
        irqEvents = self.irq_events.data()
        irqEvents = irqEvents - 1
        if irqEvents < 1:
          irqEvents = 1
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF18), byref(c_int(irqEvents)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ events')
          return 0
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF00), byref(c_int(0x09)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ line')
          return 0
        status = CAENDT5724.caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Enabling IRQ')
          return 0
        #Start asynchronous readout thread
        #self.start_store()
        #endif acqMode == 'CONTINUOUS SAMPLING'

      self.saveInfo()

      return 1


################################ TRIGGER ###################################

    def trigger(self):
 
      if ( self.restoreInfo() != self.HANDLE_RESTORE and self.worker.stopReq == True ) :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'DT5724 Device not initialized' )
        return 0

      try:
        vmeAddress = 0
        #Module SW trigger
        data = c_int(0)
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error in sofware trigger DT5724 Device'  )
          return 0
        return 1
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Generic SW trigger Error'  )
        return 0


################################# START STORE ###############################
    def start_store(self):

      if ( self.restoreInfo() != self.HANDLE_RESTORE ) :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'DT5724 Device not initialized' )
        return 0

      vmeAddress = 0

      #Module type
      devType = c_int(0)
      status = CAENDT5724.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8140), byref(devType), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading board info')
        return 0

      if (devType.value & 0x000000FF) != 0 :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid board type. Device must be DT5724 model')
        return 0

      if (devType.value & 0x0000FF00) >> 8 == 0x01 :
        self.chanMemory = self.MEM_512kS
      else:
        self.chanMemory = self.MEM_4MS

      try:
        boardId = self.board_id.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
        return 0
      try:
        clock = self.clock_source.evaluate()
        dt = clock.getDelta().data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating clock source' )
        return 0
      try:
        triggerSourceNid = TreePath(self.trig_source.getFullPath())
        #trigTime = self.trig_source.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating trigger source' )
        return 0
      try:
        startIdx = self.start_idx.data()
        endIdx = self.end_idx.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating start or end idx')
        return 0
      try:
        pts = self.pts.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating Post Trigger Samples' )
        return 0
    
      #Compute Segment Size
      try:
        nSegments = self.num_segments.data()
        segmentSamples = self.chanMemory/nSegments
      except: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading max number of segments' )
        return 0
      currStartIdx = segmentSamples - pts + startIdx
      currEndIdx = segmentSamples - pts + endIdx
    
      #Get Active channels
      chanMask = c_int(0)
      status = CAENDT5724.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8120), byref(chanMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      nActChans = 0
      chanMask = chanMask.value
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          nActChans = nActChans + 1
      if nActChans == 0:
        print 'No active groups' 
        return 1
      segmentSize = 16 + 2 * segmentSamples * nActChans
      acqMode = self.acq_mode.data()
      
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          try:        
            dac_offset = getattr(self, 'channel_%d_dac_offset'%(chan+1)).data()	  
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading channel DAC offset')		  
            return 0
          if acqMode == 'CONTINUOUS WITH COUNTER':
            useCounter = True
            data = Data.compile("2.25*($1 - 8192)/16384.+$2", TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath()), dac_offset);
          else:
            useCounter = False
            segRawPath = TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath())
            data = Data.compile("(2.25*( $ - 8192)/16384. + $ )", segRawPath, Float32(dac_offset) )
          try:
            getattr(self, 'channel_%d_data'%(chan+1)).putData(data) 
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Writing data' )
            return 0
      #endfor chan in range(0,numChannels):
    
      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False


      #self.worker.configure(self.handle, acqMode, startIdx, endIdx, pts, chanMask, nActChans, dt, trigTime, triggerSourceNid, segmentSamples, segmentSize, chanMask, self.getNid(), self, self.cv, self.readCv, useCounter, self.irq_events.data() + 1)

      self.worker.configure(self.handle, acqMode, startIdx, endIdx, pts, chanMask, nActChans, dt, triggerSourceNid, segmentSamples, segmentSize, chanMask, self.getNid(), self, self.cv, self.readCv, useCounter, self.irq_events.data() + 1)

      try:
        runCommand = 4
        """
          #External cllock not yet implemented 
          if clockMode == 'EXTERNAL':
          runCommand = runCommand | 0x00000040
        """
        #Module SW trigger
        data = c_int(0)
        status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(runCommand)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error starting acquisition on DT5724 Device'  )
          return 0
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot starting acquisition on DT5724 Device SW exception'  )
        return 0

      self.saveWorker()
      self.worker.start()
      """
      try:
        if  acqMode == 'TRANSIENT RECORDER':
          trigSoft = self.trig_soft.data()
          if trigSoft == 'ENABLED':
             trigSource = self.trig_source.data()
             t0 = trigSource[0]
             sleep(t0)
	     print "SW Trigger ", trigSource[0]
             status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
             if status != 0:
               Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error in sofware trigger DT5724 Device'  )
               return 0

             if len(trigSource) == 1 :
               sleep( 1 )
               status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
               if status != 0:
                 Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error in sofware trigger(1) DT5724 Device'  )
                 return 0

             for delay in trigSource[1 : ] :
               sleep( delay - t0 )
               t0 = delay
	       print "SW Trigger ", delay
               status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
               if status != 0:
                 Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error in sofware trigger DT5724 Device'  )
                 return 0
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot starting acquisition on DT5724 Device SW exception'  )
        return 0
      """


      return 1


#################################### STOP STORE ###################################
    def stop_store(self):


      if self.restoreInfo() != self.HANDLE_RESTORE :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'DT5724 Device not initialized' )
        return 0

      vmeAddress = 0
      #Stop device 
      status = CAENDT5724.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error stopping device')
        return 0
      #need to wait a while
      sleep(0.5)

      self.restoreWorker()
      if self.worker.isAlive():
          print "PXI CAENDT5724 stop_worker"
          self.worker.stop()
      del self.worker
 
      return 1


