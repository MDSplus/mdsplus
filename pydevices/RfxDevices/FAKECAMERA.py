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

from MDSplus import mdsExceptions, Device, Data
from threading import Thread
from ctypes import CDLL, c_void_p, c_int, c_short, c_byte, byref, c_char_p, c_float
import datetime
import time

class FAKECAMERA(Device):
    """Fake Camera"""
    parts=[
      {'path':':NAME', 'type':'text', 'value':'Fake Camera 1'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':EXP_NAME', 'type':'text', 'value':'cameratest'},
      {'path':':EXP_SHOT', 'type':'text', 'value':'84'},
      {'path':':EXP_NODE', 'type':'text', 'value':'\CAMERATEST::FLIR:FRAMES'},
      {'path':':FRAME_RATE', 'type':'numeric', 'value':25},
      {'path':':READ_LOOP', 'type':'text','value':'NO'},
      {'path':':STREAMING', 'type':'text', 'value':'Stream and Store'},
      {'path':':STREAM_PORT', 'type':'numeric', 'value':8888},
      {'path':':STREAM_AUTOS', 'type':'text', 'value':'NO'},
      {'path':':STREAM_LOLIM', 'type':'numeric', 'value':0},
      {'path':':STREAM_HILIM', 'type':'numeric', 'value':32767},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},]
    parts.append({'path':':INIT_ACT','type':'action',
          'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
          'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
          'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
          'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
          'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
          'options':('no_write_shot',)})

    handle = 0
    handles = {}
    workers = {}
    fakecamera = None
    cammdsutils = None
    camstreamutils = None


####Asynchronous readout internal class
    class AsynchStore(Thread):
      frameIdx = 0
      stopReq = False

      def configure(self, device, width, height, hBuffers):
        self.device = device
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
        status = FAKECAMERA.cammdsutils.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
        if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          raise mdsExceptions.TclFAILED_ESSENTIAL

        if self.device.streaming.data() == 'Stream and Store':
          isStreaming = 1
          isStorage = 1
        if self.device.streaming.data() == 'Only Stream':
          isStreaming = 1
          isStorage = 0
        if self.device.streaming.data() == 'Only Store':
          isStreaming = 0
          isStorage = 1

        infiniteLoop = self.device.read_loop.data()                 #to loop throw finite number of input frames

        autoScale = self.device.stream_autos.data()                 #autoscaling pixel grey depth for streaming operation
        if autoScale == 'YES':
          autoScale=c_int(1)
        else:
          autoScale=c_int(0)

        lowLim=c_int(self.device.stream_lolim.data())
        highLim=c_int(self.device.stream_hilim.data())
        minLim=c_int(0)
        maxLim=c_int(32767)

        tcpStreamHandle=c_int(-1)
        frameTime=0
        prevFrameTime=0
        status=c_int(-1)
        streamPort=c_int(self.device.stream_port.data())

        #framePeriod = int(self.device.frame_rate.data()*1000)
        skipFrameStream=int(float(self.device.frame_rate.data())/25.0)-1
        print('skipFrameStream:',skipFrameStream)
        if(skipFrameStream<0):
          skipFrameStream=0
        frameStreamCounter = skipFrameStream
        frameTotalCounter = 0

        readFrameCounter=1
        maxFramesToRead=FAKECAMERA.fakecamera.fakeGetNumFrames(self.device.handle)

        while not self.stopReq:
          if( (infiniteLoop=='YES') and (readFrameCounter == maxFramesToRead-1) ):     #to loop throw a finite num. of frames
            readFrameCounter=1

          FAKECAMERA.fakecamera.fakeGetFrame(self.device.handle, byref(status), frameBuffer, c_int(readFrameCounter))
          readFrameCounter = readFrameCounter + 1       #reset if read_loop=YES

          frameStreamCounter = frameStreamCounter + 1   #reset according to Stream decimation
          frameTotalCounter = frameTotalCounter + 1     #never resetted

          timestamp=datetime.datetime.now()
          frameTime=int(time.mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)  #ms
          if frameTotalCounter==1:
            prevFrameTime=int(time.mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)
            deltaT=frameTime-prevFrameTime
            totFrameTime=0
          else:
            deltaT=frameTime-prevFrameTime
            prevFrameTime=frameTime
            totFrameTime=totFrameTime+deltaT

          if( (isStorage==1) and ((status.value==1) or (status.value==2)) ):    #frame complete or incomplete
            FAKECAMERA.cammdsutils.camSaveFrame(frameBuffer, self.width, self.height, c_float(float(totFrameTime)/1000.0), c_int(14), treePtr, self.device.frames.getNid(), timebaseNid, c_int(frameTotalCounter-1), 0, 0, 0)
            self.idx = self.idx + 1
            print('saved frame idx:', self.idx)


          if(isStreaming==1):
            if(tcpStreamHandle.value==-1):
              fede=FAKECAMERA.camstreamutils.camOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)
              if(fede!=-1):
                print('\nConnected to FFMPEG on localhost:',streamPort.value)
            if(frameStreamCounter == skipFrameStream+1):
              frameStreamCounter=0
            if(frameStreamCounter == 0 and tcpStreamHandle.value!=-1):
              FAKECAMERA.camstreamutils.camFrameTo8bit(frameBuffer, self.width, self.height, frame8bit, autoScale, byref(lowLim), byref(highLim), minLim, maxLim)
              FAKECAMERA.camstreamutils.camSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frame8bit)

        #endwhile
        FAKECAMERA.camstreamutils.camCloseTcpConnection(byref(tcpStreamHandle))
        #print 'Stream Tcp Connection Closed'

        status = FAKECAMERA.fakecamera.fakeStopAcquisition(self.device.handle, self.hBuffers)
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        #close device and remove from info
        FAKECAMERA.fakecamera.fakeClose(self.device.handle)
        self.device.removeInfo()
        raise mdsExceptions.TclFAILED_ESSENTIAL

      def stop(self):
        self.stopReq = True
  #end class AsynchStore

