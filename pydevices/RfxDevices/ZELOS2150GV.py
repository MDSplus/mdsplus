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
from ctypes import CDLL,c_void_p,c_char_p,c_byte,c_short,c_int,c_float,byref
from datetime import datetime
from time import sleep, mktime

class ZELOS2150GV(Device):
    """Zelos 2150GV Camera"""
    parts=[
      {'path':':NAME', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':IP', 'type':'text'},
      {'path':':EXP_TIME', 'type':'numeric', 'value':20E-3},
      {'path':':AUTO_EXP', 'type':'text', 'value':'NO'},
      {'path':':EXP_LEV', 'type':'numeric', 'value':0},
      {'path':':AUTO_GAIN', 'type':'text', 'value':'NO'},
      {'path':':GAIN_LEV', 'type':'numeric', 'value':100},
      {'path':':SLOW_SCAN', 'type':'text', 'value':'NO'},
      {'path':':FRAME_X', 'type':'numeric', 'value':11},
      {'path':':FRAME_Y', 'type':'numeric', 'value':11},
      {'path':':FRAME_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':FRAME_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':MEAS_X', 'type':'numeric', 'value':11},
      {'path':':MEAS_Y', 'type':'numeric', 'value':11},
      {'path':':MEAS_WIDTH', 'type':'numeric', 'value':1920},
      {'path':':MEAS_HEIGHT', 'type':'numeric', 'value':1080},
      {'path':':VER_BINNING', 'type':'numeric', 'value':1},
      {'path':':HOR_BINNING', 'type':'numeric', 'value':1},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'INTERNAL'},
      {'path':':FRAME_PERIOD', 'type':'numeric', 'value':100E-3},
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')},
      {'path':':STREAMING', 'type':'text', 'value':'Stream and Store'},
      {'path':':STREAM_PORT', 'type':'numeric', 'value':8888},
      {'path':':STREAM_AUTOS', 'type':'text', 'value':'NO'},
      {'path':':STREAM_LOLIM', 'type':'numeric', 'value':0},
      {'path':':STREAM_HILIM', 'type':'numeric', 'value':32767}]
    parts.append({'path':':INIT_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'start_store',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
      'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'stop_store',head))",
      'options':('no_write_shot',)})
    handles = {}
    workers = {}
    kappaLib = None
    mdsLib = None
    streamLib = None


