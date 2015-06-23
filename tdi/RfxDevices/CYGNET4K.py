from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time
import tempfile
import os

class CYGNET4K(Device):
    print 'Cygnet4K'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':ID', 'type':'numeric', 'value':1},
      {'path':':EXP_TIME', 'type':'numeric', 'value':1.}, # msec
      {'path':':BINNING', 'type':'text', 'value':'1x1'},
      {'path':':ROI_X', 'type':'numeric', 'value':0},
      {'path':':ROI_Y', 'type':'numeric', 'value':0},
      {'path':':ROI_WIDTH', 'type':'numeric', 'value':2048},
      {'path':':ROI_HEIGHT', 'type':'numeric', 'value':2048},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'EXTERNAL RISING'},
      {'path':':FRAME_FREQ', 'type':'numeric', 'value':10.}, # Hz
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':TRIG_TIME', 'type':'numeric', 'value':0.},
      {'path':':DURATION', 'type':'numeric', 'value':1.},
      {'path':':CMOS_TEMP', 'type':'numeric'},
      {'path':':PCB_TEMP', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot')})
    parts.append({'path':':START_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_temp_monitor',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_temp_monitor',head))",
	  'options':('no_write_shot')})
    print 'Cygnet4K added'
 

####Asynchronous temp readout internal class
    class AsynchTemp(Thread):
      
      def configure(self, device, id, mdsLib, raptorLib):
        self.device = device
        self.id = id
        self.mdsLib = mdsLib
        self.raptorLib = raptorLib
        self.stopReq = False
 
      def run(self):
        pcbTemp = c_float(0)
        cmosTemp = c_float(0)
        currTime = c_long(0)
        tempIdx = 0;
        while (not self.stopReq):
          self.raptorLib.epixGetTemp(c_int(1), c_int(tempIdx), byref(pcbTemp), byref(cmosTemp), byref(currTime))
          self.device.pcb_temp.putRow(1024, Float32(pcbTemp), Uint64(currTime))
          self.device.cmos_temp.putRow(1024, Float32(cmosTemp), Uint64(currTime))
          time.sleep(0.5)

        return 0

      def stop(self):
        self.stopReq = True
############################################################


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


        currTime = 0.0
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
#        print('  capture started')
#        ts = time.time()
        while not self.stopReq:
          if(self.duration <= 0 or currDuration < self.duration):
            self.raptorLib.epixCaptureFrame(c_int(self.id), frameIdx, bufIdx, baseTicks, c_int(self.xPixels), c_int(self.yPixels), c_int(framesNid), c_int(timebaseNid), treePtr, listPtr, timeoutMs, byref(frameIdx), byref(bufIdx), byref(baseTicks), byref(currDuration))
            currTime = self.device.trig_time.data() + currDuration.value
            if currTime > prevTime:
#              print('  %0.3f since start of capture' % (time.time() - ts))
              measuredTimes.append(currTime)
              prevTime = currTime
              if(self.duration > 0 and currDuration >= self.duration):
                self.raptorLib.epixStopVideoCapture(c_int(self.id));
                self.device.frame_clock.putData(Float32Array(measuredTimes))
                stopped = True
            else: 
              time.sleep(0.005) # I think it should be a relatively short sleep so we do not miss any frames. --Brian

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
        try:
          raptorWorkers[idx].stopReq = True
        except:
          pass
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

###save temperature worker###  
    def saveTempWorker(self):
      global raptorTempWorkers
      global raptorTempWorkerNids
      try:
        raptorTempWorkers
      except:
        raptorTempWorkerNids = []
        raptorTempWorkers = []
      try:
        idx = raptorTempWorkerNids.index(self.getNid())
        try:
          raptorTempWorkers[idx].stopReq = True
        except:
          pass
        raptorTempWorkers[idx] = self.worker
      except:
        raptorTempWorkerNids.append(self.getNid())
        raptorTempWorkers.append(self.tempWorker)
        return
      return


###restore temperature worker###   
    def restoreTempWorker(self):
      global raptorTempWorkers    
      global raptorempWorkerNids
      try:
        idx = raptorTempWorkerNids.index(self.getNid())
        self.tempWorker = raptorTempWorkers[idx]
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

##Adjust config file
    def genconf(self):
      confPath = self.conf_file.data()
      exposure = self.exp_time.data()
      if exposure < 0     : exposure = 0.0     # must avoid negative numbers
      if exposure > 13000 : exposure = 13000.0 # tested with a (80 MHz) config file specifiying 10s exposures at 0.1Hz, so this is a safe limit at 60MHz
      exp_clks = '%08X' % int(exposure * 60e3)
      byte_str = [exp_clks[0:2], exp_clks[2:4], exp_clks[4:6], exp_clks[6:8]]
      print(byte_str)
      line0 = '    0x124F0450,     0x53060D50,     0x06D402E0,     0xE0530650,     0x5000ED02,     0x02E05306,     0x0650' + byte_str[0] + 'EE,     0xEF02E053,     \n'
      line1 = '    0x530650' + byte_str[1] + ',     0x' + byte_str[2] + 'F002E0,     0xE0530650,     0x50' + byte_str[3] + 'F102,     0x02E05306,     0x06502FDD,     0xDE02E053,     0x530650AF,     \n'

      fh, abs_path = tempfile.mkstemp()
      with open(abs_path, 'w') as adjusted:
        with open(confPath) as original:
          for line in original:
            if   line.startswith('    0x124F04') : adjusted.write(line0)
            elif line.startswith('    0x530650') : adjusted.write(line1)
            else                                 : adjusted.write(line)
      os.close(fh)
      return abs_path

##########init############################################################################    
    def init(self,arg):
      global raptorLib
      global mdsLib
      self.frames.setCompressOnPut(False)	
      self.checkLibraries()
      tmpPath = self.genconf()
      xPixels = c_int(0)
      yPixels = c_int(0)
      raptorLib.epixClose() # as config file is dynamically generated we want to force a re-open
      raptorLib.epixOpen(c_char_p(tmpPath), byref(xPixels), byref(yPixels))
#      os.remove(tmpPath) # should uncomment this line at some point --Brian
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

##########start store############################################################################   
    def start_temp_monitor(self, arg):
      global raptorLib
      global mdsLib
      self.checkLibraries()
      self.tempWorker = self.AsynchTemp()        
      self.tempWorker.daemon = True 
      self.tempWorker.stopReq = False
      self.tempWorker.configure(self, self.id.data(), mdsLib, raptorLib)
      self.saveTempWorker()
      self.tempWorker.start()

      return 1

##########stop store############################################################################   
    def stop_temp_monitor(self,arg):
      self.restoreTempWorker()
      self.tempWorker.stop()
      return 1
