from MDSplus import mdsExceptions, Device, Data, version
from ctypes import CDLL, byref, c_double, c_int, c_void_p, c_char_p, create_string_buffer
from numpy import array
from threading import Thread
import traceback

class BASLERACA(Device):
    """BASLER NEW Camera"""
    parts=[                                                              #offset nid
      {'path':':NAME', 'type':'text', 'value':'192.168.54.96'},          #1
      {'path':':COMMENT', 'type':'text'},                                

      {'path':'.FRAME', 'type':'structure'},                             #3
      {'path':'.FRAME:X', 'type':'numeric', 'value':8},                  
      {'path':'.FRAME:Y', 'type':'numeric', 'value':8},                  
      {'path':'.FRAME:WIDTH', 'type':'numeric', 'value':1920},            
      {'path':'.FRAME:HEIGHT', 'type':'numeric', 'value':1200},           
      {'path':'.FRAME:PIXEL_FORMAT', 'type':'text', 'value':'Mono8'}, 

      {'path':'.CAM_SETUP', 'type':'structure'},                         #9
      {'path':'.CAM_SETUP:GAIN_AUTO', 'type':'text', 'value':'OFF'}, 
      {'path':'.CAM_SETUP:GAIN', 'type':'numeric', 'value':0},        
      {'path':'.CAM_SETUP:GAMMA_EN', 'type':'text', 'value':'OFF'},
      {'path':'.CAM_SETUP:EXP_AUTO', 'type':'text', 'value':'Continuous'}, 
      {'path':'.CAM_SETUP:EXPOSURE', 'type':'numeric', 'value':10000}, 

      {'path':'.TIMING', 'type':'structure'},                             #15
      {'path':'.TIMING:TRIG_MODE', 'type':'text', 'value':'INTERNAL'},    
      {'path':'.TIMING:TRIG_SOURCE', 'type':'numeric'},                   
      {'path':'.TIMING:TIME_BASE', 'type':'numeric'},                     
      {'path':'.TIMING:FRAME_RATE', 'type':'numeric', 'value':5},         
      {'path':'.TIMING:BURST_DUR', 'type':'numeric', 'value':5},          
      {'path':'.TIMING:SKIP_FRAME', 'type':'numeric', 'value':0},         

      {'path':'.STREAMING', 'type':'structure'},                              #22
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
    baslerLib = None
    mdsLib = None
    streamLib = None

    error = create_string_buffer(version.tobytes(''), 512)


    """Asynchronous readout internal class"""
    class AsynchStore(Thread):

        def configure(self, device):
            self.device = device
            self.frameIdx = 0
            self.stopReq = False

        def run(self):

            print("Asychronous acquisition thread")

            status = BASLERACA.baslerLib.startFramesAcquisition(self.device.handle)
            if status < 0:
                BASLERACA.baslerLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot start frames acquisition : ' + self.device.error.raw )

            print("Fine acquisition thread")

            status = BASLERACA.baslerLib.baslerClose(self.device.handle)  #close device and remove from info
            if status < 0:
                BASLERACA.baslerLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot close camera : ' + self.device.error.raw )

            self.device.removeInfo()
            #raise mdsExceptions.TclFAILED_ESSENTIAL

        def stop(self):
            print("STOP frames acquisition loop")
            status = BASLERACA.baslerLib.stopFramesAcquisition(self.device.handle)
            if status < 0:
                BASLERACA.baslerLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot stop frames acquisition : ' + self.device.error.raw )



    def saveWorker(self):
        BASLERACA.workers[self.nid] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
        BASLERACA.handles[self.nid] = self.handle

###restore worker###
    def restoreWorker(self):
        if self.nid in BASLERACA.workers.keys():
            self.worker = BASLERACA.workers[self.nid]
            return 1
        else:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot restore worker!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

###restore info###
    def restoreInfo(self):
      print("restore Info")
      try:
        if BASLERACA.baslerLib is None:
           libName = "libbasleraca.so"
           BASLERACA.baslerLib = CDLL(libName)
           print(BASLERACA.baslerLib)
        if BASLERACA.mdsLib is None:
           libName = "libcammdsutils.so"
           BASLERACA.mdsLib = CDLL(libName)
           print(BASLERACA.mdsLib)
        if BASLERACA.streamLib is None:
           libName = "libcamstreamutils.so"
           BASLERACA.streamLib = CDLL(libName)
           print(BASLERACA.streamLib)

      except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot load library : ' + libName )
           raise mdsExceptions.TclFAILED_ESSENTIAL
      if self.nid in BASLERACA.handles.keys():
        self.handle = BASLERACA.handles[self.nid]
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
        status = BASLERACA.baslerLib.baslerOpen(c_char_p(name), byref(self.handle))

        print("Opened ", status)

        if status < 0:
          BASLERACA.baslerLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open device '+ name +'('+self.error.raw+')')
          raise mdsExceptions.TclFAILED_ESSENTIAL

      return 1

###remove info###
    def removeInfo(self):
      try:
        del(BASLERACA.handles[self.nid])
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

###Gain
      try:
         gain = self.cam_setup_gain.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gain value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setGain(self.handle, c_int(gain))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gain : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Gain Auto
      try:
         gainAuto = self.cam_setup_gain_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gain auto value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setGainAuto(self.handle, c_char_p(gainAuto))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gain Auto : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Gamma Enable
      try:
         gammaEnable = self.cam_setup_gamma_en.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gamma enable value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setGammaEnable(self.handle, c_char_p(gammaEnable))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gamma Enable : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Exposure Auto
      try:
         exposureAuto = self.cam_setup_exp_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid exposure auto value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setExposureAuto(self.handle, c_char_p(exposureAuto))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Exposure Auto : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Exposure
      try:
         exposure = self.cam_setup_exposure.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid exposure value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setExposure(self.handle, c_double(exposure))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Exposure : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###READ CAMERA TEMPERATURE
      status = BASLERACA.baslerLib.readInternalTemperature(self.handle)
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Internal Temperature : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Pixel Format
      try:
         pixelFormat = self.frame_pixel_format.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid pixel format value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setPixelFormat(self.handle, c_char_p(pixelFormat))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Pixel Format : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Rate

      try:
         frameRate = self.timing_frame_rate.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid frame rate value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setFrameRate(self.handle, c_double(frameRate))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Rate : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Area

      try:
         x = self.frame_x.data()
         y = self.frame_y.data()
         width = self.frame_width.data()
         height = self.frame_height.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Frame Area value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = BASLERACA.baslerLib.setReadoutArea(self.handle, c_int(x), c_int(y), c_int(width), c_int(height))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Area : ' + self.error.raw)
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
      status = BASLERACA.baslerLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)

      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
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


          print("lowLim ", lowLim)
          print("highLim ", highLim)
          print("streamingPort ", streamingPort)
          print("streamingServer ", streamingServer)
          deviceName = str(self).rsplit(":",1)
          deviceName = deviceName[1]
          print("Device Name ", deviceName)     
      
