from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Int64
from threading import Thread
from ctypes import CDLL, byref, c_longlong, c_int, c_void_p, c_float, c_char_p, c_uint, c_short, c_byte, c_double, get_errno, Structure, c_ubyte
import os
import time
import sys, traceback
import exceptions
import select
import errno
from datetime import datetime


class SPIDER_TIMING(Device):
    """ niSync PXIe SPIDER timing device """
    parts=[{'path':':COMMENT',    'type':'text'},
           {'path':':DEV_TYPE',   'type':'text', 'value':'PXI6683H'},
           {'path':':DEV_NUM',    'type':'numeric', 'value':0},
           {'path':':TRIG_TERM',  'type':'text', 'value':'PFI1'},
           {'path':':TRIG_EDGE',  'type':'text', 'value':'RISING'},
           {'path':':TRIG_DEC_CNT',  'type':'numeric', 'value':1},
           {'path':':ACQ_MODE',   'type':'text', 'value':'SINGLE'},
           {'path':'.PULSE_TIME', 'type':'structure'},
           {'path':'.PULSE_TIME:TAI_NS', 'type':'numeric','options':('no_write_model')},
           {'path':'.PULSE_TIME:TAI_S' , 'type':'numeric','options':('no_write_model')},
           {'path':'.PULSE_TIME:DATE'  , 'type':'text','options':('no_write_model')}]


    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':ARM_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','READY',50,None),Method(None,'arm',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop',head))",
        'options':('no_write_shot',)})




    NISYNC_DEVICE_TYPE_UNKNOWN  = c_int(-1)
    NISYNC_DEVICE_TYPE_PXI6682  = c_int(0)
    NISYNC_DEVICE_TYPE_PCI1588  = c_int(1)
    NISYNC_DEVICE_TYPE_PXI6683  = c_int(2)
    NISYNC_DEVICE_TYPE_PXI6683H = c_int(3)

    NISYNC_READ_BLOCKING    = c_int(0) # Wait until some data is available
    NISYNC_READ_NONBLOCKING = c_int(1) # Return immediately even if no data is available

    NISYNC_EDGE_RISING	= c_int(0)
    NISYNC_EDGE_FALLING	= c_int(1)
    NISYNC_EDGE_ANY		= c_int(2)
    NISYNC_EDGE_INVALID	= c_int(3)


    NISYNC_PFI0 = c_int(0)
    NISYNC_PFI1 = c_int(1)
    NISYNC_PFI2 = c_int(2)

    NISYNC_RTSI0 = c_int(3)
    NISYNC_RTSI1 = c_int(4)
    NISYNC_RTSI2 = c_int(5)
    NISYNC_RTSI3 = c_int(6)
    NISYNC_RTSI4 = c_int(7)
    NISYNC_RTSI5 = c_int(8)
    NISYNC_RTSI6 = c_int(9)
    NISYNC_RTSI7 = c_int(10)

    NISYNC_PXI_TRIG0 = c_int(11)
    NISYNC_PXI_TRIG1 = c_int(12)
    NISYNC_PXI_TRIG2 = c_int(13)
    NISYNC_PXI_TRIG3 = c_int(14)
    NISYNC_PXI_TRIG4 = c_int(15)
    NISYNC_PXI_TRIG5 = c_int(16)
    NISYNC_PXI_TRIG6 = c_int(17)
    NISYNC_PXI_TRIG7 = c_int(18)

    NISYNC_PXI_STAR0 = c_int(19)
    NISYNC_PXI_STAR1 = c_int(20)
    NISYNC_PXI_STAR2 = c_int(21)
    NISYNC_PXI_STAR3 = c_int(22)
    NISYNC_PXI_STAR4 = c_int(23)
    NISYNC_PXI_STAR5 = c_int(24)
    NISYNC_PXI_STAR6 = c_int(25)
    NISYNC_PXI_STAR7 = c_int(26)
    NISYNC_PXI_STAR8 = c_int(27)
    NISYNC_PXI_STAR9 = c_int(28)
    NISYNC_PXI_STAR10 = c_int(29)
    NISYNC_PXI_STAR11 = c_int(30)
    NISYNC_PXI_STAR12 = c_int(31)
    NISYNC_CLK10 = c_int(36)
    NISYNC_GND = c_int(37)

# Non-board terminals
    NISYNC_SW = c_int(40)
    NISYNC_ETHIN = c_int(41)
    NISYNC_ETHOUT = c_int(42)
    NISYNC_DEVICE = c_int(43)
    NISYNC_TERMINAL_INVALID = c_int(45)

    NISYNC_FTE_LATENCY = c_int(0)
    NISYNC_INPUT_TIMESTAMP_LATENCY = c_int(1)
    NISYNC_SYS_TIME_LATENCY = c_int(2)
    NISYNC_ENET_TRANSMIT_LATENCY = c_int(3)
    NISYNC_ENET_RECEIVE_LATENCY = c_int(4)


    DevTypeDict  = {'UNKNOWN':-1 , 'PXI6682':0, 'PCI1588':1, 'PXI6683':2, 'PXI6683H':3}
    trigTermDict = {'PFI1':1, 'PFI2':2}
    trigEdgeDict = {'RISING':0, 'FALLING':1, 'ANY':2, 'INVALID':3}

    class nisyncTimestampNanos(Structure):
        #creates a struct to match nisync_timestamp_nanos
        _fields_ = [('edge', c_ubyte),
                    ('nanos', c_longlong)]    


    #devFd = None
    NiSyncLib = None
    fds = {}
    workers = {}


