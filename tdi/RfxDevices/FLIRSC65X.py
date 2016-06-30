<<<<<<< HEAD
from MDSplus import mdsExceptions, Device, Data, version
from ctypes import CDLL, byref, c_double, c_int, c_void_p, c_char_p, create_string_buffer
from numpy import array
from threading import Thread
=======
from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
import traceback

class FLIRSC65X(Device):
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
<<<<<<< HEAD


=======
  
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':FRAMES_METAD', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':'.TIMING:FRAME0INT64', 'type':'numeric', 'value':0}]

    parts.append({'path':':INIT_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_store',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_store',head))",
      'options':('no_write_shot',)})
<<<<<<< HEAD

    handle = c_int(-1)
    handles = {}
    workers = {}
    flirLib = None
    mdsLib = None
    streamLib = None
    flirUtilsLib = None
=======
    print 'FLIRSC65X added'
    
    
    handle = c_int(-1)
    error = create_string_buffer('', 512)
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

    error = create_string_buffer(version.tobytes(''), 512)


    """Asynchronous readout internal class"""
    class AsynchStore(Thread):
<<<<<<< HEAD

        def configure(self, device):
            self.device = device
            self.frameIdx = 0
            self.stopReq = False

        def run(self):

            print("Asychronous acquisition thread")

            status = FLIRSC65X.flirLib.startFramesAcquisition(self.device.handle)
            if status < 0:
                FLIRSC65X.flirLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot start frames acquisition : ' + self.device.error.raw )

            print("Fine acquisition thread")

            status = FLIRSC65X.flirLib.flirClose(self.device.handle)  #close device and remove from info
            if status < 0:
                FLIRSC65X.flirLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot close camera : ' + self.device.error.raw )

            self.device.removeInfo()
            raise mdsExceptions.TclFAILED_ESSENTIAL

        def stop(self):
            print("STOP frames acquisition loop")
            status = FLIRSC65X.flirLib.stopFramesAcquisition(self.device.handle)
            if status < 0:
                FLIRSC65X.flirLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot stop frames acquisition : ' + self.device.error.raw )



    def saveWorker(self):
        FLIRSC65X.workers[self.nid] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
        FLIRSC65X.handels[self.nid] = self.handel

###restore worker###
    def restoreWorker(self):
        if self.nid in FLIRSC65X.workers.keys():
            self.worker = FLIRSC65X.workers[self.nid]
        else:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot restore worker!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return

###restore info###
    def restoreInfo(self):
      print("restore Info")
      try:
        if FLIRSC65X.flirLib is None:
           libName = "libflirsc65x.so"
           FLIRSC65X.flirLib = CDLL(libName)
           print(FLIRSC65X.flirLib)
        if FLIRSC65X.mdsLib is None:
           libName = "libcammdsutils.so"
           FLIRSC65X.mdsLib = CDLL(libName)
           print(FLIRSC65X.mdsLib)
        if FLIRSC65X.streamLib is None:
           libName = "libcamstreamutils.so"
           FLIRSC65X.streamLib = CDLL(libName)
           print(FLIRSC65X.streamLib)
        """
        if FLIRSC65X.flirUtilsLib is None:
           libName = "libflirutils.so"
           FLIRSC65X.flirUtilsLib = CDLL(libName)
           print(FLIRSC65X.flirUtilsLib)
        """
      except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot load library : ' + libName )
           raise mdsExceptions.TclFAILED_ESSENTIAL
      if self.nid in FLIRSC65X.handles.keys():
        self.handle = FLIRSC65X.handles[self.nid]
        print('RESTORE INFO HANDLE TROVATO')
      else:
        print('RESTORE INFO HANDLE NON TROVATO')
        try:
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.nid, 'Missing device name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        print("Opening")

        self.handle = c_int(-1)
        status = FLIRSC65X.flirLib.flirOpen(c_char_p(name), byref(self.handle))

        print("Opened ", status)

        if status < 0:
          FLIRSC65X.flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open device '+ name +'('+self.error.raw+')')
          raise mdsExceptions.TclFAILED_ESSENTIAL

      return

