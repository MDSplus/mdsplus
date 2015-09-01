from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time
import traceback

class FLIRSC65X(Device):
    print 'FLIRSC65X'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """FLIR655 NEW Camera"""
    parts=[
      {'path':':NAME', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},  

      {'path':'.OBJECT', 'type':'structure'},
      {'path':'.OBJECT:EMISSIVITY', 'type':'numeric', 'value':920E-3},
      {'path':'.OBJECT:DISTANCE', 'type':'numeric', 'value':2},
      {'path':'.OBJECT:REFL_TEMP', 'type':'numeric', 'value':20},
      {'path':'.OBJECT:OPTIC_TEMP', 'type':'numeric', 'value':20},   
      {'path':'.OBJECT:OPTIC_TRANS', 'type':'numeric', 'value':1},
      {'path':'.OBJECT:ATM_TEMP', 'type':'numeric', 'value':20},   
      {'path':'.OBJECT:ATM_HUM', 'type':'numeric', 'value':50},     
      {'path':'.OBJECT:ATM_TRANS', 'type':'numeric', 'value':99E-2}, 
     
      {'path':'.FRAME', 'type':'structure'},
      {'path':'.FRAME:X', 'type':'numeric', 'value':0},
      {'path':'.FRAME:Y', 'type':'numeric', 'value':0},
      {'path':'.FRAME:WIDTH', 'type':'numeric', 'value':640},
      {'path':'.FRAME:HEIGHT', 'type':'numeric', 'value':480},
      {'path':'.FRAME:TEMP_UNIT', 'type':'text', 'value':'LinearTemperature10mK'},

      {'path':'.CAM_SETUP', 'type':'structure'},
      {'path':'.CAM_SETUP:FOCAL_LENGTH', 'type':'text', 'value':'50'}, 
      {'path':'.CAM_SETUP:MEAS_RANGE', 'type':'text', 'value':'0...650'},                         
      {'path':'.CAM_SETUP:FOCUS_POS', 'type':'numeric', 'value':0},  
      {'path':'.CAM_SETUP:CALIB_AUTO', 'type':'text', 'value':'NO'},   
      {'path':'.CAM_SETUP:CALIB_TIME', 'type':'numeric', 'value':4},          
          
      {'path':'.TIMING', 'type':'structure'},
      {'path':'.TIMING:TRIG_MODE', 'type':'text', 'value':'INTERNAL'},
      {'path':'.TIMING:TRIG_SOURCE', 'type':'numeric'},
      {'path':'.TIMING:TIME_BASE', 'type':'numeric'},
      {'path':'.TIMING:FRAME_RATE', 'type':'numeric', 'value':50},   
      {'path':'.TIMING:BURST_DUR', 'type':'numeric', 'value':5},    
      {'path':'.TIMING:SKIP_FRAME', 'type':'numeric', 'value':0},
  
      {'path':'.STREAMING', 'type':'structure'},
      {'path':'.STREAMING:MODE', 'type':'text', 'value':'Stream and Store'},
      {'path':'.STREAMING:SERVER', 'type':'text', 'value':'localhost'},
      {'path':'.STREAMING:PORT', 'type':'numeric', 'value':8888},
      {'path':'.STREAMING:AUTOSCALE', 'type':'text', 'value':'YES'},  
      {'path':'.STREAMING:LOLIM', 'type':'numeric', 'value':15},
      {'path':'.STREAMING:HILIM', 'type':'numeric', 'value':50},

  
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':FRAMES_METAD', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]

    parts.append({'path':':INIT_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_store',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_store',head))",
      'options':('no_write_shot',)})
    print 'FLIRSC65X added'
    
    
    handle = c_int(-1)
    error = create_string_buffer('', 512)


    """Asynchronous readout internal class"""       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False

      def configure(self, device, flirLib):
        self.device = device
        self.flirLib = flirLib
        
      def run(self):

        print "Asychronous acquisition thread"  		

        status = self.flirLib.startFramesAcquisition(self.device.handle)
        if status < 0:
          flirLib.getLastError(self.device.handle, self.device.error)
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot start frames acquisition : ' + self.device.error.raw )

        print "Fine acquisition thread"  		

        status = self.flirLib.flirClose(self.device.handle)  #close device and remove from info
        if status < 0:
          flirLib.getLastError(self.device.handle, self.device.error)
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot close camera : ' + self.device.error.raw )

        self.device.removeInfo()
        return 0
      
      def stop(self):
        print "STOP frames acquisition loop"        
        status = self.flirLib.stopFramesAcquisition(self.device.handle)
        if status < 0:
           flirLib.getLastError(self.device.handle, self.device.error)
           Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop frames acquisition : ' + self.device.error.raw )


