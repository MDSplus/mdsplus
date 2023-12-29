from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Int64, Int8, Tree
from threading import Thread
from ctypes import CDLL, byref, c_longlong, c_int, c_void_p, c_float, c_char_p, c_uint, c_short, c_byte, c_double, get_errno, Structure, c_ubyte
import os
import time
import sys, traceback
import select
import errno
from datetime import datetime


class NI6683REC(Device):
    """ niSync PXIe SPIDER timing device """
    parts=[{'path':':COMMENT',    'type':'text'},
           {'path':':DEV_TYPE',   'type':'text', 'value':'PXI6683H'},
           {'path':':DEV_NUM',    'type':'numeric', 'value':0},
           {'path':':TRIG_DEC_CNT',  'type':'numeric', 'value':1},
           {'path':':TAIUTC_DELAY',   'type':'numeric', 'valueExpr': 'Data.compile("37000000000Q")' },
           {'path':'.PULSE_TIME', 'type':'structure'},
           {'path':'.PULSE_TIME:REF_TIME',  'type':'numeric', 'value':-6.},
           {'path':'.PULSE_TIME:TRIG_TERM',  'type':'text', 'value':'PFI1'},
           {'path':'.PULSE_TIME:TRIG_EDGE',  'type':'text', 'value':'RISING'},
           {'path':'.PULSE_TIME:TAI_NS', 'type':'numeric','options':('no_write_model')},
           {'path':'.PULSE_TIME:UTC_NS', 'type':'numeric',
                 'valueExpr': 'Data.compile("$1 - $2",head.pulse_time_tai_ns, head.taiutc_delay)'},
           {'path':'.PULSE_TIME:DATE'  , 'type':'text','options':('no_write_model')}]

    for i in range(8):
        parts.append({'path':'.TRIG_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.TRIG_%d:COMMENT'%(i+1),  'type':'text'})
        parts.append({'path':'.TRIG_%d:TERM'%(i+1),  'type':'text', 'value':'PFI2'})
        parts.append({'path':'.TRIG_%d:EDGE'%(i+1),  'type':'text', 'value':'RISING'})
        parts.append({'path':'.TRIG_%d:TAI_NS'%(i+1), 'type':'numeric','options':('no_write_model')})
        parts.append({'path':'.TRIG_%d:UTC_NS'%(i+1), 'type':'numeric',
                       'valueExpr': 'Data.compile("($1 - $2)", head.trig_%d_tai_ns, head.taiutc_delay)'%(i+1)})
        parts.append({'path':'.TRIG_%d:TIME'%(i+1), 'type':'numeric',
                       'valueExpr': 'Data.compile("data(($1 - $2)/1000000000.)+$3", head.trig_%d_tai_ns, head.pulse_time_tai_ns, head.pulse_time_ref_time)'%(i+1)})


    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','READY',50,None),Method(None,'start',head))",
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

    NISYNC_PTPD_DISABLED = c_int(0)
    NISYNC_PTPD_INITIALIZING = c_int(1)
    NISYNC_PTPD_SLAVE = c_int(2)
    NISYNC_PTPD_LISTENING = c_int(3)
    NISYNC_PTPD_MASTER = c_int(4)
    NISYNC_PTPD_FAULTY = c_int(5)


    DevTypeDict  = {'UNKNOWN':-1 , 'PXI6682':0, 'PCI1588':1, 'PXI6683':2, 'PXI6683H':3}
    trigTermDict = {'PFI0':0, 'PFI1':1, 'PFI2':2, 'TRIG0':11, 'TRIG1':12, 'TRIG2':13, 'TRIG3':14, 'TRIG4':15, 'TRIG5':16, 'TRIG6':17, 'TRIG7':18}
    trigEdgeDict = {'RISING':0, 'FALLING':1, 'ANY':2, 'INVALID':3}

    class nisyncTimestampNanos(Structure):
        #creates a struct to match nisync_timestamp_nanos
        _fields_ = [('edge', c_ubyte),
                    ('nanos', c_longlong)]    


    #devFd = None
    NiSyncLib = None
    fds = {}
    nids = {}
    workers = {}


