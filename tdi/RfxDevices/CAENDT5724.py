from MDSplus import *
from numpy import *
from threading import *
import time
import sys
from ctypes import *

class CAENDT5724(Device):
    print 'CAENDT5724'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
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
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':NUM_CHANNELS', 'type':'numeric','value':0})

    cvV1718 = 0L                    # CAEN V1718 USB-VME bridge    
    cvV2718 = 1L                    # V2718 PCI-VME bridge with optical link       
    cvA2818 = 2L                    # PCI board with optical link                  
    cvA2719 = 3L                    # Optical link piggy-back                      
    cvA32_S_DATA = 0x0D             # A32 supervisory data access                  */
    cvD32 = 0x04		    # D32
    cvD64 = 0x08
    IRQw = 0
    cv = 0
    readCv = 0
    worker = 0    
    handle = 0

    MEM_512kS = 524288
    MEM_4MS   = 4194304
    InternalFrequency = 100E6
    #InternalFrequency = 250E6
    chanMemory = 0

    HANDLE_RESTORE = 1L
    HANDLE_OPEN  = 2L


#Support Class for IRQ Wait
    class IRQWait(Thread):
      def configure(self, handle, cv, readCv):
        self.handle = handle
        self.cv = cv
        self.readCv = readCv
      def run(self):
        import time
        from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
        global caenLib
        while 0 == 0:
          self.readCv.acquire()
          self.readCv.wait()
          self.readCv.release()
          #print 'waiting IRQ'
          status = caenLib.CAENVME_IRQWait(self.handle, c_long(0x01), c_long(1000000))
          #print 'IRQ Received'
          self.cv.acquire()
          self.cv.notify()
          self.cv.release()
    #end class IRQWait        
    
#Support class for continuous store
    class AsynchStore(Thread):
      import time;
      stopReq = False
      startIdx = 0
      endIdx = 0;
      pts = 0
      actChannels = 0
      trigTime = 0
      dt = 0
      boardId = 0
      cvV1718 = 0L                    # CAEN V1718 USB-VME bridge    
      cvV2718 = 1L                    # V2718 PCI-VME bridge with optical link       
      cvA2818 = 2L                    # PCI board with optical link                  
      cvA2719 = 3L                    # Optical link piggy-back                      
      cvA32_S_DATA = 0x0D             # A32 supervisory data access                 
      cvD32 = 0x04		              # D32
      cvD64 = 0x08


      #startTimeArray = []
      #endTimeArray = []
      dtArray = []
      segmentCounter = 0
      
      def configure(self, handle, startIdx, endIdx, pts, actChans, nActChans, dt, trigTime, triggerSourceNid, segmentSamples, segmentSize, chanMask, nid, device, cv, readCv, useCounter, irqEvents):

        self.handle = handle
        self.startIdx = startIdx
        self.endIdx = endIdx
        self.pts = pts
        self.actChans = actChans
        self.nActChans = nActChans
        self.dt = dt
        self.trigTime = trigTime
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
        global caenLib
        global caenInterfaceLib

        from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
        class DT5720Data(Structure):
          _fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_short * (self.segmentSamples * self.nActChans))]


        treePtr = c_void_p(0)
        status = caenInterfaceLib.openTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))

        caenInterfaceLib.startSave(byref(self.saveList))


        vmeAddress = 0
 
        currStartIdx    = self.segmentSamples - self.pts + self.startIdx
        currEndIdx      = self.segmentSamples - self.pts + self.endIdx
        currChanSamples = currEndIdx - currStartIdx
        numChannels     = self.device.num_channels.data()
        clockNid        = self.device.clock_source.getNid()
        triggNid        = self.device.trig_source.getNid()

        channels = [] 
        chanNid = []

        for chan in range(0,numChannels):
          channels.append([])
          chanNid.append( getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).getNid() )   

        chanNid_c = (c_int * len(chanNid) )(*chanNid)

        currSegmentIdx = 0
        segmentCounter = 0
        #self.startTimeArray = []
        #self.endTimeArray = []
        self.dtArray = []	

        while not self.stopReq:
          self.readCv.acquire()
          self.readCv.notify()
          self.readCv.release()
          self.cv.acquire()
          print 'WAIT CONDITION'
          self.cv.wait()
          self.cv.release()
          print 'CONDITION ISSUED'
