from MDSplus import mdsExceptions, Device, Data, version
from ctypes import CDLL, byref, c_double, c_int, c_void_p, c_char_p, create_string_buffer
from numpy import array
from threading import Thread
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
      {'path':'.OBJECT:ATM_HUM', 'type':'numeric', 'value':0.50},
      {'path':'.OBJECT:ATM_TRANS', 'type':'numeric', 'value':99E-2},

      {'path':'.FRAME', 'type':'structure'},
      {'path':'.FRAME:X', 'type':'numeric', 'value':0},
      {'path':'.FRAME:Y', 'type':'numeric', 'value':0},
      {'path':'.FRAME:WIDTH', 'type':'numeric', 'value':640},
      {'path':'.FRAME:HEIGHT', 'type':'numeric', 'value':480},
      {'path':'.FRAME:TEMP_UNIT', 'type':'text', 'value':'LinearTemperature10mK'},

      {'path':'.CAM_SETUP', 'type':'structure'},
      {'path':'.CAM_SETUP:FOCAL_LENGTH', 'type':'text', 'value':'25'},
      {'path':'.CAM_SETUP:MEAS_RANGE', 'type':'text', 'value':'100...650'},
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
      {'path':':FRAMES_METAD', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':FRAME0_TIME', 'type':'numeric','value':0}]

    parts.append({'path':':INIT_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'startAcquisition',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stopAcquisition',head))",
      'options':('no_write_shot',)})

    handle = c_int(-1)
    handles = {}
    workers = {}
    flirLib = None
    mdsLib = None
    streamLib = None
    flirUtilsLib = None

    error = create_string_buffer(version.tobytes(''), 512)


    """Asynchronous readout internal class"""
    class AsynchStore(Thread):

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
            #raise mdsExceptions.TclFAILED_ESSENTIAL

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
        FLIRSC65X.handles[self.nid] = self.handle

###restore worker###
    def restoreWorker(self):
        if self.nid in FLIRSC65X.workers.keys():
            self.worker = FLIRSC65X.workers[self.nid]
            return 1
        else:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot restore worker!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

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

      return 1

###remove info###
    def removeInfo(self):
      try:
        del(FLIRSC65X.handles[self.nid])
      except:
        print('ERROR TRYING TO REMOVE INFO')


