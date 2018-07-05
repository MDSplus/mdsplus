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
from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
from time import sleep

class CAENDT5720(Device):
    """CAEN DT5720 4 Channels 12 Bit 250MS/S Digitizer"""
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
    del(i)
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':NUM_CHANNELS', 'type':'numeric','value':0})
    cvV1718 = 0          # CAEN V1718 USB-VME bridge
    cvV2718 = 1          # V2718 PCI-VME bridge with optical link
    cvA2818 = 2          # PCI board with optical link
    cvA2719 = 3          # Optical link piggy-back
    cvA32_S_DATA = 0x0D  # A32 supervisory data access
    cvD32 = 0x04		  # D32
    cvD64 = 0x08
    IRQw = 0
    cv = 0
    readCv = 0
    worker = 0
    handle = 0
    caenLib = None
    workers = {}

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
                CAENDT5720.caenLib.CAENVME_IRQWait(self.handle, c_long(0x01), c_long(1000000))
                self.cv.acquire()
                self.cv.notify()
                self.cv.release()


#Support class for continuous store
    class AsynchStore(Thread):
      stopReq = False
      startIdx = 0
      endIdx = 0;
      pts = 0
      actChannels = 0
      trigTime = 0
      dt = 0
      boardId = 0
      cvV1718 = 0          # CAEN V1718 USB-VME bridge
      cvV2718 = 1          # V2718 PCI-VME bridge with optical link
      cvA2818 = 2          # PCI board with optical link
      cvA2719 = 3          # Optical link piggy-back
      cvA32_S_DATA = 0x0D  # A32 supervisory data access
      cvD32 = 0x04	    # D32
      cvD64 = 0x08

      def configure(self, handle, startIdx, endIdx, pts, actChans, nActChans, dt, trigTime, segmentSamples, segmentSize, chanMask, nid, device, cv, readCv, useCounter, irqEvents):
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

      def run(self):
        class DT5720Data(Structure):
          _fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_short * (self.segmentSamples * self.nActChans))]

        vmeAddress = 0

        currStartIdx = self.segmentSamples - self.pts + self.startIdx
        currEndIdx = self.segmentSamples - self.pts + self.endIdx
        currChanSamples = currEndIdx - currStartIdx
        numChannels = self.device.num_channels.data()
        channels = [None]*numChannels
        segmentCounter = 0
        while not self.stopReq:
          self.readCv.acquire()
          self.readCv.notify()
          self.readCv.release()
          self.cv.acquire()
          self.cv.wait()
          self.cv.release()
# Read number of buffers
          actSegments = c_int(0)
          status = CAENDT5720.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Error reading number of acquired segments' )
            raise mdsExceptions.TclFAILED_ESSENTIAL
          actSegments = actSegments.value
          for chan in range(numChannels):
            channels[chan] = ndarray(currChanSamples * actSegments)
          for segmentIdx in range(0,actSegments):
            segment= DT5720Data()
            retLen = c_int(0)
            status = CAENDT5720.caenLib.CAENVME_FIFOBLTReadCycle(self.handle, c_int(vmeAddress), byref(segment), c_int(self.segmentSize),  c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))

            if status != 0:
              Data.execute('DevLogErr($1,$2)', self.nid, 'ASYNCH: Error reading data segment')
              raise mdsExceptions.TclFAILED_ESSENTIAL

            #actSize = 4 * (segment.eventSize & 0x0fffffff)
            #counter = segment.time/2
            sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
            chanSizeInInts = sizeInInts/self.nActChans
            chanSizeInShorts = chanSizeInInts * 2
            #startTime = self.trigTime + (counter + self.startIdx) * self.dt
            #endTime = startTime + currChanSamples * self.dt
            for chan in range(0,numChannels):
              if (self.chanMask & (1 << chan)) != 0:
                channels[chan][segmentIdx*currChanSamples : segmentIdx*currChanSamples + currEndIdx - currStartIdx] = segment.data[chan*chanSizeInShorts+currStartIdx:chan*chanSizeInShorts+currEndIdx]
            #endfor  chan in range(0,numChannels)
          #endfor segmentIdx in range(0,actSegments):
