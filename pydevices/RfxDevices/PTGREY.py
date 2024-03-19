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

from MDSplus import mdsExceptions, Device, Data, version, Float64
from ctypes import CDLL, byref, c_double, c_int, c_float, c_void_p, c_char_p, create_string_buffer
from numpy import array
from threading import Thread
import traceback
import time

class PTGREY(Device):
    """PTGREY NEW Camera"""
    parts=[								#offset nid
      {'path':':IP_NAME', 'type':'text', 'value':'192.168.100.18'},	#1
      {'path':':COMMENT', 'type':'text'},

      {'path':'.FRAME', 'type':'structure'},				#3
      {'path':'.FRAME:X', 'type':'numeric', 'value':0},
      {'path':'.FRAME:Y', 'type':'numeric', 'value':0},
      {'path':'.FRAME:WIDTH', 'type':'numeric', 'value':1936},
      {'path':'.FRAME:HEIGHT', 'type':'numeric', 'value':1464},
      {'path':'.FRAME:PIXEL_FORMAT', 'type':'text', 'value':'Mono16'}, 

      {'path':'.CAM_SETUP', 'type':'structure'},				#9
      {'path':'.CAM_SETUP:GAIN_AUTO', 'type':'text', 'value':'Off'}, 
      {'path':'.CAM_SETUP:GAIN', 'type':'numeric', 'value':0},        
      {'path':'.CAM_SETUP:GAMMA_EN', 'type':'text', 'value':'Off'},
      {'path':'.CAM_SETUP:EXP_AUTO', 'type':'text', 'value':'Off'}, 
      {'path':'.CAM_SETUP:EXPOSURE', 'type':'numeric', 'value':800E3},

      {'path':'.TIMING', 'type':'structure'},					#15
      {'path':'.TIMING:TRIG_MODE', 'type':'text', 'value':'INTERNAL'},    
      {'path':'.TIMING:TRIG_SOURCE', 'type':'numeric'},                   
      {'path':'.TIMING:TIME_BASE', 'type':'numeric'},                     
      {'path':'.TIMING:FRAME_RATE', 'type':'numeric', 'value':1},         
      {'path':'.TIMING:BURST_DUR', 'type':'numeric', 'value':5},          
      {'path':'.TIMING:SKIP_FRAME', 'type':'numeric', 'value':0},   

      {'path':'.STREAMING', 'type':'structure'},				#22
      {'path':'.STREAMING:MODE', 'type':'text', 'value':'Stream and Store'},
      {'path':'.STREAMING:SERVER', 'type':'text', 'value':'localhost'},
      {'path':'.STREAMING:PORT', 'type':'numeric', 'value':8888},
      {'path':'.STREAMING:AUTOSCALE', 'type':'text', 'value':'NO'},
      {'path':'.STREAMING:LOLIM', 'type':'numeric', 'value':0},
      {'path':'.STREAMING:HILIM', 'type':'numeric', 'value':4095},
      {'path':'.STREAMING:ADJROIX', 'type':'numeric', 'value':0},
      {'path':'.STREAMING:ADJROIY', 'type':'numeric', 'value':0},
      {'path':'.STREAMING:ADJROIW', 'type':'numeric', 'value':1936},
      {'path':'.STREAMING:ADJROIH', 'type':'numeric', 'value':1464},

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
    ptgreyLib = None
    mdsLib = None
    streamLib = None

    error = create_string_buffer(version.tobytes(''), 512)

    def debugPrint(self, msg="", obj=""):
          print( "------ DEBUG  " + self.name + ":" + msg, obj );


    """Asynchronous readout internal class"""
    class AsynchStore(Thread):

        def configure(self, device):
            self.device = device
            self.frameIdx = 0
            self.stopReq = False

        def run(self):

            self.device.debugPrint("Asychronous acquisition thread")

            status = PTGREY.ptgreyLib.startFramesAcquisition(self.device.handle)
            if status < 0:
                PTGREY.ptgreyLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot start frames acquisition : ' + self.device.error.raw )

            self.device.debugPrint("Acquisition thread ended")
            self.device.removeInfo()


        def stop(self):
            self.device.debugPrint("STOP frames acquisition loop")
            status = PTGREY.ptgreyLib.stopFramesAcquisition(self.device.handle)
            if status < 0:
                PTGREY.ptgreyLib.getLastError(self.device.handle, self.device.error)
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot stop frames acquisition : ' + self.device.error.raw )



    def saveWorker(self):
        PTGREY.workers[self.nid] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
        PTGREY.handles[self.nid] = self.handle

###restore worker###
    def restoreWorker(self):
        if self.nid in PTGREY.workers.keys():
            self.worker = PTGREY.workers[self.nid]
            return 1
        else:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot restore worker!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL

###check worker###
    def checkWorker(self):                      #FM 20191114
      if self.nid in PTGREY.workers.keys():
        self.debugPrint('CHECK WORKER: found')
        return 1
      else:
        self.debugPrint('CHECK WORKER: NOT found')
        return 0

###check info###
    def checkInfo(self):                        #FM 20191114
      if self.nid in PTGREY.handles.keys():
        self.debugPrint('CHECK INFO: found')
        return 1
      else:
        self.debugPrint('CHECK INFO: NOT found')
        return 0


###restore info###   (returned value 0:error 1:camera opened 2:camera was already opened)
    def restoreInfo(self):     
      self.debugPrint("restore Info")
      try:
        if PTGREY.ptgreyLib is None:
           libName = "libptgrey.so"
           PTGREY.ptgreyLib = CDLL(libName)
           self.debugPrint(obj=PTGREY.ptgreyLib)
        if PTGREY.mdsLib is None:
           libName = "libcammdsutils.so"
           PTGREY.mdsLib = CDLL(libName)
           self.debugPrint(obj=PTGREY.mdsLib)
        if PTGREY.streamLib is None:
           libName = "libcamstreamutils.so"
           PTGREY.streamLib = CDLL(libName)
           self.debugPrint(obj=PTGREY.streamLib)
      except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot load library : ' + libName )
           raise mdsExceptions.TclFAILED_ESSENTIAL

      if self.checkInfo():
        self.handle = PTGREY.handles[self.nid]
        return 2
      else:
        try:
          name = self.ip_name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.nid, 'Missing device name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint("Opening Camera")
        self.handle = c_int(-1)
        status = PTGREY.ptgreyLib.PGopen(c_char_p(name), byref(self.handle))
        if status < 0:
          PTGREY.ptgreyLib.getLastError(self.handle, self.error)
          Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open device '+ name +' ('+self.error.raw+')')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        self.debugPrint("Connected ", status)

        self.saveInfo()

      return 1

###remove info###
    def removeInfo(self):
      try:  
        status = PTGREY.ptgreyLib.PGclose(self.handle)  #close camera 
        if status < 0:
            PTGREY.ptgreyLib.getLastError(self.handle, self.error)
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot close camera : ' + self.error.raw )
      except:
        self.debugPrint('ERROR in ptgreyLib.PGclose()')

      try:
        del(PTGREY.handles[self.nid])
        self.debugPrint('Camera Info removed.')
      except:
        self.debugPrint('ERROR trying to remove info')


###remove worker###
    def removeWorker(self):
      try:
        del(PTGREY.workers[self.nid])
        self.debugPrint('Camera Worker removed.')
      except:
        self.debugPrint('ERROR trying to remove Worker')



##########init############################################################################
    def init(self):
      if self.checkWorker():    #FM 20191114
        self.debugPrint('Cannot dispatch init during acquisition/storing.')
        return 1 
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
        self.frames.setCompressOnPut(False)
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compress on put for frames node')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
        self.frames_metad.setCompressOnPut(False)
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot disable automatic compress on put for frames_metad node')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Gain Auto
      try:
         gainAuto = self.cam_setup_gain_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gain auto value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setGainAuto(self.handle, c_char_p(gainAuto))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gain Auto : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      time.sleep(0.1)

###Gain
      try:
         gain = self.cam_setup_gain.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gain value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setGain(self.handle, c_float(gain))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gain : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Gamma Enable
      try:
         gammaEnable = self.cam_setup_gamma_en.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid gamma enable value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setGammaEnable(self.handle, c_char_p(gammaEnable))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Gamma Enable : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Exposure Auto
      try:
         exposureAuto = self.cam_setup_exp_auto.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid exposure auto value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setExposureAuto(self.handle, c_char_p(exposureAuto))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Exposure Auto : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Exposure 
      try:
         exposure = self.cam_setup_exposure.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid exposure value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setExposure(self.handle, c_float(exposure))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Exposure : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###READ CAMERA TEMPERATURE
      status = PTGREY.ptgreyLib.readInternalTemperature(self.handle)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Internal Temperature : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Pixel Format
      try:
         pixelFormat = self.frame_pixel_format.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid pixel format value')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      status = PTGREY.ptgreyLib.setPixelFormat(self.handle, c_char_p(pixelFormat))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Pixel Format : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL


###Frame Rate
      try:
         fRate = self.timing_frame_rate.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid frame rate value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      frameRate = c_double(fRate)
      status = PTGREY.ptgreyLib.setFrameRate(self.handle, byref(frameRate) )
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Rate : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.timing_frame_rate.putData(Float64(frameRate))

      self.debugPrint('fede4')


###Frame Area
      try:
         x = self.frame_x.data()
         y = self.frame_y.data()
         width = self.frame_width.data()
         height = self.frame_height.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Frame Area value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

#      status = PTGREY.ptgreyLib.getReadoutArea(self.handle, byref(x), byref(y), byref(width), byref(height))
#      if status < 0:
#        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
#        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Readout Area : ' + self.error.raw)
#        raise mdsExceptions.TclFAILED_ESSENTIAL

      status = PTGREY.ptgreyLib.setReadoutArea(self.handle, c_int(x), c_int(y), c_int(width), c_int(height))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Set Frame Area : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL



###Focal Length
#      try:
#         focalLength = self.cam_setup_focal_length.data()
#      except:
#        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Focal Length value')
#        raise mdsExceptions.TclFAILED_ESSENTIAL

 
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

      self.debugPrint("OK " + triggerMode )
      if triggerMode == 'EXTERNAL':   #0=internal  1=external trigger
        trigModeCode=c_int(1)
      else:
        trigSource = array([0.])    
        trigModeCode=c_int(0)

      numTrigger = trigSource.size
      self.debugPrint("Trigger Num.: ", numTrigger)
      self.debugPrint("Trigger Source: ", trigSource)


      timeBase = Data.compile(" $ : $ + $ :(zero( size( $ ), 0.) + 1.) * 1./$", trigSource, trigSource, burstDuration, trigSource, frameRate)

      self.debugPrint("Timebase Data = " + Data.decompile(timeBase))

      self.timing_time_base.putData(timeBase)
      status = PTGREY.ptgreyLib.setTriggerMode(self.handle, trigModeCode, c_double(burstDuration), numTrigger)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
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


      if 1:    #streamingEnabled   (FM: setStreamingMode use this flag) 
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

          try:
             adjRoiX = c_int(self.streaming_adjroix.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming ROI x value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             adjRoiY = c_int(self.streaming_adjroiy.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming ROI y value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             adjRoiW = c_int(self.streaming_adjroiw.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming ROI width value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          try:
             adjRoiH = c_int(self.streaming_adjroih.data())
          except:
             Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid streaming ROI height value')
             raise mdsExceptions.TclFAILED_ESSENTIAL

          self.debugPrint("lowLim ", lowLim)
          self.debugPrint("highLim ", highLim)
#          self.debugPrint("frameTempUnitCode ", frameTempUnitCode)
          self.debugPrint("streamingPort ", streamingPort)
          self.debugPrint("streamingServer ", streamingServer)
          self.debugPrint("streaming adj ROI x ", adjRoiX)
          self.debugPrint("streaming adj ROI y  ", adjRoiY)
          self.debugPrint("streaming adj ROI w  ", adjRoiW)
          self.debugPrint("streaming adj ROI h  ", adjRoiH)
          #FM: recover device name and pass it to setStreaming to overlay text on frame!!!
          deviceName = str(self).rsplit(":",1)
          deviceName = deviceName[1]
          self.debugPrint("Device Name ", deviceName)        

      status = PTGREY.ptgreyLib.setStreamingMode(self.handle, streamingEnabled,  autoAdjustLimit, c_char_p(streamingServer), streamingPort,  lowLim,  highLim, adjRoiX, adjRoiY, adjRoiW, adjRoiH, c_char_p(deviceName));
      if status < 0:
         PTGREY.ptgreyLib.getLastError(self.handle, self.error)
         Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute streaming setup mode : ' + self.error.raw)
         raise mdsExceptions.TclFAILED_ESSENTIAL


###Acquisition


      try:
        acqSkipFrameNumber = c_int( self.timing_skip_frame.data() )
      except:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid acquisition decimation value')
        raise mdsExceptions.TclFAILED_ESSENTIAL

      status = PTGREY.ptgreyLib.setAcquisitionMode(self.handle, storeEnabled , acqSkipFrameNumber)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute acquisition setup mode : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
       treePtr = c_void_p(0)
       status = PTGREY.mdsLib.camOpenTree(c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
       if status == -1:
         Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open tree')
         raise mdsExceptions.TclFAILED_ESSENTIAL
      except:
       traceback.print_exc()

      framesNid = self.frames.nid
      timebaseNid = self.timing_time_base.nid
      framesMetadNid = self.frames_metad.nid
      frame0TimeNid = self.frame0_time.nid

      status = PTGREY.ptgreyLib.setTreeInfo( self.handle,  treePtr,  framesNid,  timebaseNid,  framesMetadNid, frame0TimeNid)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute set tree info : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.debugPrint('Init action completed.')
      return 1





####################CHANGE GAIN
    def changeGain(self):
      restoreRes=self.restoreInfo()  #0:error 1:camera opened 2:camera was already opened
      if restoreRes == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL    

      status = PTGREY.ptgreyLib.setGain(self.handle, c_float(self.cam_setup_gain.data()))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot change Gain value : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if restoreRes == 1:    #camera must be closed because has been opened only for this action
        self.removeInfo()

      return 1


####################CHANGE EXPOSURE
    def changeExposure(self):
      restoreRes=self.restoreInfo()  #0:error 1:camera opened 2:camera was already opened
      if restoreRes == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL  

      status = PTGREY.ptgreyLib.setExposure(self.handle, c_float(self.cam_setup_exposure.data()))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot change Exposure value : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if restoreRes == 1:    #camera must be closed because has been opened only for this action
        self.removeInfo()

      return 1


####################READ INTERNAL TEMPERATURE
    def readTemperature(self):
      restoreRes=self.restoreInfo()  #0:error 1:camera opened 2:camera was already opened
      if restoreRes == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL  

      status = PTGREY.ptgreyLib.readInternalTemperature(self.handle)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Read Internal Temperature : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      if restoreRes == 1:    #camera must be closed because has been opened only for this action
        self.removeInfo()

      return 1



##########start acquisition############################################################################
    def startAcquisition(self):
      if self.checkWorker():    #FM 20191114
        self.debugPrint('Acquisition already in progress. Action not performed.')
        return 1  

      if self.checkInfo():    #FM 20191114
        self.restoreInfo()
      else:
        Data.execute('DevLogErr($1,$2)', self.nid, 'Init action must be performed before acquisition : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.debugPrint("Starting acquisition")
      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = PTGREY.ptgreyLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Start Camera Acquisition : '+self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.debugPrint("Acquisition started")
      self.worker.configure(self.copy())
      self.saveWorker()
      self.worker.start()
      return 1


##########stop acquisition############################################################################
    def stopAcquisition(self):
      if self.checkWorker():    #FM 20191114
        self.restoreWorker()
        self.worker.stop()
        self.worker.join()      #CT 20200917
        self.removeWorker()
      else:
        self.debugPrint("Camera is not in acquisition. Action not performed.")
      return 1


##########software trigger (start saving in mdsplus)############################################
    def swTrigger(self):
      if not self.checkWorker():    #FM 20191114
        self.debugPrint('Camera not in acquisition. Action not performed.')
        return 1  
      if not self.checkInfo():    #FM 20191114
        self.debugPrint('Camera not opened. Action not performed.')
        return 1  
      if self.restoreInfo() == 0:
        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.debugPrint('SOFTWARE TRIGGER')

      status = PTGREY.ptgreyLib.softwareTrigger(self.handle)
      if status < 0:
        PTGREY.ptgreyLib.getLastError(self.handle, self.error)
        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Execute Software Trigger : ' + self.error.raw)
        raise mdsExceptions.TclFAILED_ESSENTIAL

      return 1
