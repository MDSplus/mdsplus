from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import time
class ZELOS2150GV(Device):
    print 'Zelos2150GV'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Zelos 2150GV Camera"""
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
    print 'Zelos2150GV added'
    
    
    handle = 0

####Asynchronous readout internal class       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False
       
      def configure(self, device, kappaLib, width, height, hBuffers):
        self.device = device
        self.kappaLib = kappaLib
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
	status = self.kappaLib.kappaOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
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

          status = self.kappaLib.kappaSaveFrame(self.device.handle, self.width, self.height, frameBuffer, isExternal, treePtr, self.device.frames.getNid(), self.device.frame_clock.getNid(), c_int(self.idx), byref(prevFrameTime), isStorage) 


          if ( (isStreaming==1) and (tcpStreamHandle.value==-1) ):   
            self.kappaLib.kappaOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)   
            print 'kappa Open Tcp Connection!!!! handle:'
            print tcpStreamHandle.value
          if ( (isStreaming==1) and (tcpStreamHandle.value!=-1) ): 
              self.kappaLib.kappaSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frameBuffer, frame8bit);

          self.idx = self.idx + 1

        #endwhile
        self.kappaLib.kappaCloseTcpConnection(byref(tcpStreamHandle))
        print 'kappaCloseTcpConnection'
    
	status = self.kappaLib.kappaStopAcquisition(self.device.handle, self.hBuffers)
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        #close device and remove from info
        self.kappaLib.kappaClose(self.device.handle)
	self.device.removeInfo()
        return 0
      def stop(self):
        self.stopReq = True
  #end class AsynchStore




###save worker###  
    def saveWorker(self):
      global zelosWorkers
      global zelosWorkerNids
      try:
        zelosWorkers
      except:
	zelosWorkerNids = []
        zelosWorkers = []
      try:
        idx = zelosWorkerNids.index(self.getNid())
        zelosWorkers[idx] = self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        zelosWorkerNids.append(self.getNid())
        zelosWorkers.append(self.worker)
        return
      return

###save Info###   
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices 
#and to avoid opening and closing devices handles 
    def saveInfo(self):
      global zelosHandles
      global zelosNids
      try:
        zelosHandles
      except:
	zelosHandles = []
	zelosNids = []
      try:
        idx = zelosNids.index(self.getNid())
      except:
        print 'SAVE INFO: SAVING HANDLE'
        zelosHandles.append(self.handle)
        zelosNids.append(self.getNid())
        return
      return

###restore worker###   
    def restoreWorker(self):
      global zelosWorkerNids
      global zelosWorkers    
      try:
        idx = zelosWorkerNids.index(self.getNid())
        self.worker = zelosWorkers[idx]
      except:
        print 'Cannot restore worker!!'

###restore info###   
    def restoreInfo(self):
      global zelosHandles
      global zelosNids
      global kappaLib
      try:
        kappaLib
      except:
        kappaLib = CDLL("libkappazelos.so")
      try:
        idx = zelosNids.index(self.getNid())
        self.handle = zelosHandles[idx]
        print 'RESTORE INFO HANDLE TROVATO'
      except:
        print 'RESTORE INFO HANDLE NON TROVATO'
        try: 
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          return 0
        try: 
          self.handle = c_void_p(0)
          kappaLib.kappaOpen(c_char_p(name), byref(self.handle))
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
          return 0
      return

###remove info###    
    def removeInfo(self):
      global zelosHandles
      global zelosNids
      try:
        zelosNids.remove(self.getNid())
        zelosHandles.remove(self.handle)
      except:
        print 'ERROR TRYING TO REMOVE INFO'
      return


##########init############################################################################    
    def init(self,arg):
      global kappaLib
      self.restoreInfo()
      try: 
       # kappaLib.kappaBaseSetup(self.handle, c_int(0), c_int(0))
	kappaLib.kappaSetColorCoding(self.handle, c_int(6));  #Y14
        if self.frame_sync.data() == 'EXTERNAL': 
          kappaLib.kappaSetExposureMode(self.handle, c_int(3))
        else:
          kappaLib.kappaSetExposureMode(self.handle, c_int(2))

#2 = ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL 
#3 = ZELOS_ENUM_EXPOSUREMODE_RESETRESTART 
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot perform base setup ')
        return 0
###Exposure	
      autoExp = self.auto_exp.data()
      if autoExp == 'YES':
      	kappaLib.kappaSetAET(self.handle, c_int(1))
        try:
	  kappaLib.kappaSetAutoExposureLevel(self.handle, c_int(self.exp_lev.data()))
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set exposure level')
          return 0
      else:
      	kappaLib.kappaSetAET(self.handle, c_int(0))
        try:
	  kappaLib.kappaSetExposure(self.handle, c_float(self.exp_time.data()))
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set exposure time')
          return 0
###Gain
      autoGain = self.auto_gain.data()
      if autoGain == 'YES':
      	kappaLib.kappaSetAGC(self.handle, c_int(1))
      else:
      	kappaLib.kappaSetAGC(self.handle, c_int(0))
        try:
	  kappaLib.kappaSetGain(self.handle, c_int(self.gain_lev.data()))
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set gain level')
          return 0
      slowScan = self.slow_scan.data()
      if slowScan == 'YES':
	  kappaLib.kappaSetSlowScan(self.handle, c_int(1))
      else:
	  kappaLib.kappaSetSlowScan(self.handle, c_int(0))
###Frame Area
      try:
     	kappaLib.kappaSetReadoutArea(self.handle, c_int(self.frame_x.data()),c_int(self.frame_y.data()),c_int(self.frame_width.data()),c_int(self.frame_height.data()))
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set frame area')
        return 0
###Measure Area
      try:
     	kappaLib.kappaSetMeasureWindow(self.handle, c_int(self.meas_x.data()),c_int(self.meas_y.data()),c_int(self.meas_width.data()),c_int(self.meas_height.data()))
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set measure window')
        return 0
###Binning
      try:
        kappaLib.kappaSetBinning(self.handle, c_int(self.hor_binning), c_int(self.ver_binning))
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set horizontal or vertical binning')
        return 0
      self.saveInfo()
      return 1

####################trigger###PER ORA NON FUNZIONA
    def trigger(self,arg):
      global kappaLib
      self.restoreInfo()

      synch = self.frame_sync.data();  ###Synchronization
      if synch == 'INTERNAL':
        try:
          timeMs = int(self.frame_period.data()/1E-3)
	  print timeMs
	  kappaLib.kappaSetTriggerTimer(self.handle, c_int(timeMs))		
	except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set Frame period in internal sync mode')
          return 0
      else:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot issue software trigger if external synchornization')
        return 0
      self.saveInfo()
      return 1
		
##########start store############################################################################   
    def start_store(self, arg):
      global kappaLib
      self.restoreInfo()
      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
      hBuffers = c_void_p(0)
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = kappaLib.kappaStartAcquisition(self.handle, byref(hBuffers), byref(width), byref(height), byref(payloadSize))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        return 0
      self.worker.configure(self, kappaLib, width, height, hBuffers)
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