# Read number of buffers 
          
          segmentCounter = caenInterfaceLib.readAndSaveSegments(self.handle, c_int(vmeAddress), c_int(numChannels), c_int(self.nActChans), c_int(self.segmentSamples), c_int(self.segmentSize), 
                                            c_int(self.startIdx), c_int(self.endIdx), c_int(self.pts),  c_int(self.useCounter),  c_int(self.chanMask), c_int(segmentCounter), 
                                            chanNid_c, clockNid, triggNid, treePtr, self.saveList)


          """
          actSegments = c_int(0)
          status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Error reading number of acquired segments' )
            return 0
          actSegments = actSegments.value
          for chan in range(0,numChannels):
            channels[chan] = ndarray(currChanSamples * actSegments)

          prevLastSegIdx = currSegmentIdx
          dimStart = ""
          dimEnd   = ""
          dimDt    = ""

          for segmentIdx in range(0,actSegments):
            segment= DT5720Data()
            retLen = c_int(0)
            status = caenLib.CAENVME_FIFOBLTReadCycle(self.handle, c_int(vmeAddress), byref(segment), c_int(self.segmentSize),  c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))

            if status != 0:
              Data.execute('DevLogErr($1,$2)', self.nid, 'ASYNCH: Error reading data segment')
              return 0

            actSize = 4 * (segment.eventSize & 0x0fffffff)
            counter = segment.time/2
            sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
            chanSizeInInts = sizeInInts/self.nActChans
            chanSizeInShorts = chanSizeInInts * 2

            #print 'self.startTime : ', self.startTimeArray
            #print 'self.endTime : ', self.endTimeArray

            if self.useCounter :
               #startTime = self.trigTime + (counter + self.startIdx) * self.dt
               startTime = (counter + self.startIdx) * self.dt
               endTime = startTime + currChanSamples * self.dt
            else:
               #startTime = self.trigTime + self.startIdx * self.dt
               #endTime   = self.trigTime + self.endIdx * self.dt
               #self.startTimeArray.append( self.trigTime[currSegmentIdx] + self.startIdx * self.dt)
               #self.endTimeArray.append( self.trigTime[currSegmentIdx] + self.endIdx * self.dt)
               self.dtArray.append( self.dt )
               dimStart = dimStart + str( self.startIdx * self.dt )
               dimEnd = dimEnd  + str( self.endIdx * self.dt )
               dimDt = dimDt + str( self.dt ) 
               if ( segmentIdx + 1 ) != actSegments :
                 dimStart = dimStart + ","
                 dimEnd = dimEnd  + ","
                 dimDt = dimDt + ","

            #print "trigger IDX ", currSegmentIdx       

            currSegmentIdx = currSegmentIdx + 1
		
            for chan in range(0,numChannels):
              if (self.chanMask & (1 << chan)) != 0:
                #print "channels["+str(chan)+"]["+str(segmentIdx*currChanSamples)+" : "+str(segmentIdx * currChanSamples + currEndIdx - currStartIdx)+"] = segment.data["+str(chan*chanSizeInShorts+currStartIdx)+" : "+str(chan*chanSizeInShorts+currEndIdx)+"]"
                
                channels[chan][segmentIdx*currChanSamples : segmentIdx * currChanSamples + currEndIdx - currStartIdx] = segment.data[chan*chanSizeInShorts+currStartIdx:chan*chanSizeInShorts+currEndIdx]
            #endfor  chan in range(0,numChannels)
          #endfor segmentIdx in range(0,actSegments):
##############################################
          if actSegments > 0:
           #dim = Range(Float64(segmentCounter * currChanSamples), Float64((segmentCounter + actSegments) * currChanSamples - 1), Float64(1.))
           #dim = Range(Float64(self.startTimeArray), Float64(self.endTimeArray), Float64(self.dtArray))

            if actSegments == 1 :
               dimExpr = "build_range($["+str(currSegmentIdx-1) +"] + [" + dimStart +"] , $["+ str(currSegmentIdx-1) +"] + [" + dimEnd +"] , $ )"

            else :
               dimExpr = "build_range($["+str(prevLastSegIdx)+".."+ str(currSegmentIdx-1) +"] + [" + dimStart +"] , $["+str(prevLastSegIdx)+".."+ str(currSegmentIdx-1) +"] + [" + dimEnd +"] , $)"
 
            #print 'DIM expr: ', dimStart
            #print 'DIM expr: ', dimEnd

            dimCmp = Data.compile(dimExpr, self.triggerSourceNid, self.triggerSourceNid, Float64(self.dtArray))

            #sTime = self.startTimeArray[0]
            #eTime = self.endTimeArray[len(self.endTimeArray)-1]

            timeStart = Data.compile("($[$])"  ,  self.triggerSourceNid, prevLastSegIdx);
            timeEnd =  Data.compile("($[$])"  ,  self.triggerSourceNid, currSegmentIdx);
			
            #print "time Start", timeStart.decompile()
            #print "DIM", dimCmp.decompile()

             
            #print 'DIM : ', dim, sTime, eTime
            for chan in range(0,numChannels):
              if (self.chanMask & (1 << chan)) != 0:
                data = Int16Array(channels[chan])
                try:
                 #getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).makeSegment(Float64(segmentCounter * currChanSamples), Float64((segmentCounter+actSegments) * currChanSamples), dim, data)
                 #getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).makeSegment( Float64(sTime), Float64(eTime) , dim, data)
                 getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).makeSegment( timeStart, timeEnd , dimCmp, data)
                except:
                  print "Unexpected error:", sys.exc_info()[1]
                  Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot write Segment in tree')		  
                  return 0
            #endif actSegments > 0
          #endfor chan in range(0,numChannels)
          #print "Reset start end"
			
          #self.startTimeArray = []
          #self.endTimeArray = []
          self.dtArray = []	
          segmentCounter = segmentCounter + actSegments
          """
