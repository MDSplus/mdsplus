from MDSplus import mdsExceptions, Device, Data, version, Int64, Float32
#from ctypes import CDLL, byref, c_double, c_int, c_void_p, c_char_p, create_string_buffer
#from numpy import array
from threading import Thread
#import traceback
try:
  import serial
except:
  pass
from datetime import datetime

class STK_TEMP(Device):
    """STK_TEMP Device"""
    parts=[                                                              #offset nid
      {'path':':PORT_NAME', 'type':'text', 'value':'/dev/ttyS0'},          #1
      {'path':':COMMENT', 'type':'text'},                                
      {'path':':TEMPERATURE', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}
      ]

    parts.append({'path':':INIT_ACT','type':'action',
      'valueExpr':"Action(Dispatch('STRIKE_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
      'valueExpr':"Action(Dispatch('STRIKE_SERVER','INIT',50,None),Method(None,'startAcquisition',head))",
      'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
      'valueExpr':"Action(Dispatch('STRIKE_SERVER','STORE',50,None),Method(None,'stopAcquisition',head))",
      'options':('no_write_shot',)})

    handle = -1
    handles = {}
    workers = {}

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

            self.device.handle.reset_input_buffer()
            idx=0
            epoch=datetime(1970,1,1)
            temperatureNid = self.device.temperature

            while not self.stopReq:
                if self.device.handle.in_waiting>0:
                  temperature=float(self.device.handle.readline().decode('utf-8').rstrip())
                  idx+=1
                  if idx==1:
                    timestamp0=datetime.utcnow()
                    deltaT=0.0
                    dt=timestamp0-epoch
                    int64Time=dt.microseconds/1000+dt.seconds*1000+dt.days*86400000
                  else:
                    timestamp=datetime.utcnow()
                    dt=timestamp-timestamp0
                    deltaT=dt.microseconds/1000000.0+dt.seconds+dt.days*86400
                    dt=timestamp-epoch
                    int64Time=dt.microseconds/1000+dt.seconds*1000+dt.days*86400000
                  print (deltaT, temperature)

                  temperatureNid.putRow(1, Float32(temperature), Int64(int64Time))

            self.device.debugPrint("End acquisition thread")
            self.device.removeInfo()

        def stop(self):
            self.device.debugPrint("STOP acquisition loop")
            self.stopReq = True


    def saveWorker(self):
        STK_TEMP.workers[self.nid] = self.worker

###save Info###
#saveInfo and restoreInfo allow to manage multiple occurrences of camera devices
#and to avoid opening and closing devices handles
    def saveInfo(self):
        STK_TEMP.handles[self.nid] = self.handle

###restore worker### 
    def restoreWorker(self):
        if self.nid in STK_TEMP.workers.keys():
            self.worker = STK_TEMP.workers[self.nid]
            return 1
        else:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot restore worker!!')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

###restore info###
    def restoreInfo(self):
      self.debugPrint("restore Info")

      if self.nid in STK_TEMP.handles.keys():
        self.handle = STK_TEMP.handles[self.nid]
        self.debugPrint('RESTORE INFO HANDLE TROVATO',  self.handle)
      else:
        self.debugPrint('RESTORE INFO HANDLE NON TROVATO')
        try:
          name = self.port_name.data()
        except:
          Data.execute('DevLogErr($1,$2)', self.nid, 'Missing device name' )
          raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
          self.handle = serial.Serial(name, 9600, timeout=0.5)      
          self.debugPrint("Serial Port opened: ", self.handle)
        except:
          self.debugPrint('ERROR opening serial port...')
          raise mdsExceptions.TclFAILED_ESSENTIAL

      return 1

###remove info###
    def removeInfo(self):
      try:
        self.handle.close()
        self.debugPrint('Serial Port closed...')
      except:
        self.debugPrint('ERROR TRYING TO CLOSE SERIAL PORT')

      try:
        del(STK_TEMP.handles[self.nid])
      except:
        self.debugPrint('ERROR TRYING TO REMOVE INFO')


##########init############################################################################
    def init(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      self.saveInfo()
      #### serial port opened on restoreInfo()

      self.debugPrint('Init action completed.')
      return 1



##########start acquisition############################################################################
    def startAcquisition(self):
      if self.restoreInfo() == 0:
          raise mdsExceptions.TclFAILED_ESSENTIAL

      #Module in acquisition check
      try:
          self.restoreWorker()
          if ( self.worker != None and self.worker.isAlive() ):
            self.debugPrint("Stop acquisition Thread...")
            self.stopAcquisition()
            self.debugPrint("Initialize...")
            self.init()
            self.restoreInfo()
      except:
          pass

      self.debugPrint("Starting Acquisition...")

      self.worker = self.AsynchStore()
      self.worker.daemon = True
      self.worker.stopReq = False


      self.debugPrint("OK!")
      self.worker.configure(self)
      self.saveWorker()
      self.worker.start()
      return 1


##########stop acquisition############################################################################
    def stopAcquisition(self):
      if self.restoreWorker() :
         self.worker.stop()
      return 1