#end class AsynchStore

###save worker###  
    def saveWorker(self):
      global cameraWorkers
      global cameraWorkerNids
      try:
        cameraWorkers
      except:
        cameraWorkerNids = []
        cameraWorkers = []
      try:
        idx = cameraWorkerNids.index(self.getNid())
        cameraWorkers[idx] = self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        cameraWorkerNids.append(self.getNid())
        cameraWorkers.append(self.worker)
        return
      return


###save Info###   
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices 
#and to avoid opening and closing devices handles 
    def saveInfo(self):
      global cameraHandles
      global cameraNids
      try:
        cameraHandles
      except:
        cameraHandles = []
        cameraNids = []
      try:
        idx = cameraNids.index(self.getNid())
      except:
        print 'SAVE INFO: SAVING HANDLE'
        cameraHandles.append(self.handle)
        cameraNids.append(self.getNid())
        return
      return

###restore worker###   
    def restoreWorker(self):
      global cameraWorkerNids
      global cameraWorkers    
      try:
        idx = cameraWorkerNids.index(self.getNid())
        self.worker = cameraWorkers[idx]
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot restore worker!!')
        return 0
      return 1

###restore info###   
    def restoreInfo(self):
      global cameraHandles
      global cameraNids
      global flirLib
      global streamLib
      global mdsLib
      global flirUtilsLib
    
      print "restore Info"
      try:
        try:
           flirLib
        except:
           libName = "libflirsc65x.so"
           flirLib = CDLL(libName)
           print flirLib
        try:
           mdsLib
        except:
           libName = "libcammdsutils.so"
           mdsLib = CDLL(libName)
           print mdsLib
        try:
           streamLib
        except:
           libName = "libcamstreamutils.so"
           streamLib = CDLL(libName)
           print streamLib
        """
        try:
           flirUtilsLib
        except:
           libName = "libflirutils.so"
           flirUtilsLib = CDLL(libName)
           print flirUtilsLib
        """
      except:
           Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load library : ' + libName )
           return 0
      try:
        idx = cameraNids.index(self.getNid())
        self.handle = cameraHandles[idx]
        print 'RESTORE INFO HANDLE TROVATO - '
      except:
        print 'RESTORE INFO HANDLE NON TROVATO - '
        try: 
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          return 0

        print "Opening"
            
        self.handle = c_int(-1)
        status = flirLib.flirOpen(c_char_p(name), byref(self.handle))

        print "Opened ", status

        if status < 0:
          flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name +'('+self.error.raw+')')
          return 0
                    
      return 1

###remove info###    
    def removeInfo(self):
      global cameraHandles
      global cameraNids
      try:
        cameraNids.remove(self.getNid())
        cameraHandles.remove(self.handle)
      except:
        print 'ERROR TRYING TO REMOVE INFO'
      return


##########init############################################################################    
    def init(self,arg):
      global flirLib
      global mdsLib

      print "OK0"
      
      if self.restoreInfo() == 0:
          return 0      

      print "OK0"

      self.saveInfo()

      print "OK1"

      try:
        self.frames.setCompressOnPut(False)    
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable automatic compresson on put for frames node')
        return 0
      
      try:
        self.frames_metad.setCompressOnPut(False)    
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable automatic compresson on put for frames_metad node')
        return 0

      print "OK2"