###################################################
          if(self.stopReq):
            print 'ASYNCH STORE EXITED!!!!'
            return 0
          status = caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01));

        #endwhile self.stopReq == 0:
        return 0
        
      def stop(self):
        global caenInterfaceLib
        self.stopReq = True
        self.cv.acquire()
        self.cv.notify()
        self.cv.release()

    #need to wait a while
        time.sleep(0.5)

        caenInterfaceLib.stopSave(self.saveList)
        self.saveList = c_void_p(0)

        
    #end class AsynchStore

    def saveInfo(self):
      global caenHandles
      global caenCvs
      global caenReadCvs
      global caenWorkers
      global caenNids
      try:
        caenHandles
      except:
	caenHandles = []
        caenCvs = []
        caenReadCvs = []
        caenWorkers = []
        caenNids = []
      try:
        idx = caenNids.index(self.getNid())
      except:
        caenHandles.append(self.handle)
        caenCvs.append(self.cv)
        caenReadCvs.append(self.readCv)
        caenWorkers.append(self.worker)
        caenNids.append(self.nid)
        return
      caenHandles[idx] = self.handle
      caenCvs[idx] = self.cv
      caenReadCvs[idx] = self.readCv
      if caenWorkers[idx] != 0:
        caenWorkers[idx].stop()
        caenWorkers[idx].stopReq = True
      caenWorkers[idx] = self.worker
      return

    def restoreInfo(self):
      global caenHandles
      global caenCvs
      global caenWorkers
      global nids
      global caenLib
      global caenInterfaceLib
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      try:
        caenLib
      except:
        caenLib = CDLL("libCAENVME.so")

      try:
        caenLib
      except:
          print 'Error loading library libCAENVME.so' 
          return 0
        
      try:
        caenInterfaceLib
      except:
        caenInterfaceLib = CDLL("libCaenInterface.so")

      try:
        caenInterfaceLib
      except:
          print 'Error loading library libCaenInterface.so' 
          return 0
        
      try:
        idx = caenNids.index(self.getNid())
        self.handle = caenHandles[idx]
        self.cv = caenCvs[idx]
        self.readCv = caenReadCvs[idx]
        self.worker = caenWorkers[idx]
        return self.HANDLE_RESTORE
      except:
        try:
          boardId = self.board_id.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
          return 0
        self.handle = c_long(0)
        print 'HANDLE NON TROVATO ADESSO INIZIA:IZZO DRIVER'
        status = caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(boardId), byref(self.handle))
        if status != 0:
          print 'Error initializing CAENVME' 
          return 0
        self.cv = Condition()
        self.readCv = Condition()
        IRQw = self.IRQWait()
        IRQw.daemon = True
        IRQw.configure(self.handle, self.cv, self.readCv)
        IRQw.start()
      return self.HANDLE_OPEN
 
    def start_store(self):
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time
      try:
        boardId = self.board_id.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
        return 0
      vmeAddress = 0
      try:
        clock = self.clock_source.evaluate()
        dt = clock.getDelta().data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating clock source' )
        return 0
      try:
        triggerSourceNid = TreePath(self.trig_source.getFullPath())
        trigTime = self.trig_source.data()
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
    
    # Get Active channels
      chanMask = c_int(0)
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8120), byref(chanMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      nActChans = 0
      chanMask = chanMask.value
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          nActChans = nActChans + 1
      if nActChans == 0:
        print 'No active groups' 
        return 1
      segmentSize = 16 + 2*segmentSamples * nActChans
      acqMode = self.acq_mode.data()
      
      
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          try:        
            offset = getattr(self, 'channel_%d_offset'%(chan+1)).data()	  
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading channel offset')		  
            return 0
          if acqMode == 'CONTINUOUS WITH COUNTER':
            useCounter = True
            data = Data.compile("2.25*($1 - 8192)/16384.+$2", TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath()), offset);
          else:
            useCounter = False
            startTime = startIdx * dt
            endTime = (endIdx-1) * dt
            segRawPath = TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath())
            trigPath = TreePath(self.trig_source.getFullPath())
            #data = Data.compile("BUILD_SIGNAL(2.25*($VALUE - 8192)/16384.+$1, DATA($2), MAKE_RANGE($3+$4, $3+$5, REPLICATE($6,0,SIZE($3))))", Float32(offset), segRawPath, trigPath, Float64(startTime), Float64(endTime), Float64(dt))
            data = Data.compile("(2.25*( $ - 8192)/16384. + $ )", segRawPath, Float32(offset) )
          try:
            getattr(self, 'channel_%d_data'%(chan+1)).putData(data) 
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Writing data' )
            return 0
      #endfor chan in range(0,numChannels):
      

      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
      self.worker.configure(self.handle, startIdx, endIdx, pts, chanMask, nActChans, dt, trigTime, triggerSourceNid, segmentSamples, segmentSize, chanMask, self.getNid(), self, self.cv, self.readCv, useCounter, self.irq_events.data() + 1)
      self.worker.start()
      return 1

      
    def stop_store(self,arg):
      self.worker.stop()
      return 1