######### National Instruments Sync Device Manager 

    def saveInfo(self):
        SPIDER_TIMING.fds[self.nid] = self.devFd

    def restoreInfo(self):
         try:
            print 'restoreInfo'
            self.devFd = SPIDER_TIMING.fds[self.nid]
         except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
 
    def initializeInfo(self):
        if SPIDER_TIMING.NiSyncLib is None:
           SPIDER_TIMING.NiSyncLib = CDLL("libnisync.so")

        try:
            devType = c_int(self.DevTypeDict[self.dev_type.data()]);
        except:
            devType = self.NISYNC_DEVICE_TYPE_UNKNOWN

        try:
            devNum = c_int(self.dev_num.data());
        except:
            devNum = c_int(-1)

        try:
            devTerm = c_int(self.trigTermDict[self.trig_term.data()]);
        except:
            devTerm = c_int(-1)

        try:
            print "test",devType, devNum, devTerm, self.NISYNC_READ_NONBLOCKING
            self.devFd = SPIDER_TIMING.NiSyncLib.nisync_open_terminal(devType, devNum, devTerm, self.NISYNC_READ_NONBLOCKING);
            print "self.devFd", self.devFd

            if self.devFd < 0 :
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open terminal NiSync Timing Device')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except BaseException as e:
            print str(e)
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception on NiSync Timing Device open terminal')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return

    def closeInfo(self):
        try:
            self.devFd = SPIDER_TIMING.fds[self.nid]
            if self.devFd > 0 :
                os.close(self.devFd)
            del(SPIDER_TIMING.fds[self.nid])
            self.devFd = -1
        except:
            pass
        return

################################### Worker Management
    def saveWorker(self):
      SPIDER_TIMING.workers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        self.worker = SPIDER_TIMING.workers[self.nid]
      except:
        print('Cannot restore worker!!')

########################AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device):
            self.device = device
            self.stopReq = False
 
        def run(self):

            poll = select.poll()
            poll.register(self.device.devFd, select.POLLIN)

            nTimestamps = c_int(1)
            ts = self.device.nisyncTimestampNanos()

            timeout = 1000
            while not self.stopReq:
               poolfd = poll.poll(timeout)
               for fd1, event in poolfd :
                   print "fd    = ", fd1
                   print "event = ", event
               count = SPIDER_TIMING.NiSyncLib.nisync_read_timestamps_ns(self.device.devFd, byref(ts), nTimestamps);
               if count > 0 :
                   print ts.edge, ts.nanos
                   dt = datetime.fromtimestamp(ts.nanos // 1000000000)
                   dt_str = dt.strftime('%Y-%m-%d %H:%M:%S') +"."+ str(int(ts.nanos % 1000000000)).zfill(9)
                   print dt_str

                   try:
                       self.device.pulse_time_tai_ns.putData( Int64(ts.nanos))
                       self.device.pulse_time_tai_s.putData(Int64(ts.nanos//1000000000))
                       self.device.pulse_time_date.putData(dt_str)
                   except BaseException as e:
                       print e
                       print('Error save timestamp')
                   
                   break

            if self.device.devFd > 0 :
                poll.unregister(self.device.devFd)
                
            print 'AsynchStore stop'

            return

        def stop(self):
            self.stopReq = True

      
#############End Inner class AsynchStore

    def init(self):

#Configuration check

        try:
            devType = self.DevTypeDict[self.dev_type.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Device Type module Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            devNum = self.dev_num.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Device number Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            devTerm = self.trigTermDict[self.trig_term.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Device terminal Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigDecCnt = c_int(self.trig_dec_cnt.data())
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Trigger decimation count Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigEdge = c_int(self.trigEdgeDict[self.trig_edge.data()])
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Trigger edge Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#Open device

        try:
            self.initializeInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open NiSync IEEE1588 device')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        status = SPIDER_TIMING.NiSyncLib.nisync_enable_timestamp_trigger(self.devFd, trigEdge, trigDecCnt)

        print "self.devFd", self.devFd, status, trigEdge, trigDecCnt


        if status < 0 :  
            self.closeInfo()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot enable timestamp trigger')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1

    def arm(self):

        try:
            self.restoreInfo() 
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'SPIDER timing device device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

 
        self.worker = self.AsynchStore()        
        self.worker.daemon = True 
        self.worker.stopReq = False

        self.worker.configure(self)

        print "Start Worker"

        self.saveWorker()
        self.worker.start()

        if  not self.worker.isAlive() :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'SPIDER timing device device not armed')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1

    def stop(self):

        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'SPIDER timing device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.restoreWorker()
        if self.worker.isAlive():
            print "stop_worker"
            self.worker.stop()
            self.worker.join()
        print "Close Info"
        self.closeInfo()
        return 1