######### National Instruments Sync Device Manager 

    def saveInfo(self):
        NI6683REC.fds[self.nid] = self.devFd
        NI6683REC.nids[self.nid] = self.timeNid

    def restoreInfo(self):
         try:
            print ('restoreInfo')
            self.devFd = NI6683REC.fds[self.nid]
            self.timeNid = NI6683REC.nids[self.nid]
         except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
 
    def closeInfo(self):
        try:
            self.devFd = NI6683REC.fds[self.nid]
            for fd in self.devFd:
                if fd == -1 :
                   continue
                os.close(fd)
            del(NI6683REC.fds[self.nid])
            self.devFd = -1               
            del NI6683REC.nids[self.nid]
        except:
            pass
        return

    def initializeInfo(self):

        if NI6683REC.NiSyncLib is None:
           NI6683REC.NiSyncLib = CDLL("libnisync.so")
 
        try:
            devType = c_int(self.DevTypeDict[self.dev_type.data()])
        except:
            devType = self.NISYNC_DEVICE_TYPE_UNKNOWN

        try:
            devNum = c_int(self.dev_num.data())
        except:
            devNum = c_int(-1)

        try:
            trigTermName = self.pulse_time_trig_term.data()
            trigTerm = c_int(self.trigTermDict[trigTermName])
        except:
            trigTerm = c_int(-1)


        fd = NI6683REC.NiSyncLib.nisync_open_device(devType, devNum)
        if not fd < 0 :
            ptpd_state = c_int(0)
            #status = NI6683REC.NiSyncLib.nisync_set_ptpd_state(fd, self.NISYNC_PTPD_SLAVE)
            status = NI6683REC.NiSyncLib.nisync_get_ptpd_state(fd, byref(ptpd_state))
            if status < 0 :
            	Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read PTPd service state')
            	raise mdsExceptions.TclFAILED_ESSENTIAL
            print("PTPd service state %d"%(ptpd_state.value))
            if ptpd_state.value != 2 :
            	Data.execute('DevLogErr($1,$2)', self.getNid(), 'PTPd service is not running or not synchronized')
            	raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device %s num %d Timing Device'%(devType, devNum))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        os.close(fd)


        self.devFd = []
        self.timeNid = {}
        try:

            print ("test",devType, devNum, trigTerm, self.NISYNC_READ_NONBLOCKING)
            self.devFd.append(NI6683REC.NiSyncLib.nisync_open_terminal(devType, devNum, trigTerm, self.NISYNC_READ_NONBLOCKING));
            print ("Pulse ", trigTermName, self.devFd[0])

            nids = []
            nids.append(self.pulse_time_tai_ns.getPath())     
            nids.append(self.pulse_time_date.getPath())     
                 
            self.timeNid[self.devFd[0]] = nids    
                       
            if self.devFd[0] < 0 :
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open terminal %s NiSync Timing Device'%(trigTermName))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            for tr in range(8):
                if getattr(self, 'trig_%d'%(tr+1)).isOn():
                    try:
                        trigTermName = getattr(self, 'trig_%d_term'%(tr+1)).data()
                        trigTerm = c_int(self.trigTermDict[trigTermName])
                    except:
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid terminal for trigger %s '%(tr+1))
                        continue

                    #print ("TRIG_%d"%(tr+1),devType, devNum, trigTerm, self.NISYNC_READ_NONBLOCKING)
                    print ("TRIG_%d"%(tr+1), trigTermName, trigTerm)
                    self.devFd.append(NI6683REC.NiSyncLib.nisync_open_terminal(devType, devNum, trigTerm, self.NISYNC_READ_NONBLOCKING))
                    
                    nids = []
                    nids.append( getattr(self, 'trig_%d_tai_ns'%(tr+1)).getPath() )     
                    self.timeNid[self.devFd[len(self.devFd)-1]] = nids    

                    print ("devFd", trigTermName, self.devFd[len(self.devFd)-1])
                else:
                    self.devFd.append(-1)

            for pp, nn in self.timeNid.items():
               for n in nn:
                  print( self.getNode(n).getPath() )

        except BaseException as e:
            print (str(e))
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception on NiSync Timing Device on open terminal %s '%(trigTermName))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return



################################### Worker Management
    def saveWorker(self):
      NI6683REC.workers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        self.worker = NI6683REC.workers[self.nid]
      except:
        self.worker = None
        print('Cannot restore worker!!')