##############################################
          if actSegments > 0:
            dim = Range(Float64(segmentCounter * currChanSamples), Float64((segmentCounter + actSegments) * currChanSamples - 1), Float64(1.))
#            print 'DIM: ', dim
            for chan in range(0,numChannels):
              if (self.chanMask & (1 << chan)) != 0:
                data = Int16Array(channels[chan])
                try:
                  getattr(self.device, 'channel_%d_seg_raw'%(chan+1)).makeSegment(Float64(segmentCounter * currChanSamples), Float64((segmentCounter+actSegments) * currChanSamples), dim, data)
                except:
                  Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot write Segment in tree')
                  raise mdsExceptions.TclFAILED_ESSENTIAL
            #endif actSegments > 0
          #endfor chan in range(0,numChannels)
          segmentCounter = segmentCounter + actSegments
###################################################
          if(self.stopReq):
            print('ASYNCH STORE EXITED!!!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL
          status = CAENDT5720.caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01));

        #endwhile self.stopReq == 0:
        raise mdsExceptions.TclFAILED_ESSENTIAL

      def stop(self):
        self.stopReq = True
        self.cv.acquire()
        self.cv.notify()
        self.cv.release()

    #end class AsynchStore

    def saveInfo(self):
      if self.getNid() in CAENDT5720.workers.keys():
        oldworker = CAENDT5720.workers[self.nid][0]
        if oldworker != 0:
          oldworker.stop()
          oldworker.stopReq = True
        CAENDT5720.workers[self.nid] = (self.worker,self.handle,self.cv,self.readCv)

    def restoreInfo(self):
      if CAENDT5720.caenLib is None:
        CAENDT5720.caenLib = CDLL("libCAENVME.so")
      if self.getNid() in CAENDT5720.workers.keys():
        self.worker,self.handle,self.cvself.readCv = CAENDT5720.workers[self.nid]
      else:
        try:
          boardId = self.board_id.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        self.handle = c_long(0)
        print('HANDLE NON TROVATO ADESSO INIZIA:IZZO DRIVER')
        status = CAENDT5720.caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(boardId), byref(self.handle))
        if status != 0:
          print('Error initializing CAENVME')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        self.cv = Condition()
        self.readCv = Condition()
        IRQw = self.IRQWait()
        IRQw.daemon = True
        IRQw.configure(self.handle, self.cv, self.readCv)
        IRQw.start()
      return

    def start_store(self):
      try:
        self.board_id.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Board ID specification')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      vmeAddress = 0
      try:
        clock = self.clock_source.evaluate()
        dt = clock.getDelta().data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating clock source' )
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        trigTime = self.trig_source.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating trigger source' )
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        startIdx = self.start_idx.data()
        endIdx = self.end_idx.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating start or end idx')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        pts = self.pts.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating Post Trigger Samples' )
        raise mdsExceptions.TclFAILED_ESSENTIAL

    #Compute Segment Size
      try:
        nSegments = self.num_segments.data()
        segmentSamples = 1048576/nSegments
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading max number of segments' )
        raise mdsExceptions.TclFAILED_ESSENTIAL

    # Get Active channels
      chanMask = c_int(0)
      CAENDT5720.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8120), byref(chanMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      nActChans = 0
      chanMask = chanMask.value
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          nActChans = nActChans + 1
      if nActChans == 0:
        print('No active groups')
        return
      segmentSize = 16 + 2*segmentSamples * nActChans
      acqMode = self.acq_mode.data()


      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          try:
            offset = getattr(self, 'channel_%d_offset'%(chan+1)).data()
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading channel offset')
            raise mdsExceptions.TclFAILED_ESSENTIAL
          if acqMode == 'CONTINUOUS WITH COUNTER':
            useCounter = True
            data = Data.compile("2*($1 - 2048)/4096.+$2", TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath()), offset);
          else:
            useCounter = False
            startTime = startIdx * dt
            endTime = (endIdx-1) * dt
            segRawPath = TreePath(getattr(self, 'channel_%d_seg_raw'%(chan+1)).getFullPath())
            trigPath = TreePath(self.trig_source.getFullPath())
            data = Data.compile("BUILD_SIGNAL(2*($VALUE - 2048)/4096.+$1, DATA($2), MAKE_RANGE($3+$4, $3+$5, REPLICATE($6,0,SIZE($3))))", Float32(offset), segRawPath, trigPath, Float64(startTime), Float64(endTime), Float64(dt))
          try:
            getattr(self, 'channel_%d_data'%(chan+1)).putData(data)
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Writing data' )
            raise mdsExceptions.TclFAILED_ESSENTIAL
      #endfor chan in range(0,numChannels):


      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      self.worker.configure(self.handle, startIdx, endIdx, pts, chanMask, nActChans, dt, trigTime, segmentSamples, segmentSize, chanMask, self.getNid(), self, self.cv, self.readCv, useCounter, self.irq_events.data() + 1)
      self.worker.start()
      return


    def stop_store(self):
      self.worker.stop()
      return

