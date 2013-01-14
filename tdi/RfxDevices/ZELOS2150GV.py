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
      {'path':':EXP_TIME', 'type':'numeric', 'value':50E-3},
      {'path':':AUTO_EXP', 'type':'text', 'value':'OFF'},
      {'path':':EXP_LEV', 'type':'numeric', 'value':0},
      {'path':':AUTO_GAIN', 'type':'text', 'value':'OFF'},
      {'path':':GAIN_LEV', 'type':'numeric', 'value':100},
      {'path':':SLOW_SCAN', 'type':'text', 'value':'OFF'},
      {'path':':FRAME_X', 'type':'numeric', 'value':0},
      {'path':':FRAME_Y', 'type':'numeric', 'value':0},
      {'path':':FRAME_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':FRAME_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':MEAS_X', 'type':'numeric', 'value':0},
      {'path':':MEAS_Y', 'type':'numeric', 'value':0},
      {'path':':MEAS_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':MEAS_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':VER_BINNING', 'type':'numeric', 'value':1},
      {'path':':HOR_BINNING', 'type':'numeric', 'value':1},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'INTERNAL'},
      {'path':':FRAME_PERIOD', 'type':'numeric', 'value':100E-3},
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal'}]
    print 'Zelos2150GV 1'
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_acq',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_acq',head))",
	  'options':('no_write_shot',)})
    print 'Zelos2150GV'
    
    
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
        frameType = c_short * (self.height.value * self.width.value)
        frameBuffer = frameType()
	self.idx = 0
	treePtr = c_void_p(0);
	status = self.kappaLib.kappaOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
	if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          return 0
        
	while not self.stopReq:
#	for i in range(0,5):
          if self.device.frame_sync.data() == 'EXTERNAL': 
            isExternal = c_int(1)
          else:
            isExternal = c_int(0)
          status = self.kappaLib.kappaSaveFrame(self.device.handle, self.width, self.height, frameBuffer, isExternal, treePtr, self.device.frames.getNid(), self.device.frame_clock.getNid(), c_int(self.idx))
	  if status == -1:
            Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot save frame')
#            return 0
	  self.idx = self.idx + 1
    #endwhile
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

#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices and to avoid opening
# and closing devices handles 
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

    def restoreWorker(self):
      global zelosWorkerNids
      global zelosWorkers
      
      try:
        idx = zelosWorkerNids.index(self.getNid())
        self.worker = zelosWorkers[idx]
      except:
        print 'Cannot restore worker!!'

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
    
    def removeInfo(self):
      global zelosHandles
      global zelosNids
      zelosNids.remove(self.getNid())
      zelosHandles.remove(self.handle)
      return
##########init############################################################################    
    def init(self,arg):
      global kappaLib
      self.restoreInfo()
      try: 
        kappaLib.kappaBaseSetup(self.handle)
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

###Synchronization
      synch = self.frame_sync.data();
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
		
################StartStore
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

    def stop_store(self,arg):
      print 'STOP STORE'
      self.restoreWorker()
      self.worker.stop()
      print 'FLAG SETTATO'
      return 1