################################# INIT ###############################
    def init(self,arg):
      global caenLib
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time


      if self.restoreInfo() == 0 : 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open DT5724 Device' )
        return 0

      vmeAddress = 0  
#Module Reset
      data = c_int(0)
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF24), byref(data), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting DT5724 Device' )
        return 0
	
    #give some time
      time.sleep(0.1)

#Module type
      devType = c_int(0)
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8140), byref(devType), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading board info')
        return 0
      """
      if (devType.value & 0x000000FF) != 0 :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid board type. Device must be DT5724 model')
        return 0
      """
      if (devType.value & 0x0000FF00) >> 8 == 0x01 :
        self.chanMemory = self.MEM_512kS
      else:
        self.chanMemory = self.MEM_4MS
 
      #self.chanMemory = int( 1024*1024 )

      numChannels = devType.value >> 16
      print 'NUM CHANNELS: ', numChannels
      print 'Channel Memory: ', self.chanMemory
      self.num_channels.putData(numChannels)

      """      
      print "write decimation factor"  	
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8044), byref(c_int(0x2)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing decimation' )
        return 0
      """

#number of segments
      segmentDict = {1:0, 2:1, 4:2, 8:3, 16:4, 32:5, 64:6, 128:7, 256:8, 512:9, 1024:10}
      try:
        nSegments=self.num_segments.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Number of Segments')
        return 0
      segmentCode = segmentDict[nSegments]
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x800c), byref(c_int(segmentCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      print "Buffer Organization 0x800C ", segmentCode
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
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8000), byref(c_int(conf)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing group configuration')
        return 0

#Channel configurations
      trigEnableCode = 0L
      chanEnableCode = 0L
      enabledDict = {'ENABLED':1, 'DISABLED':0}
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
#threshold level
        threshold = getattr(self, 'channel_%d_thresh_level'%(chan+1)).data()
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1080 + chan * 0x100), byref(c_int(threshold)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold level')
          return 0
#threshold samples
        threshSamples = getattr(self, 'channel_%d_thresh_sampl'%(chan+1)).data()
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1084 + chan * 0x100), byref(c_int(threshSamples)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold samples')
          return 0

#FIRMWARE
        firmware = c_uint(0)
        status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x108C + chan * 0x100), byref(firmware), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        print "firmware  AMC FPGA Addr ", hex(vmeAddress + 0x108C + chan * 0x100), hex((firmware.value >> 16) & 0x0000ffff), " Version ", hex((firmware.value >> 8) & 0x000000ff), ".", hex((firmware.value ) & 0x000000ff)

#offset
        offset = getattr(self, 'channel_%d_offset'%(chan+1)).data()
        if(offset > 1.125):
          offset = 1.125
        if(offset < -1.125):
          offset = -1.125
        offset = (offset / 1.) * 32767
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1098 + chan * 0x100), byref(c_int(int(offset + 0x08000))), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing DAC offset')
          return 0

