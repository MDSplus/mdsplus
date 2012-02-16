from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import time
class FAKECAMERA(Device):
    print 'Fake Camera'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """fake Camera"""
    parts=[
      {'path':':NAME', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':IP', 'type':'text'},
      {'path':':EXP_TIME', 'type':'numeric', 'value':20E-3},
      {'path':':AUTO_EXP', 'type':'text', 'value':'NO'},
      {'path':':EXP_LEV', 'type':'numeric', 'value':0},
      {'path':':AUTO_GAIN', 'type':'text', 'value':'NO'},
      {'path':':GAIN_LEV', 'type':'numeric', 'value':100},
      {'path':':SLOW_SCAN', 'type':'text', 'value':'NO'},
      {'path':':FRAME_X', 'type':'numeric', 'value':11},
      {'path':':FRAME_Y', 'type':'numeric', 'value':11},
      {'path':':FRAME_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':FRAME_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':MEAS_X', 'type':'numeric', 'value':11},
      {'path':':MEAS_Y', 'type':'numeric', 'value':11},
      {'path':':MEAS_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':MEAS_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':VER_BINNING', 'type':'numeric', 'value':1},
      {'path':':HOR_BINNING', 'type':'numeric', 'value':1},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'INTERNAL'},
      {'path':':FRAME_PERIOD', 'type':'numeric', 'value':100E-3},
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':STREAMING', 'type':'text', 'value':'Stream and Store'},
      {'path':':STREAM_PORT', 'type':'numeric', 'value':8888},]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot',)})
    print 'Fake Camera added'
    
    
    handle = 0

####Asynchronous readout internal class       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False
       
      def configure(self, device, FakeCamLib, width, height, hBuffers):
        self.device = device
        self.FakeCamLib = FakeCamLib
        self.width = width
        self.height = height
        self.hBuffers = hBuffers


      def run(self):
        frameType = c_short * (self.height.value * self.width.value) #used for acquired frame
        frameBuffer = frameType()

        frameType = c_byte * (self.height.value * self.width.value)  #used for streaming frame
        frame8bit = frameType()

	self.idx = 0


	treePtr = c_void_p(0);
	status = self.FakeCamLib.fakeOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
	if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          return 0
        if self.device.frame_sync.data() == 'EXTERNAL': 
          isExternal = c_int(1)
        else:
          isExternal = c_int(0)
       

        if self.device.streaming.data() == 'Stream and Store': 
          isStreaming = 1
          isStorage = c_int(1)
        if self.device.streaming.data() == 'Only Stream': 
          isStreaming = 1
          isStorage = c_int(0)
        if self.device.streaming.data() == 'Only Store': 
          isStreaming = 0
          isStorage = c_int(1)


        tcpStreamHandle=c_int(-1)
        prevFrameTime=c_int64(0)
        status=-1
        streamPort=c_int(self.device.stream_port.data())


        while not self.stopReq:

          status = self.FakeCamLib.fakeSaveFrame(self.device.handle, self.width, self.height, frameBuffer, isExternal, treePtr, self.device.frames.getNid(), self.device.frame_clock.getNid(), c_int(self.idx), byref(prevFrameTime), isStorage) 


          if ( (isStreaming==1) and (tcpStreamHandle.value==-1) ):   
            self.FakeCamLib.fakeOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)   
            print 'Fake camera Open Tcp Connection!!!! handle:'
            print tcpStreamHandle.value
          if ( (isStreaming==1) and (tcpStreamHandle.value!=-1) ): 
              self.FakeCamLib.fakeSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frameBuffer, frame8bit);

          if status == 0:
              self.idx = self.idx + 1

        #endwhile
        self.FakeCamLib.fakeCloseTcpConnection(byref(tcpStreamHandle))
        print 'FakeCameraCloseTcpConnection'
    
	status = self.FakeCamLib.fakeStopAcquisition(self.device.handle, self.hBuffers)
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        #close device and remove from info
        self.FakeCamLib.fakeClose(self.device.handle)
	self.device.removeInfo()
        return 0
      def stop(self):
        self.stopReq = True
  #end class AsynchStore




###save worker###  
    def saveWorker(self):
      global fakeWorkers
      global fakeWorkerNids
      try:
        fakeWorkers
      except:
	fakeWorkerNids = []
        fakeWorkers = []
      try:
        idx = fakeWorkerNids.index(self.getNid())
        fakeWorkers[idx] = self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        fakeWorkerNids.append(self.getNid())
        fakeWorkers.append(self.worker)
        return
      return

###save Info###   
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices 
#and to avoid opening and closing devices handles 
    def saveInfo(self):
      global fakeHandles
      global fakeNids
      try:
        fakeHandles
      except:
	fakeHandles = []
	fakeNids = []
      try:
        idx = fakeNids.index(self.getNid())
      except:
        print 'SAVE INFO: SAVING HANDLE'
        fakeHandles.append(self.handle)
        fakeNids.append(self.getNid())
        return
      return