###save worker###
    def saveWorker(self):
      FAKECAMERA.workers[self.getNid()] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
      FAKECAMERA.handlers[self.getNid()] = self.handle;

###restore worker###
    def restoreWorker(self):
      if self.getNid() in FAKECAMERA.handles.keys():
        self.worker = FAKECAMERA.workers[self.getNid()]
      else:
        print('Cannot restore worker!!')

###restore info###
    def restoreInfo(self):
      if FAKECAMERA.fakecamera is None:
        FAKECAMERA.fakecamera = CDLL("libfakecamera.so")
      if FAKECAMERA.cammdsutils is None:
        FAKECAMERA.cammdsutils = CDLL("libcammdsutils.so")
      if FAKECAMERA.camstreamutils is None:
        FAKECAMERA.camstreamutils = CDLL("libcamstreamutils.so")
      if self.getNid() in FAKECAMERA.handles.keys():
        self.handle = FAKECAMERA.handles[self.getNid()]
        if Device.debug: print('RESTORE INFO HANDLE TROVATO')
      else:
        print('RESTORE INFO HANDLE NON TROVATO')

        try:
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
          exp_name = self.exp_name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source experiment name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
          exp_shot = self.exp_shot.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source shot' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
          exp_node = self.exp_node.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing source frame node' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
          frame_rate = self.frame_rate.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing reading frame rate' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        self.handle = c_void_p(0)
        print('restoreInfo before fakeOpen')
        status = FAKECAMERA.lib.fakeOpen(c_char_p(exp_name), c_char_p(exp_shot), c_char_p(exp_node), c_float(frame_rate), byref(self.handle))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
          raise mdsExceptions.TclFAILED_ESSENTIAL
        if Device.debug: print('restoreInfo ended')
      return

###remove info###
    def removeInfo(self):
      try:
        del(FAKECAMERA.hanles[self.getNid()])
      except:
        print('ERROR TRYING TO REMOVE INFO')


##########init############################################################################
    def init(self):
      self.restoreInfo()
      self.frames.setCompressOnPut(False)

#      status = FakeCam.Lib.fakeSetColorCoding(self.handle, c_int(6));
#      if status < 0:
#        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Color Coding')
#        raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      return


##########start store############################################################################
    def start_store(self):
      self.restoreInfo()
      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False
      hBuffers = c_void_p(0)
      width = c_int(0)
      height = c_int(0)
      payloadSize = c_int(0)
      status = FAKECAMERA.fakecamera.fakeStartAcquisition(self.handle, byref(hBuffers), byref(width), byref(height), byref(payloadSize))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.worker.configure(self, width, height, hBuffers)
      self.saveWorker()
      self.worker.start()
      return


##########stop store############################################################################
    def stop_store(self):
      if Device.debug: print('STOP STORE')
      self.restoreWorker()
      self.worker.stop()
      print('done')
      return