################################# INIT ###############################
    def init(self):
      self.restoreInfo()
      vmeAddress = 0
#Module Reset
      data = c_int(0)
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF24), byref(data), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting V1740 Device' )
        raise mdsExceptions.TclFAILED_ESSENTIAL

    #give some time
      sleep(0.1)

#Module type
      devType = c_int(0)
      status = CAENDT5720.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8140), byref(devType), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing group configuration')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      numChannels = devType.value >> 16
      print('NUM CHANNELS: ', numChannels)
      self.num_channels.putData(numChannels)

#number of segments
      segmentDict = {1:0, 2:1, 4:2, 8:3, 16:4, 32:5, 64:6, 128:7, 256:8, 512:9, 1024:10}
      try:
        nSegments=self.num_segments.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Number of Segments')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      segmentCode = segmentDict[nSegments]
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x800c), byref(c_int(segmentCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing number of segments' )
        raise mdsExceptions.TclFAILED_ESSENTIAL
#Global Channel Configuration
      trigModeDict = {'OVER THRESHOLD':0, 'UNDER THRESHOLD':1}
      try:
        trigMode = self.trig_mode.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Trigger mode')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      trigModeCode = trigModeDict[trigMode]
      conf = trigModeCode << 6
      conf = conf | 0x00000010
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8000), byref(c_int(conf)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing group configuration')
        raise mdsExceptions.TclFAILED_ESSENTIAL

#Channel configurations
      trigEnableCode = 0
      chanEnableCode = 0
      enabledDict = {'ENABLED':1, 'DISABLED':0}
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
#threshold level
        threshold = getattr(self, 'channel_%d_thresh_level'%(chan+1)).data()
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1080 + chan * 0x100), byref(c_int(threshold)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold level')
          raise mdsExceptions.TclFAILED_ESSENTIAL
#threshold samples
        threshSamples = getattr(self, 'channel_%d_thresh_sampl'%(chan+1)).data()
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1084 + chan * 0x100), byref(c_int(threshSamples)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing threshold samples')
          raise mdsExceptions.TclFAILED_ESSENTIAL
#offset
        offset = getattr(self, 'channel_%d_offset'%(chan+1)).data()
        if(offset > 1):
          offset = 1.
        if(offset < -1):
          offset = -1
        offset = (offset / 1.) * 32767
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x1098 + chan * 0x100), byref(c_int(int(offset + 0x08000))), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing DAC offset')
          raise mdsExceptions.TclFAILED_ESSENTIAL
#states
        state = getattr(self, 'channel_%d_state'%(chan+1)).data()
        chanEnableCode = chanEnableCode | (enabledDict[state] << chan)
        trigState = getattr(self, 'channel_%d_trig_state'%(chan+1)).data()
        trigEnableCode = trigEnableCode | (enabledDict[trigState] << chan)

#endfor chan in range(0,numChannels)

#Set channel enabled mask
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8120), byref(c_int(chanEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel enable register')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing Channel trigger enable register')
        raise mdsExceptions.TclFAILED_ESSENTIAL

#Set trigger enabling
      trigExt = self.trig_ext.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigExt] << 30)
      trigSoft = self.trig_soft.data()
      trigEnableCode = trigEnableCode | (enabledDict[trigSoft] << 31)
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error writing trigger configuration')
        raise mdsExceptions.TclFAILED_ESSENTIAL

