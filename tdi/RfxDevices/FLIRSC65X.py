from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time

class FLIRSC65X(Device):
    print 'FLIRSC65X'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """FLIR655 Camera"""
    parts=[
      {'path':':NAME', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},  
      {'path':':OBJ_EMIS', 'type':'numeric', 'value':920E-3},
      {'path':':OBJ_DIST', 'type':'numeric', 'value':2},
      {'path':':OBJ_RFL_TMP', 'type':'numeric', 'value':20},
      {'path':':OBJ_OPT_TMP', 'type':'numeric', 'value':20},   
      {'path':':OBJ_OPT_TRS', 'type':'numeric', 'value':1},
      {'path':':OBJ_ATM_TMP', 'type':'numeric', 'value':20},   
      {'path':':OBJ_ATM_HUM', 'type':'numeric', 'value':50},     
      {'path':':OBJ_ATM_TRS', 'type':'numeric', 'value':99E-2},      
      {'path':':FRAME_X', 'type':'numeric', 'value':0},
      {'path':':FRAME_Y', 'type':'numeric', 'value':0},
      {'path':':FRAME_WIDTH', 'type':'numeric', 'value':640},
      {'path':':FRAME_HEIGHT', 'type':'numeric', 'value':480},  
      {'path':':CAM_F_LENGTH', 'type':'text', 'value':'50'}, 
      {'path':':CAM_MS_RANGE', 'type':'text', 'value':'0...650'},                         
      {'path':':IMG_TEMP', 'type':'text', 'value':'LinearTemperature10mK'},            
      {'path':':FRAME_SYNC', 'type':'text', 'value':'INT. TRIGGER'},
      {'path':':FRAME_TBASE', 'type':'numeric'},
      {'path':':FRAME_RATE', 'type':'text', 'value':'50'},   
      {'path':':FRAME_TSTART', 'type':'numeric', 'value':0}, 
      {'path':':FRAME_BRST_D', 'type':'numeric', 'value':5},    
      {'path':':FRAME_NR_TRG', 'type':'numeric', 'value':3},    
      {'path':':CALIB_AUTO', 'type':'text', 'value':'NO'},   
      {'path':':CALIB_TIME', 'type':'numeric', 'value':4},            
      {'path':':STREAMING', 'type':'text', 'value':'Stream and Store'},
      {'path':':STREAM_PORT', 'type':'numeric', 'value':8888},
      {'path':':STREAM_AUTOS', 'type':'text', 'value':'YES'},  
      {'path':':STREAM_LOLIM', 'type':'numeric', 'value':15},
      {'path':':STREAM_HILIM', 'type':'numeric', 'value':50},
      {'path':':SKP_FR_STORE', 'type':'numeric', 'value':0},    
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':FRAMES_METAD', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot',)})
    print 'FLIRSC65X added'
    
    
    handle = 0

####Asynchronous readout internal class       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False
   
    
      def configure(self, device, flirLib, mdsLib, streamLib, width, height, payloadSize):
        self.device = device
        self.flirLib = flirLib
        self.mdsLib = mdsLib
        self.streamLib = streamLib
        self.width = width
        self.height = height
        self.payloadSize = payloadSize


      def run(self):
        frameType = c_short * (self.height.value * self.width.value) #used for acquiring frame
        frameBuffer = frameType()

        frameType = c_byte * (self.height.value * self.width.value)  #used for streaming frame
        frame8bit = frameType()

        frameType = c_byte * (self.payloadSize.value - (sizeof(c_short) * self.height.value * self.width.value) )  #used for metadata
        metaData = frameType()
        metaSize=c_int(self.payloadSize.value - (sizeof(c_short) * self.height.value * self.width.value))

        treePtr = c_void_p(0)
        status = self.mdsLib.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
        if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          return 0

        if self.device.frame_sync.data() == 'EXT. TRIGGER':       #internal/external trigger
          isExternal = 1
          timebaseNid=self.device.frame_tbase.getNid()
        else:
          isExternal = 0
          timebaseNid=c_int(-1)
       
        if self.device.streaming.data() == 'Stream and Store': 
          isStreaming = 1
          isStorage = 1
        if self.device.streaming.data() == 'Only Stream': 
          isStreaming = 1
          isStorage = 0
        if self.device.streaming.data() == 'Only Store': 
          isStreaming = 0
          isStorage = 1

        frameRate = self.device.frame_rate.data()                  #preserve 25fps in streaming
        if frameRate == '200':
          skipFrameStream=4
        elif frameRate == '100': 
          skipFrameStream=2
        elif frameRate == '50': 
          skipFrameStream=1
        elif frameRate == '25': 
          skipFrameStream=0
        elif frameRate == '12.5': 
          skipFrameStream=0
        elif frameRate == '6.25': 
          skipFrameStream=0
        elif frameRate == '3.12': 
          skipFrameStream=0

        streamPort=c_int(self.device.stream_port.data())            #streaming vars
        tcpStreamHandle=c_int(-1)

        autoCalib = self.device.calib_auto.data()                   #enable/disable auto calibration
        if autoCalib == 'NO':
          self.flirLib.setCalibMode(self.device.handle, c_int(0))   
        else:
          self.flirLib.setCalibMode(self.device.handle, c_int(1))   
        
        autoScale = self.device.stream_autos.data()                 #autoscaling pixel grey depth for streaming operation
        if autoScale == 'YES':
          autoScale=c_int(1)
        else:
          autoScale=c_int(0)
        #scaling limits depends on img_temperature
        #for flir minLim=2000   =200K=-73C
        #for flir maxLim=62000  =6200K=5927C

        img_temperature = self.device.img_temp.data()               #modify limits according to image temperature precision
        if img_temperature == 'LinearTemperature10mK':
          img_temperature=c_int(10)
          lowLim=c_int(self.device.stream_lolim.data()*100)
          highLim=c_int(self.device.stream_hilim.data()*100)
          minLim=c_int(0)            
          maxLim=c_int(62000-27315)      #346.85°C  
        if img_temperature == 'LinearTemperature100mK':
          img_temperature=c_int(100)
          lowLim=c_int(self.device.stream_lolim.data()*10)
          highLim=c_int(self.device.stream_hilim.data()*10)
          minLim=c_int(0)         
          maxLim=c_int(62000-27315)      #3468.5°C 
     
        skipFrameStore = self.device.skp_fr_store.data()           #frames store decimation
        frameStoreCounter = skipFrameStore
        frameStreamCounter = skipFrameStream                       #frames stream decimation

        burstDuration = self.device.frame_brst_d.data()            #external trigger timing
        burstNframe = burstDuration * float(frameRate) + 1
        burstFrameCount = 0
        Ntrigger = self.device.frame_nr_trg.data()
        NtriggerCount = 0

        frameTime=0                                        #relative frame time used only in internal trigger
        startStoreTrg = 0                                  #software trigger / hardware trigger mask     
        status=c_int(-1)                                   #operation status                             
        self.idx = 0                                       #frame index passed to saving frames on mdsplus
        frameTotalCounter = 0

        while not self.stopReq:

          self.flirLib.getFrame(self.device.handle, byref(status), frameBuffer, metaData)                    #get the frame
          self.flirLib.frameConv(self.device.handle, frameBuffer, self.width, self.height, img_temperature)  #convert kelvin in Celsius

          frameStoreCounter = frameStoreCounter + 1     #reset according to Store decimation
          frameStreamCounter = frameStreamCounter + 1   #reset according to Stream decimation
          frameTotalCounter = frameTotalCounter + 1     #never resetted     
          
          if isExternal==1:                          #external clock source
            burstFrameCount = burstFrameCount + 1
            if (burstFrameCount == burstNframe):        
              startStoreTrg=0                           #disable storing
              burstFrameCount = 0
              NtriggerCount = NtriggerCount + 1
              if autoCalib == 'NO':                     #execute calibration action @ every burst of frames (only if NO auto calibration)
                self.device.calib(0)
            if NtriggerCount==Ntrigger:                 #stop store when all trigger are received              
              self.device.stop_store(0)   
            if (status.value==4):                       #start data storing @ 1st trigger seen (trigger is on image header!)
              startStoreTrg=1
          else:                                      #internal clock source
            startStoreTrg=1           
            frameTime = (frameTotalCounter-1) * 1./float(frameRate)

          if(isStorage==1 and startStoreTrg==1):                              #is a frame to be saved?
            if ((status.value==1) or (status.value==2) or (status.value==4)): #is the frame complete, incomplete or triggered?
              if (frameStoreCounter == skipFrameStore+1):                       #is a frame NOT to be skipped for decimation?
                #self.mdsLib.camSaveFrame(frameBuffer, self.width, self.height, c_float(frameTime), c_int(14), treePtr, self.device.frames.getNid(), timebaseNid, c_int(self.idx))
                savestatus=self.mdsLib.camSaveFrame(frameBuffer, self.width, self.height, c_float(frameTime), c_int(14), treePtr, self.device.frames.getNid(), timebaseNid, c_int(frameTotalCounter-1), metaData, metaSize,self.device.frames_metad.getNid()) 

                frameStoreCounter=0
                self.idx = self.idx + 1

          if(isStreaming==1):
            if(tcpStreamHandle.value==-1): 
              fede=self.streamLib.camOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)
              if(fede!=-1):
                print '\nConnected to FFMPEG on localhost:',streamPort.value
            if(frameStreamCounter == skipFrameStream+1):
              frameStreamCounter=0
            if(frameStreamCounter == 0 and tcpStreamHandle.value!=-1):
              self.streamLib.camFrameTo8bit(frameBuffer, self.width, self.height, frame8bit, autoScale, byref(lowLim), byref(highLim), minLim, maxLim)
              self.streamLib.camSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frame8bit)             

        #endwhile

        self.streamLib.camCloseTcpConnection(byref(tcpStreamHandle))  
    
        status = self.flirLib.stopAcquisition(self.device.handle)  #stop camera acquisition
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        if autoCalib == 'NO':
          self.flirLib.setCalibMode(self.device.handle, c_int(1))  #re-enable auto calibration
       
        self.flirLib.flirClose(self.device.handle)                 #close device and remove from info
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot close camera')

        self.device.removeInfo()
        return 0


      def stop(self):
        self.stopReq = True
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
        print 'Cannot restore worker!!'