#states
        state = getattr(self, 'channel_%d_state'%(chan+1)).data()
        chanEnableCode = chanEnableCode | (enabledDict[state] << chan)
        trigState = getattr(self, 'channel_%d_trig_state'%(chan+1)).data()
        trigEnableCode = trigEnableCode | (enabledDict[trigState] << chan)

#endfor chan in range(0,numChannels)
      
#Set channel enabled mask
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8120), byref(c_int(chanEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel enable register')
        return 0
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel trigger enable register')
        return 0

#Set trigger enabling
      trigExt = self.trig_ext.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigExt] << 30)
      trigSoft = self.trig_soft.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigSoft] << 31)
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing trigger configuration')
        return 0

#Front Panel trigger out setting set TRIG/CLK to TTL
      data = 1
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x811C), byref(c_int(data)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

#trigger source
      try:
        trigSource = self.trig_source.data()
#if trigger is expressed as an array, consider only the first element
        if len(self.trig_source.getShape()) > 0:
          trigSource = trigSource[0]
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')
        return 0

#Clock source	
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

#Post Trigger Samples
      try:
        pts = int( self.pts.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve PTS Samples')
        return 0
      segmentSize = self.chanMemory/nSegments 
      #segmentSize = self.MEM_512kS/nSegments
      if pts > segmentSize:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'PTS Larger than segmentSize')
        return 0
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8114), byref(c_int(pts>>1)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      print "Post Trigger Settings 0x8114 ", int(pts>>1)

#################
    # Get custom size
      customSize = 0
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8020), byref(c_int(customSize)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      print "Custom Size 0x8020 ", customSize

#FIRMWARE
      firmware = c_uint(0)
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8124), byref(firmware), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      print "firmware ROC FPGA Addr ", hex(vmeAddress + 0x108C + chan * 0x100), hex((firmware.value))
      print "firmware ROC FPGA Addr ", hex(vmeAddress + 0x108C + chan * 0x100), hex((firmware.value >> 16) & 0x0000ffff), " Version ", hex((firmware.value >> 8) & 0x000000ff), ".", hex((firmware.value ) & 0x000000ff)
      
#################

#Time management
      useTime=self.use_time.data()
      if useTime == 'YES':
        try:
          print "trigsource [0]", trigSource
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
#Internal/External clock
      print 'startIdx: ', int(startIdx)
      print 'endIdx: ', int(endIdx)
      print 'SEGMENT SIZE: ', int(segmentSize)
      print 'PTS: ', pts

      currStartIdx = int(segmentSize - pts + startIdx.data())
      print 'currStartIdx: ', currStartIdx
      if currStartIdx < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/pre-trigger samples')
          return 0

      currEndIdx = int(segmentSize - pts + endIdx.data())
      print 'segmentSize: ', segmentSize
      print 'PTS: ', pts
      print 'endIdx: ', endIdx
      print 'currEndIdx: ', currEndIdx
      if currEndIdx >= segmentSize:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/post-trigger samples')
          return 0

      acqMode = self.acq_mode.data()
      if acqMode == 'CONTINUOUS' or acqMode == 'CONTINUOUS WITH COUNTER':
        irqEvents = self.irq_events.data()
        irqEvents = irqEvents - 1
        if irqEvents < 1:
          irqEvents = 1
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF18), byref(c_int(irqEvents)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ events')
          return 0
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF00), byref(c_int(0x09)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ line')
          return 0
        status = caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Enabling IRQ')
          return 0
#Start asynchronous readout
        self.start_store()
      #endif acqMode == 'CONTINUOUS SAMPLING'
      print "Run device"
      
      runCommand = 4 
      if clockMode == 'EXTERNAL':
        runCommand = runCommand | 0x00000040
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(4)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      print "Fine Init 0"     
      self.saveInfo()
      print "Fine Init"
      return 1


################################TRIGGER###################################

    def trigger(self,arg):
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time

      if ( self.restoreInfo() != self.HANDLE_RESTORE and self.worker.stopReq == True ) :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'DT5724 Device not initialized' )
        return 0

      try:
        vmeAddress = 0
  #Module SW trigger
        data = c_int(0)
        status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting DT5724 Device'  )
          return 0
        return 1
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Generic SW trigger Error'  )
        return 0