###remove info###
    def removeInfo(self):
=======
      frameIdx = 0
      stopReq = False

      def configure(self, device, flirLib):
        self.device = device
        self.flirLib = flirLib
        
      def run(self):

        print self.device.getNodeName()," - Asynchronous acquisition thread started"  		

        status = self.flirLib.startFramesAcquisition(self.device.handle)    #acquisition loop 
        if status < 0:
          flirLib.getLastError(self.device.handle, self.device.error)
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot start frames acquisition : ' + self.device.error.raw )
        
        print self.device.getNodeName()," - Asynchronous acquisition thread ended"	
        return
      
      def stop(self):
        if self.isAlive():  #worker is running
          status = self.flirLib.stopFramesAcquisition(self.device.handle)
          if status < 0:
             flirLib.getLastError(self.device.handle, self.device.error)
             Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop frames acquisition : ' + self.device.error.raw )
        else:
          print 'Worker is NOT alive. Stop not necessary.'  
        return
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
        print 'Save Worker: New Worker saved'
        cameraWorkerNids.append(self.getNid())
        cameraWorkers.append(self.worker)
        return
      return

###restore worker###   
    def restoreWorker(self):
      global cameraWorkerNids
      global cameraWorkers    
      try:
        idx = cameraWorkerNids.index(self.getNid())
        self.worker = cameraWorkers[idx]
        print 'Restore worker: worker recovered'
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Restore worker: WORKER NOT PRESENT')
        return 0
      return 1


###remove worker###   
    def removeWorker(self):
      global cameraWorkerNids
      global cameraWorkers    
      
      print 'Remove worker'
      try:
        cameraWorkerNids.remove(self.getNid())
        cameraWorkers.remove(self.worker)
        del self.worker
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot remove worker!!')
        return 0
         
      return 1

    
    
###create Info###   
#allow to manage multiple occurrences of camera devices and to avoid opening and closing devices handles 
    def createInfo(self):
      global cameraHandles
      global cameraNids
      global flirLib
      global streamLib
      global mdsLib 

      try:
        cameraHandles
      except:
        cameraHandles = []
        cameraNids = []
      
      print self.getNodeName(),' - Create Info: creating new handle'
                  
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
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: CANNOT LOAD LIBRARY ' + libName )
          return 0
                
      print "Opening Flir Camera..."
            
      try: 
          name = self.name.data()
      except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: MISSING DEVICE NAME' )
          return 0
                                        
      self.handle = c_int(-1)
      status = flirLib.flirOpen(c_char_p(name), byref(self.handle))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: Cannot open FLIR camera '+ name +'('+self.error.raw+')')
        return 0
                
      cameraHandles.append(self.handle)
      cameraNids.append(self.getNid())
      print self.getNodeName()," - Camera opened and handle saved."              
      return 1         
      
      
###restore info###   
    def restoreInfo(self):
      global cameraHandles
      global cameraNids
            
      try:
        idx = cameraNids.index(self.getNid())     
        self.handle = cameraHandles[idx]
        print self.getNodeName(),' - Restore Info: handle recovered'
      except:
        print self.getNodeName(),' - Restore Info: HANDLE NOT PRESENT'
        return 0
                 
      return 1



###remove info###    
    def removeInfo(self):
      global cameraHandles
      global cameraNids
      global flirLib
      
      print 'Close camera and remove info'
      status = flirLib.flirClose(self.handle)  #CLOSE FLIR CAMERA     --- self.device.handle
      if status < 0:
         flirLib.getLastError(self.handle, self.error)   #self.device.
         Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR CLOSING THE CAMERA: ' + self.error.raw )   #self.handle