##########init############################################################################
    def init(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL
      self.saveInfo()

      try:
        self.frames.setCompressOnPut(False)
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compresson on put for frames node')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
        self.frames_metad.setCompressOnPut(False)
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compresson on put for frames_metad node')
        raise mdsExceptions.TclFAILED_ESSENTIAL

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
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid value for object atmosfere trasmission')
        raise mdsExceptions.TclFAILED_ESSENTIAL


      status = FLIRSC65X.flirLib.setObjectParameters(self.handle, o_refl_temp, o_atm_temp, o_distance, o_emissivity, o_atm_hum , o_optic_temp, o_optic_trans, o_atm_trans )
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Object Parameters : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Rate
      try:
         frameRate = self.timing_frame_rate.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid frame rate value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = FLIRSC65X.flirLib.setFrameRateNew(self.handle, c_double(frameRate))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Rate : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Area
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

###Focal Length
      try:
         focalLength = self.cam_setup_focal_length.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Focal Length value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if focalLength == '25':
        focalLengthInt=0
      elif focalLength == '41':
        focalLengthInt=3         #offset to select the correct calibration curve using the Measurement Range

###Measurement Range
      try:
         measureRange = self.cam_setup_meas_range.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid measurement range value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if measureRange == '-40...150':
        measRangeInt=0
      elif measureRange == '100...650':
        measRangeInt=1
      elif measureRange == '300...2000':
        measRangeInt=2

      status = FLIRSC65X.flirLib.setMeasurementRange(self.handle, c_int(measRangeInt+focalLengthInt))
      if status < 0:
        try:
          FLIRSC65X.flirLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Measurement Range : ' + self.error.raw)
          raise mdsExceptions.TclFAILED_ESSENTIAL
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

      print("OK " + triggerMode )
      if triggerMode == 'EXTERNAL':   #0=internal  1=external trigger
        trigModeCode=c_int(1)
      else:
        trigSource = array([0.])
        trigModeCode=c_int(0)

      numTrigger = trigSource.size
      print("OK - NUM TRIGGER ", numTrigger)
      print("OK - Trigger Source ", trigSource)


      timeBase = Data.compile(" $ : $ + $ :(zero( size( $ ), 0.) + 1.) * 1./$", trigSource, trigSource, burstDuration, trigSource, frameRate)

      print("Data = " + Data.decompile(timeBase))

      self.timing_time_base.putData(timeBase)
      status = FLIRSC65X.flirLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)

      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Internal/External Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

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
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming mode setup')
        raise mdsExceptions.TclFAILED_ESSENTIAL


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
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming high temperature limit parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             streamingPort = c_int(self.streaming_port.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming port parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             streamingServer = self.streaming_server.data()
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming server parameter value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

#fede 20161012
#      else:
#          autoAdjustLimit = c_int(0)
#          streamingPort = c_int(8888)
#          lowLim = c_int(0)
#          highLim = c_int(36)
#          streamingServer = "localhost"

          print("lowLim ", lowLim)
          print("highLim ", highLim)
          print("frameTempUnitCode ", frameTempUnitCode)
          print("streamingPort ", streamingPort)
          print("streamingServer ", streamingServer)
          deviceName = str(self).rsplit(":",1)
          deviceName = deviceName[1]
          print("Device Name ", deviceName)     
      
#fede: recover device name and pass it to set streaming to overlay text on frame!!!

          status = FLIRSC65X.flirLib.setStreamingMode(self.handle, frameTempUnitCode, streamingEnabled,  autoAdjustLimit, c_char_p(streamingServer), streamingPort,  lowLim,  highLim, c_char_p(deviceName));
          if status < 0:
            FLIRSC65X.flirLib.getLastError(self.handle, self.error)
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute streaming setup mode : ' + self.error.raw)
            raise mdsExceptions.TclFAILED_ESSENTIAL


###Acquisition


      try:
        acqSkipFrameNumber = c_int( self.timing_skip_frame.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid acquisition decimation value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute acquisition setup mode : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
       treePtr = c_void_p(0)
       status = FLIRSC65X.mdsLib.camOpenTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
       if status == -1:
         Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open tree')
         raise mdsExceptions.TclFAILED_ESSENTIAL
      except:
       traceback.print_exc()

      framesNid = self.frames.nid
      timebaseNid = self.timing_time_base.nid
      framesMetadNid = self.frames_metad.nid
      frame0TimeNid = self.frame0_time.nid

      status = FLIRSC65X.flirLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid)
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

      print('Init action completed.')
      print('Fede')
      return 1

####################MANUAL CALIBRATION ACTION
    def calib(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.executeAutoCalib(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Auto Calibration '+ self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      #self.saveInfo()
      return 1


####################MANUAL AUTOFOCUS ACTION
    def autofocus(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.executeAutoFocus(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Auto Focus : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1


####################READ FOCUS POSITION
    def readFocusPos(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      focPos=c_int(0)
      status = FLIRSC65X.flirLib.getFocusAbsPosition(self.handle, byref(focPos))
      print('Focus Position Read: ', focPos)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Focus Position : '+ self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.cam_setup_focus_pos.putData(focPos.value)   #write data in mdsplus

      self.saveInfo()
      return 1


####################WRITE FOCUS POSITION
    def writeFocusPos(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = FLIRSC65X.flirLib.setFocusAbsPosition(self.handle, c_int(self.cam_setup_focus_pos.data()))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Write Focus Position : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1


##########start acquisition############################################################################
    def startAcquisition(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      print("start store")
      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      print("start store2")
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = FLIRSC65X.flirLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Start Camera Acquisition : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
      print("start store3")
      self.worker.configure(self)
      self.saveWorker()
      self.worker.start()
      return 1


##########stop acquisition############################################################################
    def stopAcquisition(self):
      if self.restoreWorker() :
      	self.worker.stop()
      return 1
##########software trigger (start saving in mdsplus)############################################
    def swTrigger(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      print('SOFTWARE TRIGGER')

      status = FLIRSC65X.flirLib.softwareTrigger(self.handle)
      if status < 0:
        FLIRSC65X.flirLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Software Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1