#Front Panel trigger out setting set TRIG/CLK to TTL
      data = 1
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x811C), byref(c_int(data)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

#trigger source
      try:
        trigSource = self.trig_source.data()
#if trigger is expressed as an array, consider only the first element
        if len(self.trig_source.getShape()) > 0:
          trigSource = trigSource[0]
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source')
        raise mdsExceptions.TclFAILED_ESSENTIAL

#Clock source
      clockMode = self.clock_mode.data()
      if clockMode == 'EXTERNAL':
        try:
          clockSource = self.clock_source()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Clock source')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      else:
        clockSource = Range(None, None, Float64(1/250E6))
        self.clock_source.putData(clockSource)

#Post Trigger Samples
      try:
        pts = self.pts.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve PTS Samples')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      segmentSize = 1048576/nSegments
      if pts > segmentSize:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'PTS Larger than segmentSize')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8114), byref(c_int(pts>>2)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

#Time management
      useTime=self.use_time.data()
      if useTime == 'YES':
        try:
          startTime = self.start_time.data()
          endTime = self.end_time.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        if endTime > 0:
          endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, segmentSize, trigSource), clockSource), Float64(endTime + trigSource))
        else:
          endIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, segmentSize, trigSource + endTime), clockSource), Float64(trigSource))
        self.end_idx.putData(Int32(endIdx + 0.5))
        if startTime > 0:
          startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, segmentSize, trigSource), clockSource), startTime + trigSource)
        else:
          startIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, segmentSize, trigSource + startTime), clockSource), trigSource)
        self.start_idx.putData(Int32(startIdx + 0.5))
#Internal/External clock
#      print 'startIdx: ', startIdx
#      print 'endIdx: ', endIdx
#      print 'SEGMENT SIZE: ', segmentSize, pts
#      print 'PTS: ', pts
      currStartIdx = segmentSize - pts + startIdx.data()
#      print 'currStartIdx: ', currStartIdx
      if currStartIdx < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/pre-trigger samples')
          raise mdsExceptions.TclFAILED_ESSENTIAL

      currEndIdx = segmentSize - pts + endIdx.data()
#      print 'segmentSize: ', segmentSize
#      print 'PTS: ', pts
#      print 'endIdx: ', endIdx
#      print 'currEndIdx: ', currEndIdx
      if currEndIdx >= segmentSize:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid segment size/post-trigger samples')
          raise mdsExceptions.TclFAILED_ESSENTIAL

      acqMode = self.acq_mode.data()
      if acqMode == 'CONTINUOUS' or acqMode == 'CONTINUOUS WITH COUNTER':
        irqEvents = self.irq_events.data()
        irqEvents = irqEvents - 1
        if irqEvents < 1:
          irqEvents = 1
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF18), byref(c_int(irqEvents)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ events')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0xEF00), byref(c_int(0x09)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error setting IRQ line')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        status = CAENDT5720.caenLib.CAENVME_IRQEnable(self.handle, c_int(0x01))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error Enabling IRQ')
          raise mdsExceptions.TclFAILED_ESSENTIAL
#Start asynchronous readout
        self.start_store()
      #endif acqMode == 'CONTINUOUS SAMPLING'
# Run device
      runCommand = 4
      if clockMode == 'EXTERNAL':
        runCommand = runCommand | 0x00000040
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(4)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      self.saveInfo()
      return