>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      try:
        del(FLIRSC65X.handles[self.nid])
      except:
<<<<<<< HEAD
        print('ERROR TRYING TO REMOVE INFO')


##########init############################################################################
    def init(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
=======
        print 'ERROR TRYING TO REMOVE INFO'
        return 0
      
      return 1      



##########init############################################################################    
    def init(self,arg):
        
      if self.restoreWorker() == 1:    #TRY TO RESTORE WORKER.
          if self.worker.isAlive():
             print 'ERROR: CANNOT DO INIT DURING ACQUISITION'
             return 0                 #MUST NOT CLOSE CAMERA
          else:
             self.removeWorker()      #WORKER EXIST FROM THE LAST ACQUISITION 
             print 'remove last acquisition worker'
                        
       
      if self.restoreInfo() == 0:    #TRY TO RESTORE HANDLE.  
        if self.createInfo() == 0:   #IF NOT EXISTS TRY TO CREATE NEW HANDLE
          return 0                   #ON ERROR EXIT FROM INIT
                                 
      if self.initCamera(arg)==0:    #ON INIT ERROR CLOSE CAMERA. 
         self.removeInfo()
         return 0
      
      print self.getNodeName(), ' - Init action completed.'
      return 1        
        
        
        
        
##########initCamera###########      
    def initCamera(self,arg):
      global flirLib
      global mdsLib
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

      try:
        self.frames.setCompressOnPut(False)
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compresson on put for frames node')
        raise mdsExceptions.TclFAILED_ESSENTIAL

=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable automatic compression on put for frames node')
        return 0
      
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      try:
        self.frames_metad.setCompressOnPut(False)
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compresson on put for frames_metad node')
        raise mdsExceptions.TclFAILED_ESSENTIAL
=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable automatic compression on put for frames_metad node')
        return 0

>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

###Object Parameters

      try:
        o_refl_temp = c_double(self.object_refl_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object refletive temperature')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_atm_temp = c_double(self.object_atm_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object atmosfere temperature')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_distance = c_double(self.object_distance.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object distance')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_emissivity = c_double(self.object_emissivity.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object emissivity')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_atm_hum = c_double(self.object_atm_hum.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object atmosfere humidity')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_optic_temp = c_double(self.object_optic_temp.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object optic temperature')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_optic_trans = c_double(self.object_optic_trans.data())
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object optic transmission')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        o_atm_trans = c_double(self.object_atm_trans.data())
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object atmosfere trasmission')
        raise mdsExceptions.TclFAILED_ESSENTIAL


      status = FLIRSC65X.flirLib.setObjectParameters(self.handle, o_refl_temp, o_atm_temp, o_distance, o_emissivity, o_atm_hum , o_optic_temp, o_optic_trans, o_atm_trans )
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Object Parameters : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid value for object atmosfere trasmission')
        return 0
       
       
      status = flirLib.setObjectParameters(self.handle, o_refl_temp, o_atm_temp, o_distance, o_emissivity, o_atm_hum , o_optic_temp, o_optic_trans, o_atm_trans )
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Object Parameters : ' + self.error.raw)
        return 0
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

###Frame Rate
      try:
         frameRate = self.timing_frame_rate.data()
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid frame rate value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = FLIRSC65X.flirLib.setFrameRateNew(self.handle, c_double(frameRate))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Rate : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Area
=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid frame rate value')
        return 0
      status = flirLib.setFrameRateNew(self.handle, c_double(frameRate))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Frame Rate : ' + self.error.raw)
        return 0

###Frame Area

>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      x=c_int(0)
      y=c_int(0)
      width=c_int(0)
      height=c_int(0)
      status = FLIRSC65X.flirLib.getReadoutArea(self.handle, byref(x), byref(y), byref(width), byref(height))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Get Readout Area : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      #write data in mdsplus
      self.frame_x.putData(x.value)
      self.frame_y.putData(y.value)
      self.frame_width.putData(width.value)
      self.frame_height.putData(height.value)

###Measurement Range
      try:
         measureRange = self.cam_setup_meas_range.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid measurement range value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if measureRange == '-40...150':
        measRangeInt=c_int(0)
      elif measureRange == '0...650':
        measRangeInt=c_int(1)
      elif measureRange == '300...2000':
        measRangeInt=c_int(2)

<<<<<<< HEAD
      status = FLIRSC65X.flirLib.setMeasurementRange(self.handle, measRangeInt)
      if status < 0:
        try:
          FLIRSC65X.flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Measurement Range : ' + self.error.raw)
          raise mdsExceptions.TclFAILED_ESSENTIAL
=======
      status = flirLib.setMeasurementRange(self.handle, measRangeInt)
      if status < 0:
        try:
          flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Measurement Range : ' + self.error.raw)
          return 0
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
        except:
          traceback.print_exc()

###Image Temperature
      try:
        frameTempUnit = self.frame_temp_unit.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid image temperature unit (Radiometric, 10mk, 100mk) value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if frameTempUnit == 'Radiometric':
        frameTempUnitCode=c_int(0)
      elif frameTempUnit == 'LinearTemperature10mK':
        frameTempUnitCode=c_int(1)
      elif frameTempUnit == 'LinearTemperature100mK':
        frameTempUnitCode=c_int(2)

      status = FLIRSC65X.flirLib.setIrFormat(self.handle, frameTempUnitCode)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Image Temperature unit : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Trigger mode
      try:
        burstDuration = self.timing_burst_dur.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid acquisition duration value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
        triggerMode = self.timing_trig_mode.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid trigger mode value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
        trigSource = self.timing_trig_source.data()
      except:
        if triggerMode == 'EXTERNAL':
           Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid trigger source value')
           raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
           trigSource = array([0.])
<<<<<<< HEAD

=======
        
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      print("OK " + triggerMode )
      if triggerMode == 'EXTERNAL':   #0=internal  1=external trigger
        trigModeCode=c_int(1)
      else:
        trigSource = array([0.])
<<<<<<< HEAD
        trigModeCode=c_int(0)

      numTrigger = trigSource.size
      print("OK - NUM TRIGGER ", numTrigger)
      print("OK - Trigger Source ", trigSource)
=======
        trigModeCode=c_int(0) 
 
      numTrigger = trigSource.size
      print "OK - NUM TRIGGER ", numTrigger
      print "OK - Trigger Source ", trigSource
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218


      timeBase = Data.compile(" $ : $ + $ :(zero( size( $ ), 0.) + 1.) * 1./$", trigSource, trigSource, burstDuration, trigSource, frameRate)

<<<<<<< HEAD
      print("Data = " + Data.decompile(timeBase))

      self.timing_time_base.putData(timeBase)
      status = FLIRSC65X.flirLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)

      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