#fede: recover device name and pass it to set streaming to overlay text on frame!!!

          status = BASLERACA.baslerLib.setStreamingMode(self.handle, streamingEnabled,  autoAdjustLimit, c_char_p(streamingServer), streamingPort,  lowLim,  highLim, c_char_p(deviceName));
          if status < 0:
            BASLERACA.baslerLib.getLastError(self.handle, self.error)
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute streaming setup mode : ' + self.error.raw)
            raise mdsExceptions.TclFAILED_ESSENTIAL


###Acquisition


      try:
        acqSkipFrameNumber = c_int( self.timing_skip_frame.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid acquisition decimation value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      status = BASLERACA.baslerLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute acquisition setup mode : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
       treePtr = c_void_p(0)
       status = BASLERACA.mdsLib.camOpenTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
       if status == -1:
         Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open tree')
         raise mdsExceptions.TclFAILED_ESSENTIAL
      except:
       traceback.print_exc()

      framesNid = self.frames.nid
      timebaseNid = self.timing_time_base.nid
      framesMetadNid = self.frames_metad.nid
      frame0TimeNid = self.frame0_time.nid

      status = BASLERACA.baslerLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid)
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute set tree info : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      print('Init action completed.')
      return 1



####################CHANGE GAIN
    def changeGain(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = BASLERACA.baslerLib.setGain(self.handle, c_int(self.cam_setup_gain.data()))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot change Gain value : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1


####################CHANGE EXPOSURE
    def changeExposure(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = BASLERACA.baslerLib.setExposure(self.handle, c_double(self.cam_setup_exposure.data()))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot change Exposure Value : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1


####################READ INTERNAL TEMPERATURE
    def readTemperature(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      status = BASLERACA.baslerLib.readInternalTemperature(self.handle)
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Internal Temperature : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1


##########start acquisition############################################################################
    def startAcquisition(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      print("Starting Acquisition...")
      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = BASLERACA.baslerLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Start Camera Acquisition : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
      print("OK!")
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

      status = BASLERACA.baslerLib.softwareTrigger(self.handle)
      if status < 0:
        BASLERACA.baslerLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Software Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return 1