####Asynchronous readout internal class
    class AsynchStore(Thread):

      def configure(self, device, width, height, hBuffers):
        self.device = device
        self.width = width
        self.height = height
        self.hBuffers = hBuffers
        self.frameIdx = 0
        self.stopReq = False

      def run(self):
        frameType = c_short * (self.height.value * self.width.value) #used for acquired frame
        frameBuffer = frameType()

        frameType = c_byte * (self.height.value * self.width.value)  #used for streaming frame
        frame8bit = frameType()

        treePtr = c_void_p(0)
        status = ZELOS2150GV.mdsLib.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
        if status == -1:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
          raise mdsExceptions.TclFAILED_ESSENTIAL

        if self.device.frame_sync.data() == 'EXTERNAL':
          isExternal = 1
          timebaseNid=self.device.frame_clock.getNid()
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
        streamPort=c_int(self.device.stream_port.data())
        #frameTimeInt=0
        prevFrameTime=0
        totFrameTime=0
        framePeriod = int(self.device.frame_period.data()*1000)
        skipFrameStream=int(float(1/self.device.frame_period.data())/25.0)-1
        print('skipFrameStream:',skipFrameStream)
        if(skipFrameStream<0):
          skipFrameStream=0
        frameStreamCounter = skipFrameStream
        frameTotalCounter = 0
        status=c_int(-1)
        self.idx = 0

        while not self.stopReq:

          ZELOS2150GV.kappaLib.kappaGetFrame(self.device.handle, byref(status), frameBuffer)
          if status.value==3:
            print('get frame timeout!')
          else:
            frameStreamCounter = frameStreamCounter + 1   #reset according to Stream decimation
            frameTotalCounter = frameTotalCounter + 1     #never resetted

          if isExternal==0:  #internal clock source -> S.O. timestamp
             timestamp=datetime.now()
             frameTime=int(mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)  #ms
             if frameTotalCounter==1:
               prevFrameTime=int(mktime(timestamp.timetuple())*1000)+int(timestamp.microsecond/1000)
               deltaT=frameTime-prevFrameTime
               totFrameTime=0
             else:
               deltaT=frameTime-prevFrameTime
               prevFrameTime=frameTime
               totFrameTime=totFrameTime+deltaT
             if (deltaT<framePeriod) and (deltaT>5):
               sleep(float(framePeriod-deltaT)/1000.0)


          #if( (isStorage==1) and ((status.value==1) or (status.value==2)) ):    #frame complete or incomplete
          if( (isStorage==1) and (status.value==1)  ):  #frame complete
            ZELOS2150GV.mdsLib.camSaveFrame(frameBuffer, self.width, self.height, c_float(float(totFrameTime)/1000.0), c_int(14), treePtr, self.device.frames.getNid(), timebaseNid, c_int(frameTotalCounter-1), 0, 0, 0)
            self.idx = self.idx + 1
            print('saved frame idx:', self.idx)

          if(isStreaming==1):
            if(tcpStreamHandle.value==-1):
              fede=ZELOS2150GV.streamLib.camOpenTcpConnection(streamPort, byref(tcpStreamHandle), self.width, self.height)
              if(fede!=-1):
                print('\nConnected to FFMPEG on localhost:',streamPort.value)
            if(frameStreamCounter == skipFrameStream+1):
              frameStreamCounter=0
            if(frameStreamCounter == 0 and tcpStreamHandle.value!=-1):
              ZELOS2150GV.streamLib.camFrameTo8bit(frameBuffer, self.width, self.height, frame8bit, autoScale, byref(lowLim), byref(highLim), minLim, maxLim)
              ZELOS2150GV.streamLib.camSendFrameOnTcp(byref(tcpStreamHandle), self.width, self.height, frame8bit)

        #endwhile
        ZELOS2150GV.streamLib.camCloseTcpConnection(byref(tcpStreamHandle))
        #print 'Stream Tcp Connection Closed'

        status = ZELOS2150GV.kappaLib.kappaStopAcquisition(self.device.handle, self.hBuffers)
        if status != 0:
          Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot stop camera acquisition')

        #close device and remove from info
        ZELOS2150GV.kappaLib.kappaClose(self.device.handle)
        self.device.removeInfo()
        raise mdsExceptions.TclFAILED_ESSENTIAL


      def stop(self):
        self.stopReq = True
  #end class AsynchStore




###save worker###
    def saveWorker(self):
        ZELOS2150GV.workers[self.nid] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
        ZELOS2150GV.handels[self.nid] = self.handel

###restore worker###
    def restoreWorker(self):
      if self.nid in ZELOS2150GV.workers.keys():
        self.worker = ZELOS2150GV.workers[self.nid]
      else:
        print('Cannot restore worker!!')

###restore info###
    def restoreInfo(self):
      if ZELOS2150GV.kappaLib is None:
        ZELOS2150GV.kappaLib = CDLL("libkappazelos.so")
      if ZELOS2150GV.mdsLib is None:
        ZELOS2150GV.mdsLib = CDLL("libcammdsutils.so")
      if ZELOS2150GV.streamLib is None:
        ZELOS2150GV.streamLib = CDLL("libcamstreamutils.so")
      if self.nid in ZELOS2150GV.handels.keys():
        self.handel = ZELOS2150GV.handels[self.nid]
      else:
        print('RESTORE INFO HANDLE NON TROVATO')
        try:
          name = self.name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing device name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL

        self.handle = c_void_p(0)
        status = ZELOS2150GV.kappaLib.kappaOpen(c_char_p(name), byref(self.handle))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device '+ name)
          raise mdsExceptions.TclFAILED_ESSENTIAL
      return

###remove info###
    def removeInfo(self):
      try:
        del(ZELOS2150GV.handels[self.nid])
      except:
        print('ERROR TRYING TO REMOVE INFO')