########################AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device):
            self.device = device
            self.stopReq = False
 
        def run(self):

            self.device.setTree(
                Tree(self.device.getTree().name, self.device.getTree().shot))
            #self.device = self.device.copy()


            poll = select.poll()
            for fd in self.device.devFd :
               if fd == -1 :
                  continue
               print ("Poll register fd", fd)
               poll.register(fd, select.POLLIN)

            nTimestamps = c_int(1000)
            #ts = self.device.nisyncTimestampNanos()
            ts=(self.device.nisyncTimestampNanos*1000)()

            try:
               tai_utc_delay = self.device.taiutc_delay.data()
            except:
               tai_utc_delay = 37000000000;
               self.device.taiutc_delay.putData(Int64(tai_utc_delay))

            """
            for pp, nn in self.device.timeNid.items():
               for n in nn:
                  print("Node ", n )
                  print("Node Path ", self.device.getNode(n).getPath() )
            """
   
            first = 1
            timeout = 1000
            ts_nanos_prev = 0
            while not self.stopReq:
               poolfd = poll.poll(timeout)
               for fd1, event in poolfd :
                   print( "fd    = ", fd1)
                   print( "event = ", event)
                   count = NI6683REC.NiSyncLib.nisync_read_timestamps_ns(fd1, byref(ts), nTimestamps);
                   print( "count = ", count)
                   print( "tai_utc_delay = ", tai_utc_delay)
                   print( "------------------------------------------------")
                   for i in range(count) :
                       print( 1000000000. / (ts[i].nanos-ts_nanos_prev), ts[i].nanos, count )
                       dt = datetime.fromtimestamp((ts[i].nanos-tai_utc_delay) // 1000000000)
                       dt_str = dt.strftime('%Y-%m-%d %H:%M:%S') +"."+ str(int(( (ts[i].nanos-tai_utc_delay) % 1000000000))).zfill(9)
                       print( dt_str)

                       nids = self.device.timeNid[fd1];
                       try:
                           if ( len(nids) == 2 ) :
                               print( self.device.getNode(nids[0]).getPath())
                               #nids[0].putData(Int64(ts[i].nanos))
                               self.device.getNode(nids[0]).putRow(1000, Int64(ts[i].nanos), Int64(ts[i].nanos) )
                               if first :
                                  first = 0
                                  print( self.device.getNode(nids[1]).getPath())
                                  self.device.getNode(nids[1]).putData(dt_str)
                           else:
                               print( self.device.getNode(nids[0]).getPath())
                               self.device.getNode(nids[0]).putRow(1000, Int64(ts[i].nanos), Int64(ts[i].nanos) )
                       except BaseException as e:
                           print( e)
                           print('Error save timestamp')
                           
                       """
                       try:
                           self.device.pulse_time_tai_ns.putData( Int64(ts.nanos))
                           self.device.pulse_time_tai_s.putData(Int64(ts.nanos//1000000000))
                           self.device.pulse_time_date.putData(dt_str)
                       except BaseException as e:
                           print (e)
                           print('Error save timestamp')
                       """
                       ts_nanos_prev = ts[i].nanos
                     #break
                   print ("------------------------------------------------")
               time.sleep(1)

            for fd in self.device.devFd :
               if fd == -1 :
                  continue
               poll.unregister(fd)
                
            print ('AsynchStore stop')

            return

        def stop(self):
            self.stopReq = True

      
#############End Inner class AsynchStore

    def init(self):

        try:
            self.restoreInfo() 
            self.stop()
        except:
            print ('Not started')
            pass
 

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

        """
        try:
            devTerm = self.trigTermDict[self.trig_term.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Device terminal Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """

        try:
            trigDecCnt = c_int(self.trig_dec_cnt.data())
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Trigger decimation count Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigEdge = c_int(self.trigEdgeDict[self.pulse_time_trig_edge.data()])
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Pulse time Trigger edge Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

#Open device

        try:
            self.initializeInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open NiSync IEEE1588 device')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        """
        status = NI6683REC.NiSyncLib.nisync_enable_timestamp_trigger(self.devFd[0], trigEdge, trigDecCnt)

        print ("self.devFd", self.devFd, status, trigEdge, trigDecCnt)


        if status < 0 :  
            self.closeInfo()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot enable pulse time timestamp trigger')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        for tr in range(8):
            if getattr(self, 'trig_%d'%(tr+1)).isOn():
                try:
                    trigEdgeName = getattr(self, 'trig_%d_edge'%(tr+1)).data()
                    trigEdge = c_int(self.trigEdgeDict[trigEdgeName])
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid trigger edge for trigger %s '%(tr+1))
                    continue

                status = NI6683REC.NiSyncLib.nisync_enable_timestamp_trigger(self.devFd[tr+1], trigEdge, trigDecCnt)
                if status < 0 :  
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot enable pulse time timestamp for trigger %d'%(tr+1))
                    continue
        """ 
        return 1

    def start(self):

        try:
            self.restoreInfo() 
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'SPIDER timing device device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigDecCnt = c_int(self.trig_dec_cnt.data())
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Trigger decimation count Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigEdge = c_int(self.trigEdgeDict[self.pulse_time_trig_edge.data()])
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Pulse time Trigger edge Undefined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        status = NI6683REC.NiSyncLib.nisync_enable_timestamp_trigger(self.devFd[0], trigEdge, trigDecCnt)

        print ("self.devFd", self.devFd, status, trigEdge, trigDecCnt)


        if status < 0 :  
            self.closeInfo()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot enable pulse time timestamp trigger')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        for tr in range(8):
            if getattr(self, 'trig_%d'%(tr+1)).isOn():
                try:
                    trigEdgeName = getattr(self, 'trig_%d_edge'%(tr+1)).data()
                    trigEdge = c_int(self.trigEdgeDict[trigEdgeName])
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid trigger edge for trigger %s '%(tr+1))
                    continue

                status = NI6683REC.NiSyncLib.nisync_enable_timestamp_trigger(self.devFd[tr+1], trigEdge, trigDecCnt)
                if status < 0 :  
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot enable pulse time timestamp for trigger %d'%(tr+1))
                    continue

#Module in acquisition check
        try:
            self.restoreWorker()
            if self.worker.isAlive():
               Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module is in acquisition')
               return
        except:
               pass

        self.worker = self.AsynchStore()        
        self.worker.daemon = True 
        self.worker.stopReq = False

        self.worker.configure(self.copy())

        print ("Start Worker")

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
        print (">>>>>>>>>>>>", self.worker)
        if self.worker != None and self.worker.isAlive():
            print ("stop_worker")
            self.worker.stop()
            self.worker.join()
        print ("Close Info")
        self.closeInfo()
        return 1

