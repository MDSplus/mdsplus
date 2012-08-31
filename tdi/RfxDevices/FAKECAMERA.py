from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import datetime
import time

class FAKECAMERA(Device):
    print 'Fake Camera'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Fake Camera"""
    parts=[
      {'path':':NAME', 'type':'text', 'value':'Fake Camera 1'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':EXP_NAME', 'type':'text', 'value':'cameratest'},
      {'path':':EXP_SHOT', 'type':'text', 'value':'84'},
      {'path':':EXP_NODE', 'type':'text', 'value':'\CAMERATEST::FLIR:FRAMES'},
      {'path':':FRAME_RATE', 'type':'numeric', 'value':25},
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
    print 'Fake Camera added'
    
    
    handle = 0

####Asynchronous readout internal class       
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False

      def configure(self, device, FakeCamLib, mdsLib, streamLib, width, height, hBuffers):
        self.device = device
        self.FakeCamLib = FakeCamLib
        self.mdsLib = mdsLib
        self.streamLib = streamLib
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
        timebaseNid=c_int(-1);  #frame time remapped with storage time by S.O.
        status = self.mdsLib.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
        if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          return 0

        if self.device.streaming.data() == 'Stream and Store': 
          isStreaming = 1
          isStorage = 1
        if self.device.streaming.data() == 'Only Stream': 
          isStreaming = 1
          isStorage = 0
        if self.device.streaming.data() == 'Only Store': 
          isStreaming = 0
          isStorage = 1


        tcpStreamHandle=c_int(-1)
        frameTime=0
        prevFrameTime=0
        status=c_int(-1)
        streamPort=c_int(self.device.stream_port.data())

        lowLim=c_int(0)
        highLim=c_int(32767)
        framecounter=0

        while not self.stopReq:

          self.FakeCamLib.fakeGetFrame(self.device.handle, byref(status), frameBuffer, c_int(framecounter))	
          framecounter = framecounter + 1


          timestamp=datetime.datetime.now()
          prevFrameTime=frameTime
          frameTime=int(time.mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)
          deltaT=frameTime-prevFrameTime
          #print 'delta T: ', deltaT
          #print 'timestamp 64bit: ', frameTime
          #ts=datetime.datetime.now()
          #ft=int(time.mktime(ts.timetuple())*1000)+int(ts.microsecond/1000)
          #print 'timestamp 64bit post wait: ', ft

          if( (isStorage==1) and ((status.value==1) or (status.value==2)) ):    #frame complete or incomplete
            self.mdsLib.camSaveFrame(frameBuffer, self.width, self.height, byref(c_int64(frameTime)), c_int(14), treePtr, self.device.frames.getNid(), timebaseNid, c_int(self.idx)) 
            self.idx = self.idx + 1
            print 'saved frame idx:', self.idx

          if ( (isStreaming==1) and (tcpStreamHandle.value==-1) ):   
            self.streamLib.camOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)   
            #print 'Stream Tcp Connection Opened. Handle:',tcpStreamHandle.value

          if ( (isStreaming==1) and (tcpStreamHandle.value!=-1) ): 
            #self.streamLib.camFrameTo8bit(frameBuffer, self.width, self.height, c_int(14), frame8bit);
            self.streamLib.camFrameTo8bit(frameBuffer, self.width, self.height, frame8bit, c_int(0), byref(lowLim), byref(highLim), c_int(0), c_int(32767));
            self.streamLib.camSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frame8bit);
   
        #endwhile
        self.streamLib.camCloseTcpConnection(byref(tcpStreamHandle))
        #print 'Stream Tcp Connection Closed'
    
        status = self.FakeCamLib.fakeStopAcquisition(self.device.handle, self.hBuffers)
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        #close device and remove from info
        self.FakeCamLib.fakeClose(self.device.handle)
        self.device.removeInfo()
        return 0



      def stop(self):
        self.stopReq = True
  #end class AsynchStore




###save worker###  
    def saveWorker(self):
      global fakeWorkers
      global fakeWorkerNids
      try:
        fakeWorkers
      except:
	fakeWorkerNids = []
        fakeWorkers = []
      try:
        idx = fakeWorkerNids.index(self.getNid())
        fakeWorkers[idx] = self.worker
      except:
        print 'SAVE WORKER: NEW WORKER'
        fakeWorkerNids.append(self.getNid())
        fakeWorkers.append(self.worker)
        return
      return

###save Info###   
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices 
#and to avoid opening and closing devices handles 
    def saveInfo(self):
      global fakeHandles
      global fakeNids
      try:
        fakeHandles
      except:
	fakeHandles = []
	fakeNids = []
      try:
        idx = fakeNids.index(self.getNid())
      except:
        print 'SAVE INFO: SAVING HANDLE'
        fakeHandles.append(self.handle)
        fakeNids.append(self.getNid())
        return
      return

###restore worker###   
    def restoreWorker(self):
      global fakeWorkerNids
      global fakeWorkers    
      try:
        idx = fakeWorkerNids.index(self.getNid())
        self.worker = fakeWorkers[idx]
      except:
        print 'Cannot restore worker!!'

###restore info###   
    def restoreInfo(self):
      global fakeHandles
      global fakeNids
      global FakeCamLib
      global streamLib
      global mdsLib
      try:
        FakeCamLib
      except:
        FakeCamLib = CDLL("libfakecamera.so")
      try:
        mdsLib
      except:
        mdsLib = CDLL("libcammdsutils.so")
      try:
        streamLib
      except:
        streamLib = CDLL("libcamstreamutils.so")
      try:
        idx = fakeNids.index(self.getNid())
        self.handle = fakeHandles[idx]
        print 'RESTORE INFO HANDLE TROVATO'
      except:
        print 'RESTORE INFO HANDLE NON TROVATO'

        try: 
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          return 0

        try: 
          exp_name = self.exp_name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source experiment name' )
          return 0

        try: 
          exp_shot = self.exp_shot.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source shot' )
          return 0

        try: 
          exp_node = self.exp_node.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source frame node' )
          return 0

        try: 
          frame_rate = self.frame_rate.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing reading frame rate' )
          return 0

        self.handle = c_void_p(0)
        status = FakeCamLib.fakeOpen(c_char_p(exp_name), c_char_p(exp_shot), c_char_p(exp_node), c_float(frame_rate), byref(self.handle))
        if status < 0:  
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
          return 0

      return

###remove info###    
    def removeInfo(self):
      global fakeHandles
      global fakeNids
      try:
        fakeNids.remove(self.getNid())
        fakeHandles.remove(self.handle)
      except:
        print 'ERROR TRYING TO REMOVE INFO'
      return


##########init############################################################################    
    def init(self,arg):
      global FakeCamLib
      self.restoreInfo()
      self.frames.setCompressOnPut(False)	

#      status = FakeCamLib.fakeSetColorCoding(self.handle, c_int(6)); 
#      if status < 0:
#        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Color Coding')
#        return 0

      self.saveInfo()
      return 1

		
##########start store############################################################################   
    def start_store(self, arg):
      global FakeCamLib
      global mdsLib
      global streamLib
      self.restoreInfo()
      self.worker = self.AsynchStore()        
      self.worker.daemon = True 
      self.worker.stopReq = False
      hBuffers = c_void_p(0)
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = FakeCamLib.fakeStartAcquisition(self.handle, byref(hBuffers), byref(width), byref(height), byref(payloadSize))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        return 0
      self.worker.configure(self, FakeCamLib, mdsLib, streamLib, width, height, hBuffers)
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