################################TRIGGER###################################

    def trigger(self):
      self.restoreInfo()
      try:
        vmeAddress = 0
  #Module SW trigger
        status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8108), byref(c_int(0)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
          print('Error resetting V1740 Device')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        return
      except:
        print('Generic SW trigger Error')
        raise mdsExceptions.TclFAILED_ESSENTIAL

####################################STORE###################################

    def store(self):
      self.restoreInfo()
      vmeAddress = 0
# Stop device
      status = CAENDT5720.caenLib.CAENVME_WriteCycle(self.handle, c_int(vmeAddress + 0x8100), byref(c_int(0)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error stopping device')
        raise mdsExceptions.TclFAILED_ESSENTIAL
    #need to wait a while
      sleep(0.1)
      acqMode = self.acq_mode.data()
      if acqMode == 'CONTINUOUS' or acqMode == 'CONTINUOUS WITH COUNTER':
        self.stop_store(0)
        return


      try:
        clock = self.clock_source.evaluate()
        dt = clock.getDelta().data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating clock source' )
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        trig = self.trig_source.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating trigger source' )
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        startIdx = self.start_idx.data()
        endIdx = self.end_idx.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating start or end idx')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        pts = self.pts.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error evaluating Post Trigger Samples' )
        raise mdsExceptions.TclFAILED_ESSENTIAL



    # Read number of buffers
      actSegments = c_int(0)
      status = CAENDT5720.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading number of acquired segments' )
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if actSegments.value == 0:
        return


    #Compute Segment Size
      try:
        nSegments = self.num_segments.data()
        segmentSamples = 1048576/nSegments
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading max number of segments' )
        raise mdsExceptions.TclFAILED_ESSENTIAL


    # Get Active channels
      chanMask = c_int(0)
      status = CAENDT5720.caenLib.CAENVME_ReadCycle(self.handle, c_int(vmeAddress + 0x8120), byref(chanMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
      nActChans = 0
      chanMask = chanMask.value
      numChannels = self.num_channels.data()
      for chan in range(0,numChannels):
        if (chanMask & (1 << chan)) != 0:
          nActChans = nActChans + 1
      if nActChans == 0:
        print('No active groups')
        return

      segmentSize = 16 + 2*segmentSamples * nActChans

      class DT5720Data(Structure):
        _fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_short * (segmentSize / 2))]

      actSegments = actSegments.value

      currStartIdx = segmentSamples - pts + startIdx
      currEndIdx = segmentSamples - pts + endIdx
      currChanSamples = Int32(currEndIdx - currStartIdx + 0.5).data()
      triggers = []
      deltas = []
      channels = [None]*numChannels
      for chan in range(numChannels):
        channels[chan] = ndarray(currChanSamples * actSegments)
      for segmentIdx in range(0,actSegments):
        segment= DT5720Data()
        retLen = c_int(0)
        status = CAENDT5720.caenLib.CAENVME_FIFOBLTReadCycle(self.handle, c_int(vmeAddress), byref(segment), c_int(segmentSize),  c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading data segment')
          raise mdsExceptions.TclFAILED_ESSENTIAL

        actSize = 4 * (segment.eventSize & 0x0fffffff)
        if actSize != segmentSize:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Expected event size different from expected size' )
          raise mdsExceptions.TclFAILED_ESSENTIAL
        counter = segment.time/2
        triggers.append(counter*dt)
        deltas.append(dt)
        sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
        chanSizeInInts = sizeInInts/nActChans
        chanSizeInShorts = chanSizeInInts * 2
        #chanOffset = 0
        for chan in range(numChannels):
          if (chanMask & (1 << chan)) != 0:
            channels[chan][segmentIdx * currChanSamples : segmentIdx * currChanSamples + currEndIdx - currStartIdx] = segment.data[chan*chanSizeInShorts+currStartIdx:chan*chanSizeInShorts+currEndIdx]
        #endfor  chan in range(numChannels)
      #endfor segmentIdx in range(actSegments):
      if len(self.trig_source.getShape()) > 0:
        dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig[0]),Range(Float64Array(trig) + Float64(startIdx * dt),  Float64Array(trig) + Float64(endIdx * dt), Float64Array(deltas)))
      else:
        dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig),Range(Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(startIdx * dt),  Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(endIdx * dt), Float64Array(deltas)))
      dim.setUnits("s");
      for chan in range(numChannels):
        if (chanMask & (1 << chan)) != 0:
          try:
            offset = getattr(self, 'channel_%d_offset'%(chan+1))
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading channel offset')
            raise mdsExceptions.TclFAILED_ESSENTIAL
          raw = Int16Array(channels[chan])
          raw.setUnits("counts")
          data = Data.compile("2*($VALUE - 2048)/4096.+$1", offset);
          data.setUnits("Volts")
          signal = Signal(data, raw, dim)
          try:
            getattr(self, 'channel_%d_data'%(chan+1)).putData(signal)
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot write Signal in tree')
            raise mdsExceptions.TclFAILED_ESSENTIAL
      #endfor chan in range(numChannels)
      return