###restore info###   
    def restoreInfo(self):
      global cameraHandles
      global cameraNids
      global flirLib
      global streamLib
      global mdsLib
      try:
        flirLib
      except:
        flirLib = CDLL("libflirsc65x.so")
      try:
        mdsLib
      except:
        mdsLib = CDLL("libcammdsutils.so")
      try:
        streamLib
      except:
        streamLib = CDLL("libcamstreamutils.so")
      try:
        idx = cameraNids.index(self.getNid())
        self.handle = cameraHandles[idx]
        print 'RESTORE INFO HANDLE TROVATO'
      except:
        print 'RESTORE INFO HANDLE NON TROVATO'
        try: 
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          return 0

        self.handle = c_int(0)
        status = flirLib.flirOpen(c_char_p(name), byref(self.handle))
        if status < 0:  
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
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
      
      if  self.restoreInfo() == 0:
          return 0      
  
      self.frames.setCompressOnPut(False)	

###Object Parameters
      status = flirLib.setObjectParameters(self.handle, c_double(self.obj_rfl_tmp.data()), c_double(self.obj_atm_tmp.data()), c_double(self.obj_dist.data()), c_double(self.obj_emis.data()), c_double(self.obj_atm_hum.data()), c_double(self.obj_opt_tmp.data()), c_double(self.obj_opt_trs.data()), c_double(self.obj_atm_trs.data()))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Object Parameters')
        return 0