####################################STORE###################################

    def store(self,arg):
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time


      if self.restoreInfo() != self.HANDLE_RESTORE and self.worker.stopReq == True :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'DT5724 Device not initialized' )
        return 0


      vmeAddress = 0
# Stop device 
      status = caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error stopping device')
        return 0
    #need to wait a while
      time.sleep(0.5)
      acqMode = self.acq_mode.data()
      if acqMode == 'CONTINUOUS' or acqMode == 'CONTINUOUS WITH COUNTER':
        self.stop_store(0)
        return 1
	
  
      try:
        clock = self.clock_source.evaluate()
        dt = clock.getDelta().data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating clock source' )
        return 0
      try:
        trig = self.trig_source.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating trigger source' )
        return 0
      try:
        currStartIdx = startIdx = self.start_idx.data()
        currEndIdx = endIdx = self.end_idx.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating start or end idx')
        return 0
      try:
        pts = self.pts.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating Post Trigger Samples' )
        return 0
    
    
    
    # Read number of buffers 
      actSegments = c_int(0)
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading number of acquired segments' )
        return 0
    
      if actSegments.value == 0:
        return 1

    
    #Compute Segment Size
      try:
        nSegments = self.num_segments.data()
        segmentSamples = self.chanMemory/nSegments
      except: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading max number of segments' )
        return 0
    	
    
    # Get Active channels
      chanMask = c_int(0)
      status = caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8120), byref(chanMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      nActChans = 0
      chanMask = chanMask.value
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          nActChans = nActChans + 1
      if nActChans == 0:
        print 'No active groups' 
        return 1
 
      segmentSize = 16 + 2*segmentSamples * nActChans

      class DT5720Data(Structure):
        _fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_short * (segmentSize / 2))]
    
      actSegments = actSegments.value
      
      currChanSamples = Int32(currEndIdx - currStartIdx + 0.5).data()
      triggers = []
      deltas = []
      channels = [] 
      for chan in range(0,numChannels):
        channels.append([])
      for chan in range(0,numChannels):
        channels[chan] = ndarray(currChanSamples * actSegments)
      for segmentIdx in range(0,actSegments):
        segment= DT5720Data()
        retLen = c_int(0)
        status = caenLib.CAENVME_FIFOBLTReadCycle(self.handle, c_int(vmeAddress), byref(segment), c_int(segmentSize),  c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading data segment')
          return 0

        actSize = 4 * (segment.eventSize & 0x0fffffff)
        if actSize != segmentSize: 
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Expected event size different from expected size' )		  
          return 0
        counter = segment.time/2
        triggers.append(counter*dt)
        deltas.append(dt)
        sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
        chanSizeInInts = sizeInInts/nActChans
        chanSizeInShorts = chanSizeInInts * 2
        chanOffset = 0
        for chan in range(0,numChannels):
          if (chanMask & (1 << chan)) != 0:
            channels[chan][segmentIdx * currChanSamples : segmentIdx * currChanSamples + currEndIdx - currStartIdx] = segment.data[chan*chanSizeInShorts+currStartIdx:chan*chanSizeInShorts+currEndIdx]
        #endfor  chan in range(0,numChannels)
      #endfor segmentIdx in range(0,actSegments):
      if len(self.trig_source.getShape()) > 0:
        dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig[0]),Range(Float64Array(trig) + Float64(startIdx * dt),  Float64Array(trig) + Float64(endIdx * dt), Float64Array(deltas)))
      else:
        dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig),Range(Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(startIdx * dt),  Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(endIdx * dt), Float64Array(deltas)))
      dim.setUnits("s");
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          try:        
            offset = getattr(self, 'channel_%d_offset'%(chan+1))	  
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading channel offset')		  
            return 0
          raw = Int16Array(channels[chan])
          raw.setUnits("counts")
          data = Data.compile("2*($VALUE - 8192)/16384.+$1", offset);
          data.setUnits("Volts")
          signal = Signal(data, raw, dim)
          try:
            getattr(self, 'channel_%d_data'%(chan+1)).putData(signal)
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot write Signal in tree')		  
            return 0
      #endfor chan in range(0,numChannels)
      return 1
    

      
