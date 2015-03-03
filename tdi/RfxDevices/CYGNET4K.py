from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time

class CYGNET4K(Device):
    print 'Cygnet4K'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':ID', 'type':'numeric', 'value':1},
      {'path':':EXP_TIME', 'type':'numeric', 'value':20E-3},
      {'path':':BINNING', 'type':'text', 'value':'1x1'},
      {'path':':ROI_X', 'type':'numeric', 'value':0},
      {'path':':ROI_Y', 'type':'numeric', 'value':0},
      {'path':':ROI_WIDTH', 'type':'numeric', 'value':2048},
      {'path':':ROI_HEIGHT', 'type':'numeric', 'value':2048},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'EXTERNAL'},
      {'path':':FRAME_FREQ', 'type':'numeric', 'value':10.},
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':TRIG_TIME', 'type':'numeric', 'value':0.},
      {'path':':DURATION', 'type':'numeric', 'value':1.},
      {'path':':SENSOR_TEMP', 'type':'numeric'},
      {'path':':PCB_TEMP', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot')})
    print 'Cygnet4K added'
 



####Asynchronous readout internal class       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False
   
    
      def configure(self, device, id, mdsLib, raptorLib, xPixels, yPixels, duration):
        self.device = device
        self.id = id
        self.mdsLib = mdsLib
        self.raptorLib = raptorLib
        self.xPixels = xPixels
        self.yPixels = yPixels
        self.duration = duration


      def run(self):
        treePtr = c_void_p(0)
        status = self.mdsLib.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
        if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          return 0
                
        listPtr = c_void_p(0);
        self.mdsLib.camStartSave(byref(listPtr))


        currTime = c_float(0)
        frameIdx = c_int(0)
        baseTicks = c_int(-1)
        timeoutMs = c_int(500)
        bufIdx = c_int(-1)
        timebaseNid = self.device.frame_clock.getNid()
        framesNid = self.device.frames.getNid()
        measuredTimes = []
        currDuration = c_float(0)
        prevTime = self.device.trig_time.data()
        stopped = False

        self.raptorLib.epixStartVideoCapture(c_int(self.id))
        while not self.stopReq:
          if(self.duration <= 0 or currDuration < self.duration):
            self.raptorLib.epixCaptureFrame(c_int(self.id), frameIdx, bufIdx, baseTicks, c_int(self.xPixels), c_int(self.yPixels), c_int(framesNid), c_int(timebaseNid), treePtr, listPtr, timeoutMs, byref(frameIdx), byref(bufIdx), byref(baseTicks), byref(currDuration))
            currTime = self.device.trig_time.data() + currDuration
            if currTime > prevTime:
              measuredTimes.append(currTime)
              prevTime = currTime
              if(self.duration > 0 and currDuration >= self.duration):
                self.raptorLib.epixStopVideoCapture(c_int(self.id));
                self.device.frame_clock.putData(Float32Array(measuredTimes))
                stopped = True
            else: 
              time.sleep(0.5)  #do nothing , only wait stop

#Finished storing frames, stop camera integration and store measured frame times
        if (not stopped):
          self.raptorLib.epixStopVideoCapture(c_int(self.id));
          self.device.frame_clock.putData(Float32Array(measuredTimes))
        self.mdsLib.camStopSave(listPtr)        
        return 0


      def stop(self):
        self.stopReq = True
#end class AsynchStore




###save worker###  
    def saveWorker(self):
      global raptorWorkers
      global raptorWorkerNids
      try:
        raptorWorkers
      except:
        raptorWorkerNids = []
        raptorWorkers = []
      try:
        idx = raptorWorkerNids.index(self.getNid())
        raptorWorkers[idx] = self.worker
      except:
        raptorWorkerNids.append(self.getNid())
        raptorWorkers.append(self.worker)
        return
      return


###restore worker###   
    def restoreWorker(self):
      global raptorWorkers    
      global raptorWorkerNids
      try:
        idx = raptorWorkerNids.index(self.getNid())
        self.worker = raptorWorkers[idx]
      except:
        print 'Cannot restore worker!!'

##Check for libraries
    def checkLibraries(self):
      global raptorLib
      global mdsLib
      try:
        raptorLib
      except:
        raptorLib = CDLL("libRaptor.so")
      try:
        mdsLib
      except:
        mdsLib = CDLL("libcammdsutils.so")
 
##########init############################################################################    
    def init(self,arg):
      global raptorLib
      global mdsLib
      self.frames.setCompressOnPut(False)	
      self.checkLibraries()
      confPath = self.conf_file.data()
      xPixels = c_int(0)
      yPixels = c_int(0)
      raptorLib.epixOpen(c_char_p(confPath), byref(xPixels), byref(yPixels))
      idx = self.id.data();
      if idx <= 0:
        print 'Wrong value of Device Id, must be greater than 0'
        return 0
      frameRate = self.frame_freq.data();
      exposure = self.exp_time.data()
      trigMode = self.frame_sync.data()
      codedTrigMode = 0
      if(trigMode == 'EXTERNAL RISING'): 
        codedTrigMode = 0xC0
      if(trigMode == 'EXTERNAL FALLING'): 
        codedTrigMode = 0x40
      if(trigMode == 'INTEGRATE THEN READ'): 
        codedTrigMode = 0x0C
      if(trigMode == 'FIXED FRAME RATE'): 
        codedTrigMode = 0x0E
    
      raptorLib.epixSetConfiguration(c_int(idx), c_float(frameRate), c_float(exposure), c_int(codedTrigMode))
      PCBTemperature = c_float(0)
      CMOSTemperature = c_float(0)
      binning = c_int(0)
      roiXSize = c_int(0)
      roiXOffset = c_int(0)
      roiYSize = c_int(0)
      roiYOffset = c_int(0)
      
      raptorLib.epixGetConfiguration(c_int(idx), byref(PCBTemperature), byref(CMOSTemperature), byref(binning), byref(roiXSize), byref(roiXOffset), byref(roiYSize), byref(roiYOffset))


      self.sensor_temp.putData(Float32(CMOSTemperature))
      self.pcb_temp.putData(Float32(PCBTemperature))
      if(binning == 0x00):
        self.binning.putData('1x1')
      if(binning == 0x11):
        self.binning.putData('2x2')
      if(binning == 0x22):
        self.binning.putData('4x4')
      self.roi_x.putData(Int32(roiXOffset))
      self.roi_width.putData(Int32(roiXSize))
      self.roi_y.putData(Int32(roiYOffset))
      self.roi_height.putData(Int32(roiYSize))

      return 1
		
##########start store############################################################################   
    def start_store(self, arg):
      global raptorLib
      global mdsLib
      self.checkLibraries()
      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
      self.worker.configure(self, self.id.data(), mdsLib, raptorLib, self.roi_width.data(), self.roi_height.data(), self.duration.data())
      self.saveWorker()
      self.worker.start()

      return 1

##########stop store############################################################################   
    def stop_store(self,arg):
      self.restoreWorker()
      self.worker.stop()
      return 1









   
 