###Frame Rate
      skipFrameStream = c_int(1)
      frameRate = self.frame_rate.data()

      if frameRate == '200':
        frInt=c_int(0)
      elif frameRate == '100': 
        frInt=c_int(1)
      elif frameRate == '50': 
        frInt=c_int(2)
      elif frameRate == '25': 
        frInt=c_int(3)
      elif frameRate == '12.5': 
        frInt=c_int(4)
      elif frameRate == '6.25': 
        frInt=c_int(5)
      elif frameRate == '3.12': 
        frInt=c_int(6)

      status = flirLib.setFrameRate(self.handle, frInt, byref(skipFrameStream))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Frame Rate')
        return 0


###Frame Area
      x=0
      y=0
      width=0
      height=0
      status = flirLib.getReadoutArea(self.handle, byref(c_int(x)), byref(c_int(y)), byref(c_int(width)), byref(c_int(height)))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Get Readout Area')
        return 0 

      #write data in mdsplus
      self.frame_x.putData(x)
      self.frame_y.putData(y)
      self.frame_width.putData(width)
      self.frame_height.putData(height)


###Measurement Range
      measureRange = self.cam_ms_range.data()

      if measureRange == '-40...150':
        measRangeInt=c_int(0)
      elif measureRange == '0...650': 
        measRangeInt=c_int(1)
      elif measureRange == '300...2000': 
        measRangeInt=c_int(2)

      status = flirLib.setMeasurementRange(self.handle, measRangeInt)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Measurement Range')
        return 0


###Image Temperature
      imgTemp = self.img_temp.data()

      if imgTemp == 'Radiometric':
        imgTempInt=c_int(0)
      elif imgTemp == 'LinearTemperature10mK': 
        imgTempInt=c_int(1)
      elif imgTemp == 'LinearTemperature100mK': 
        imgTempInt=c_int(2)

      status = flirLib.setIrFormat(self.handle, imgTempInt)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Image Temperature')
        return 0

###Frame Sync
      if self.frame_sync.data() == 'INT. TRIGGER':   #0=internal  1=external trigger
        expModeInt=c_int(0) 
      else:
        expModeInt=c_int(1) 

      status = flirLib.setExposureMode(self.handle, expModeInt)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Internal/External Trigger')
        return 0


###Auto Calibration
      status = flirLib.executeAutoCalib(self.handle)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Calibration')
        return 0

###Auto Focus
      status = flirLib.executeAutoFocus(self.handle)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Focus')
        return 0
        

      print 'Init action completed.'
      self.saveInfo()
      return 1

####################MANUAL CALIBRATION ACTION
    def calib(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      status = flirLib.executeAutoCalib(self.handle)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Calibration')
        return 0

      self.saveInfo()
      return 1


####################MANUAL AUTOFOCUS ACTION
    def autofocus(self,arg):
      global flirLib

      if self.restoreInfo() == 0:
          return 0  

      status = flirLib.executeAutoFocus(self.handle)
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Execute Auto Focus')
        return 0

      self.saveInfo()
      return 1

		
##########start store############################################################################   
    def start_store(self, arg):
      global flirLib
      global mdsLib
      global streamLib

      if self.restoreInfo() == 0:
          return 0      

      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
#     hBuffers = c_void_p(0)
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = flirLib.startAcquisition(self.handle, byref(width), byref(height), byref(payloadSize))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        return 0
      self.worker.configure(self, flirLib, mdsLib, streamLib, width, height, payloadSize)
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