###restore worker###   
    def restoreWorker(self):
      global fakeWorkerNids
      global fakeWorkers    
      try:
        idx = fakeWorkerNids.index(self.getNid())
        self.worker = fakeWorkers[idx]
      except:
        print 'Cannot restore worker!!'

###restore info###   
    def restoreInfo(self):
      global fakeHandles
      global fakeNids
      global FakeCamLib
      try:
        FakeCamLib
      except:
        FakeCamLib = CDLL("libfakecamera.so")
      try:
        idx = fakeNids.index(self.getNid())
        self.handle = fakeHandles[idx]
        print 'RESTORE INFO HANDLE TROVATO'
      except:
        print 'RESTORE INFO HANDLE NON TROVATO'
        try: 
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          return 0

        self.handle = c_void_p(0)
        status = FakeCamLib.fakeOpen(c_char_p(name), byref(self.handle))
        if status < 0:  
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
          return 0
      return

###remove info###    
    def removeInfo(self):
      global fakeHandles
      global fakeNids
      try:
        fakeNids.remove(self.getNid())
        fakeHandles.remove(self.handle)
      except:
        print 'ERROR TRYING TO REMOVE INFO'
      return


##########init############################################################################    
    def init(self,arg):
      global FakeCamLib
      self.restoreInfo()
      self.frames.setCompressOnPut(False)	

      status = FakeCamLib.fakeSetColorCoding(self.handle, c_int(6));  #Y14
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Color Coding')
        return 0

###Exposure	Mode
      if self.frame_sync.data() == 'EXTERNAL': 
        status = FakeCamLib.fakeSetExposureMode(self.handle, c_int(3)) #3 = fake_ENUM_EXPOSUREMODE_RESETRESTART 
      else:
        status = FakeCamLib.fakeSetExposureMode(self.handle, c_int(2)) #2 = fake_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL

      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Exposure Mode')
        return 0

###Exposure	
      autoExp = self.auto_exp.data()
      if autoExp == 'YES':
      	status = FakeCamLib.fakeSetAET(self.handle, c_int(1))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AET On')
          return 0
        status = FakeCamLib.fakeSetAutoExposureLevel(self.handle, c_int(self.exp_lev.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Auto Exposure Level')
          return 0
      else:
      	status = FakeCamLib.fakeSetAET(self.handle, c_int(0))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AET Off')
          return 0
        status = FakeCamLib.fakeSetExposure(self.handle, c_float(self.exp_time.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Exposure Time')
          return 0
          
###Gain
      autoGain = self.auto_gain.data()
      if autoGain == 'YES':
      	status = FakeCamLib.fakeSetAGC(self.handle, c_int(1))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AGC On')
          return 0
      else:
      	status = FakeCamLib.fakeSetAGC(self.handle, c_int(0))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AGC On')
          return 0
        status = FakeCamLib.fakeSetGain(self.handle, c_int(self.gain_lev.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Gain')
          return 0

###Slow Scan
      slowScan = self.slow_scan.data()
      if slowScan == 'YES':
       status = FakeCamLib.fakeSetSlowScan(self.handle, c_int(1))
      else:
       status = FakeCamLib.fakeSetSlowScan(self.handle, c_int(0))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Slow Scan')
        return 0

###Frame Area
      status = FakeCamLib.fakeSetReadoutArea(self.handle, c_int(self.frame_x.data()),c_int(self.frame_y.data()),c_int(self.frame_width.data()),c_int(self.frame_height.data()))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Readout Area')
        return 0

###Measure Area
      status = FakeCamLib.fakeSetMeasureWindow(self.handle, c_int(self.meas_x.data()),c_int(self.meas_y.data()),c_int(self.meas_width.data()),c_int(self.meas_height.data()))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Measure Window')
        return 0

###Binning
      status = FakeCamLib.fakeSetBinning(self.handle, c_int(self.hor_binning), c_int(self.ver_binning))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set horizontal or vertical binning')
        return 0

      self.saveInfo()
      return 1

####################trigger###PER ORA NON FUNZIONA
    def trigger(self,arg):
      global FakeCamLib
      self.restoreInfo()

      synch = self.frame_sync.data();  ###Synchronization
      if synch == 'INTERNAL':
        timeMs = int(self.frame_period.data()/1E-3)
        status = FakeCamLib.fakeSetTriggerTimer(self.handle, c_int(timeMs))		
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set Frame period in internal sync mode')
          return 0
      else:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot issue software trigger if external synchornization')
        return 0
      self.saveInfo()
      return 1
		
##########start store############################################################################   
    def start_store(self, arg):
      global FakeCamLib
      self.restoreInfo()
      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
      hBuffers = c_void_p(0)
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = FakeCamLib.fakeStartAcquisition(self.handle, byref(hBuffers), byref(width), byref(height), byref(payloadSize))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        return 0
      self.worker.configure(self, FakeCamLib, width, height, hBuffers)
      self.saveWorker()
      self.worker.start()
      return 1


##########stop store############################################################################   
    def stop_store(self,arg):
      print 'STOP STORE'
      self.restoreWorker()
      self.worker.stop()
      print 'FLAG SETTATO'
      return 1