##########init############################################################################
    def init(self):
      global kappaLib
      self.restoreInfo()
      self.frames.setCompressOnPut(False)

      status = ZELOS2150GV.kappaLib.kappaSetColorCoding(self.handle, c_int(6))   #Y14
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Color Coding')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Exposure	Mode
      if self.frame_sync.data() == 'EXTERNAL':
        status = ZELOS2150GV.kappaLib.kappaSetExposureMode(self.handle, c_int(3)) #3 = ZELOS_ENUM_EXPOSUREMODE_RESETRESTART
      else:
        status = ZELOS2150GV.kappaLib.kappaSetExposureMode(self.handle, c_int(2)) #2 = ZELOS_ENUM_EXPOSUREMODE_FREERUNNINGSEQUENTIAL

      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Exposure Mode')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Exposure
      autoExp = self.auto_exp.data()
      if autoExp == 'YES':
        status = ZELOS2150GV.kappaLib.kappaSetAET(self.handle, c_int(1))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AET On')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        status = ZELOS2150GV.kappaLib.kappaSetAutoExposureLevel(self.handle, c_int(self.exp_lev.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Auto Exposure Level')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      else:
        status = ZELOS2150GV.kappaLib.kappaSetAET(self.handle, c_int(0))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AET Off')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        status = ZELOS2150GV.kappaLib.kappaSetExposure(self.handle, c_float(self.exp_time.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Exposure Time')
          raise mdsExceptions.TclFAILED_ESSENTIAL

###Gain
      autoGain = self.auto_gain.data()
      if autoGain == 'YES':
        status = ZELOS2150GV.kappaLib.kappaSetAGC(self.handle, c_int(1))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AGC On')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      else:
        status = ZELOS2150GV.kappaLib.kappaSetAGC(self.handle, c_int(0))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set AGC On')
          raise mdsExceptions.TclFAILED_ESSENTIAL
        status = ZELOS2150GV.kappaLib.kappaSetGain(self.handle, c_int(self.gain_lev.data()))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Gain')
          raise mdsExceptions.TclFAILED_ESSENTIAL

###Slow Scan
      slowScan = self.slow_scan.data()
      if slowScan == 'YES':
       status = ZELOS2150GV.kappaLib.kappaSetSlowScan(self.handle, c_int(1))
      else:
       status = ZELOS2150GV.kappaLib.kappaSetSlowScan(self.handle, c_int(0))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Slow Scan')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Frame Area
      status = ZELOS2150GV.kappaLib.kappaSetReadoutArea(self.handle, c_int(self.frame_x.data()),c_int(self.frame_y.data()),c_int(self.frame_width.data()),c_int(self.frame_height.data()))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Readout Area')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Measure Area
      status = ZELOS2150GV.kappaLib.kappaSetMeasureWindow(self.handle, c_int(self.meas_x.data()),c_int(self.meas_y.data()),c_int(self.meas_width.data()),c_int(self.meas_height.data()))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set Measure Window')
        raise mdsExceptions.TclFAILED_ESSENTIAL

###Binning
      status = ZELOS2150GV.kappaLib.kappaSetBinning(self.handle, c_int(self.hor_binning), c_int(self.ver_binning))
      if status < 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set horizontal or vertical binning')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.saveInfo()
      return

####################trigger###PER ORA NON FUNZIONA
    def trigger(self):
      self.restoreInfo()
      synch = self.frame_sync.data()   ###Synchronization
      if synch == 'INTERNAL':
        timeMs = int(self.frame_period.data()/1E-3)
        status = ZELOS2150GV.kappaLib.kappaSetTriggerTimer(self.handle, c_int(timeMs))
        if status < 0:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set Frame period in internal sync mode')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      else:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot issue software trigger if external synchornization')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.saveInfo()
      return

##########start store############################################################################
    def start_store(self):
      global kappaLib
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
      status = ZELOS2150GV.kappaLib.kappaStartAcquisition(self.handle, byref(hBuffers), byref(width), byref(height), byref(payloadSize))
      if status != 0:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Start Camera Acquisition')
        raise mdsExceptions.TclFAILED_ESSENTIAL
      self.worker.configure(self, width, height, hBuffers)
      self.saveWorker()
      self.worker.start()
      return


##########stop store############################################################################
    def stop_store(self):
      print('STOP STORE')
      self.restoreWorker()
      self.worker.stop()
      print('FLAG SETTATO')
      return