###Object Parameters

      try: 
        o_refl_temp = c_double(self.object_refl_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object refletive temperature')
        return 0
      try: 
        o_atm_temp = c_double(self.object_atm_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object atmosfere temperature')
        return 0
      try: 
        o_distance = c_double(self.object_distance.data()) 
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object distance')
        return 0
      try: 
        o_emissivity = c_double(self.object_emissivity.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object emissivity')
        return 0
      try: 
        o_atm_hum = c_double(self.object_atm_hum.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object atmosfere humidity')
        return 0
      try: 
        o_optic_temp = c_double(self.object_optic_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object optic temperature')
        return 0
      try: 
        o_optic_trans = c_double(self.object_optic_trans.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object optic transmission')
        return 0
      try: 
        o_atm_trans = c_double(self.object_atm_trans.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object atmosfere trasmission')
        return 0

      print "OK3"

       
      status = flirLib.setObjectParameters(self.handle, o_refl_temp, o_atm_temp, o_distance, o_emissivity, o_atm_hum , o_optic_temp, o_optic_trans, o_atm_trans )
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Object Parameters : ' + self.error.raw)
        return 0

      print "OK3"

###Frame Rate
      try:
         frameRate = self.timing_frame_rate.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid frame rate value')
        return 0
      status = flirLib.setFrameRateNew(self.handle, c_double(frameRate))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Frame Rate : ' + self.error.raw)
        return 0

###Frame Area
      print "OK"
      x=c_int(0)
      y=c_int(0)
      width=c_int(0)
      height=c_int(0)
      status = flirLib.getReadoutArea(self.handle, byref(x), byref(y), byref(width), byref(height))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Get Readout Area : ' + self.error.raw)
        return 0 

      #write data in mdsplus
      self.frame_x.putData(x.value)
      self.frame_y.putData(y.value)
      self.frame_width.putData(width.value)
      self.frame_height.putData(height.value)

###Measurement Range
      try:
         measureRange = self.cam_setup_meas_range.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid measurement range value')
        return 0      

      if measureRange == '-40...150':
        measRangeInt=c_int(0)
      elif measureRange == '0...650': 
        measRangeInt=c_int(1)
      elif measureRange == '300...2000': 
        measRangeInt=c_int(2)

      status = flirLib.setMeasurementRange(self.handle, measRangeInt)
      if status < 0:
        try:
          flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Measurement Range : ' + self.error.raw)
          return 0
        except:
          traceback.print_exc()

###Image Temperature
      try:
        frameTempUnit = self.frame_temp_unit.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid image temperature unit (Radiometric, 10mk, 100mk) value')
        return 0      

      if frameTempUnit == 'Radiometric':
        frameTempUnitCode=c_int(0)
      elif frameTempUnit == 'LinearTemperature10mK': 
        frameTempUnitCode=c_int(1)
      elif frameTempUnit == 'LinearTemperature100mK': 
        frameTempUnitCode=c_int(2)

      status = flirLib.setIrFormat(self.handle, frameTempUnitCode)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Image Temperature unit : ' + self.error.raw)
        return 0

###Frame Trigger mode
      try:
        burstDuration = self.timing_burst_dur.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid acquisition duration value')
        return 0      

      try:
        triggerMode = self.timing_trig_mode.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid trigger mode value')
        return 0      

      try:
        trigSource = self.timing_trig_source.data()
      except:
        if triggerMode == 'EXTERNAL':
           Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid trigger source value')
           return 0      
        else:
           trigSource = array([0.])
        
      print("OK " + triggerMode )
      if triggerMode == 'EXTERNAL':   #0=internal  1=external trigger
        trigModeCode=c_int(1)
      else:
        trigSource = array([0.])
        trigModeCode=c_int(0) 
 
      numTrigger = trigSource.size
      print "OK - NUM TRIGGER ", numTrigger
      print "OK - Trigger Source ", trigSource


      timeBase = Data.compile(" $ : $ + $ :(zero( size( $ ), 0.) + 1.) * 1./$", trigSource, trigSource, burstDuration, trigSource, frameRate)

      print "Data = " + Data.decompile(timeBase)  
 
      self.timing_time_base.putData(timeBase)          
      status = flirLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)
 
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        return 0

###Calibration
      try:
        calibAuto = self.cam_setup_calib_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid auto calibration setup')
        return 0

      calibModeCode = c_int(1) 	
      if calibAuto == 'NO':        
        try:
           calibTime = self.cam_setup_calib_time.data()
           calibModeCode = c_int(0) 	
        except:
           Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid calibration duration value')
           return 0
        if numTrigger > 1 and (burstDuration + calibTime) > (trigSource[1] - trigSource[0]) :
           Data.execute('DevLogErr($1,$2)', self.getNid(), 'Calibration executed during acquisition')
           return 0
      
      status = flirLib.setCalibMode(self.handle, calibModeCode)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        return 0
        
###Streaming
      try:
        streamingMode = self.streaming_mode.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming mode setup')
        return 0

      
      if streamingMode == 'Stream and Store': 
          streamingEnabled = c_int(1)
          storeEnabled = c_int(1)
      elif streamingMode == 'Only Stream': 
          streamingEnabled = c_int(1)
          storeEnabled = c_int(0)
      else: #streamingMode == 'Only Store': 
          streamingEnabled = c_int(0)
          storeEnabled = c_int(1)


      if streamingEnabled :
          try:
             if self.streaming_autoscale.data() == 'YES' :
                  autoAdjustLimit = c_int(1)
             else:  
                  autoAdjustLimit = c_int(0)
          except:
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming autoscale parameter value')
             return 0

          try:
             lowLim = c_int(self.streaming_lolim.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming low temperature limit parameter value')
             return 0
             
          try:
             highLim = c_int(self.streaming_hilim.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming high temperature limit parameter value')
             return 0  

          try:
             streamingPort = c_int(self.streaming_port.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming port parameter value')
             return 0  

          try:
             streamingServer = self.streaming_server.data()
          except:
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming server parameter value')
             return 0  

      else:
          autoAdjustLimit = c_int(0)
          streamingPort = c_int(8888)
          lowLim = c_int(0)
          highLim = c_int(36)
          streamingServer = "localhost"

      print "lowLim ", lowLim
      print "highLim ", highLim
      print "frameTempUnitCode ", frameTempUnitCode
      print "streamingPort ", streamingPort
      print "streamingServer ", streamingServer

                    
      status = flirLib.setStreamingMode(self.handle, frameTempUnitCode, streamingEnabled,  autoAdjustLimit, c_char_p(streamingServer), streamingPort,  lowLim,  highLim);
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot execute streaming setup mode : ' + self.error.raw)
        return 0


###Acquisition                


      try:
        acqSkipFrameNumber = c_int( self.timing_skip_frame.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid acquisition decimation value')
        return 0
        
      status = flirLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)    
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot execute acquisition setup mode : ' + self.error.raw)
        return 0
        
      try:
       treePtr = c_void_p(0)
       status = mdsLib.camOpenTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
       if status == -1:
         Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open tree')
         return 0
      except:
       traceback.print_exc()
        
      framesNid = self.frames.getNid()
      timebaseNid = self.timing_time_base.getNid()
      framesMetadNid = self.frames_metad.getNid()
       
      status = flirLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot execute set tree info : '+self.error.raw)
        return 0

        
###Auto Calibration
      status = flirLib.executeAutoCalib(self.handle)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Calibration : '+self.error.raw)
        return 0

      print 'Init action completed.'
      return 1

####################MANUAL CALIBRATION ACTION
    def calib(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      status = flirLib.executeAutoCalib(self.handle)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Calibration '+ self.error.raw)
        return 0

      #self.saveInfo()
      return 1


####################MANUAL AUTOFOCUS ACTION
    def autofocus(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      status = flirLib.executeAutoFocus(self.handle)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Focus : ' + self.error.raw)
        return 0

      self.saveInfo()
      return 1

        
####################READ FOCUS POSITION
    def readFocusPos(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      focPos=0
      status = flirLib.getFocusAbsPosition(self.handle, byref(c_int(focPos)))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Focus Position : '+ self.error.raw)
        return 0 
   
      self.focus_pos.putData(focPos)   #write data in mdsplus

      self.saveInfo()
      return 1        
        
        
####################WRITE FOCUS POSITION
    def writeFocusPos(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      status = flirLib.setFocusAbsPosition(self.handle, c_int(self.focus_pos.data()))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Write Focus Position : ' + self.error.raw)
        return 0 

      self.saveInfo()
      return 1
            
            
##########start store############################################################################   
    def start_store(self, arg):
      global flirLib
      global mdsLib
      global streamLib
      global flirUtilsLib

      if self.restoreInfo() == 0:
          return 0      

      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False

      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = flirLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition : '+self.error.raw)
        return 0
      self.worker.configure(self, flirLib)
      self.saveWorker()
      self.worker.start()
      return 1


##########stop store############################################################################   
    def stop_store(self,arg):

      print 'STOP STORE'

      if  self.restoreWorker() :
      	self.worker.stop()
      return 1