=======
      print "Data = " + Data.decompile(timeBase)  
 
      self.timing_time_base.putData(timeBase)          
      status = flirLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)
 
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        return 0
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

###Calibration
      try:
        calibAuto = self.cam_setup_calib_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid auto calibration setup')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      calibModeCode = c_int(1)
      if calibAuto == 'NO':
        try:
           calibTime = self.cam_setup_calib_time.data()
           calibModeCode = c_int(0)
        except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid calibration duration value')
           raise mdsExceptions.TclFAILED_ESSENTIAL
        if numTrigger > 1 and (burstDuration + calibTime) > (trigSource[1] - trigSource[0]) :
           Data.execute('DevLogErr($1,$2)', self.nid, 'Calibration executed during acquisition')
           raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.setCalibMode(self.handle, calibModeCode)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Streaming
      try:
        streamingMode = self.streaming_mode.data()
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming mode setup')
        raise mdsExceptions.TclFAILED_ESSENTIAL


      if streamingMode == 'Stream and Store':
=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming mode setup')
        return 0

      
      if streamingMode == 'Stream and Store': 
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
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
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming autoscale parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             lowLim = c_int(self.streaming_lolim.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming low temperature limit parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             highLim = c_int(self.streaming_hilim.data())
          except:
<<<<<<< HEAD
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming high temperature limit parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL
=======
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming high temperature limit parameter value')
             return 0  
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

          try:
             streamingPort = c_int(self.streaming_port.data())
          except:
<<<<<<< HEAD
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming port parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL
=======
             Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid streaming port parameter value')
             return 0  
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

          try:
             streamingServer = self.streaming_server.data()
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming server parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

      else:
          autoAdjustLimit = c_int(0)
          streamingPort = c_int(8888)
          lowLim = c_int(0)
          highLim = c_int(36)
          streamingServer = "localhost"

<<<<<<< HEAD
      print("lowLim ", lowLim)
      print("highLim ", highLim)
      print("frameTempUnitCode ", frameTempUnitCode)
      print("streamingPort ", streamingPort)
      print("streamingServer ", streamingServer)


      status = FLIRSC65X.flirLib.setStreamingMode(self.handle, frameTempUnitCode, streamingEnabled,  autoAdjustLimit, c_char_p(streamingServer), streamingPort,  lowLim,  highLim);
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute streaming setup mode : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Acquisition
=======
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
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218


      try:
        acqSkipFrameNumber = c_int( self.timing_skip_frame.data() )
      except:
<<<<<<< HEAD
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid acquisition decimation value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)
=======
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid acquisition decimation value')
        return 0
        
      status = flirLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)    
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute acquisition setup mode : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
       treePtr = c_void_p(0)
       status = FLIRSC65X.mdsLib.camOpenTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
       if status == -1:
<<<<<<< HEAD
         Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open tree')
         raise mdsExceptions.TclFAILED_ESSENTIAL
      except:
       traceback.print_exc()

      framesNid = self.frames.nid
      timebaseNid = self.timing_time_base.nid
      framesMetadNid = self.frames_metad.nid

      status = FLIRSC65X.flirLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid)
=======
         Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open tree')
         return 0
      except:
       traceback.print_exc()
        
      framesNid = self.frames.getNid()
      timebaseNid = self.timing_time_base.getNid()
      framesMetadNid = self.frames_metad.getNid()
       
      status = flirLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid)
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute set tree info : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Auto Calibration
      status = FLIRSC65X.flirLib.executeAutoCalib(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Auto Calibration : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

<<<<<<< HEAD
      print('Init action completed.')
      return
=======
      return 1
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218





####################MANUAL CALIBRATION ACTION
<<<<<<< HEAD
    def calib(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.executeAutoCalib(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Auto Calibration '+ self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      #self.saveInfo()
      return


####################MANUAL AUTOFOCUS ACTION
    def autofocus(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL
=======
    def calib(self,arg):
      global flirLib
      
      if self.restoreInfo() == 0:    #TRY TO RESTORE HANDLE.        
        if self.createInfo() == 0:   #IF NOT EXISTS TRY TO CREATE NEW HANDLE
          return 0                   #ON ERROR EXIT 
      else: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: device already in use')
        return 0                           
                  
      status = flirLib.executeAutoCalib(self.handle)
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Calibration '+ self.error.raw)
        return 0
      
      self.removeInfo()
      return 1


####################MANUAL AUTOFOCUS ACTION
    def autofocus(self,arg):
      global flirLib

      if self.restoreInfo() == 0:    #TRY TO RESTORE HANDLE.        
        if self.createInfo() == 0:   #IF NOT EXISTS TRY TO CREATE NEW HANDLE
          return 0                   #ON ERROR EXIT 
      else: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: device already in use')
        return 0   
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

      status = FLIRSC65X.flirLib.executeAutoFocus(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Auto Focus : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

<<<<<<< HEAD
      self.saveInfo()
      return


####################READ FOCUS POSITION
    def readFocusPos(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      focPos=0
      status = FLIRSC65X.flirLib.getFocusAbsPosition(self.handle, byref(c_int(focPos)))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Focus Position : '+ self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.focus_pos.putData(focPos)   #write data in mdsplus

      self.saveInfo()
      return


####################WRITE FOCUS POSITION
    def writeFocusPos(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.setFocusAbsPosition(self.handle, c_int(self.focus_pos.data()))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Write Focus Position : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return


##########start store############################################################################
    def start_store(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
=======
      self.removeInfo()   
      return 1

        
####################READ FOCUS POSITION
    def rd_foc_pos(self,arg):    
      global flirLib

      if self.restoreInfo() == 0:    #TRY TO RESTORE HANDLE.        
        if self.createInfo() == 0:   #IF NOT EXISTS TRY TO CREATE NEW HANDLE
          return 0                   #ON ERROR EXIT 
      else: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: device already in use')
        return 0  
      
      focPos=0
      status = flirLib.getFocusAbsPosition(self.handle, byref(c_int(focPos)))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Focus Position : '+ self.error.raw)
        return 0 
      
      print "Readed Focus Position: ", focPos 
      self.cam_setup_focus_pos.putData(focPos)   #write data in mdsplus

      self.removeInfo()   
      return 1        
        
        
####################WRITE FOCUS POSITION
    def wr_foc_pos(self,arg):
      global flirLib

      if self.restoreInfo() == 0:    #TRY TO RESTORE HANDLE.        
        if self.createInfo() == 0:   #IF NOT EXISTS TRY TO CREATE NEW HANDLE
          return 0                   #ON ERROR EXIT 
      else: 
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: device already in use')
        return 0  

      focPos=self.cam_setup_focus_pos.data()
      status = flirLib.setFocusAbsPosition(self.handle, c_int(focPos))
      if status < 0:
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Write Focus Position : ' + self.error.raw)
        return 0 
      
      print "Writed Focus Position: ", focPos 
      self.removeInfo()
      return 1
            
            
##########start store############################################################################   
    def start_store(self, arg):
      global flirLib
      global mdsLib
      global streamLib

      print 'Start FLIR Store'

      if self.restoreInfo() == 0:    #EXIT if NO HANDLE exists.
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'ERROR: Init action MUST be perform before start_store')
          return 0
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

      if self.restoreWorker() == 1:    #TRY TO RESTORE WORKER.
          if self.worker.isAlive():
             print 'ERROR: CANNOT DO START STORE DURING ACQUISITION'
             return 0                 #MUST NOT CLOSE CAMERA
          else:
             print 'Remove last acquisition Worker'
             self.removeWorker()      #WORKER EXIST FROM THE LAST ACQUISITION 
                  
      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      self.worker.configure(self, flirLib)
      self.saveWorker()        
                
      timestamp=datetime.datetime.now()
      sampleTime=int(time.mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)  #ms   
      self.timing_frame0int64.putData(Int64(sampleTime))    
             
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = FLIRSC65X.flirLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
<<<<<<< HEAD
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Start Camera Acquisition : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.worker.configure(self)
      self.saveWorker()
=======
        flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition : '+self.error.raw)
        return 0
      
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218
      self.worker.start()
      return


<<<<<<< HEAD
##########stop store############################################################################
    def stop_store(self):

      print('STOP STORE')

      if  self.restoreWorker() :
      	self.worker.stop()
      return
=======
##########stop store############################################################################   
    def stop_store(self,arg):

      print 'STOP STORE'
      try: 
                             
        if  self.restoreWorker():
          self.worker.stop()
          self.removeWorker()
        
        if self.restoreInfo():  
          self.removeInfo()
          self.handle = c_int(-1)                            

      except:
          print 'ERROR IN STOP STORE'
              
      return 1
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

