import  MDSplus
import threading
import time
from ctypes import CDLL, c_int, c_double, c_char_p, byref, c_byte


class RFX_TRIGUART(MDSplus.Device):
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':HI_DIV', 'type': 'numeric', 'value': 500},
             {'path': ':LO_DIV', 'type': 'numeric', 'value': 500},
             {'path': ':SEG_SIZE', 'type': 'numeric', 'value': 1000},
             {'path': ':TRIG_TIME', 'type': 'numeric', 'value': 0.},
             {'path': ':CHAN_1', 'type': 'signal'},
             {'path': ':CHAN_2', 'type': 'signal'},
             {'path': ':CHAN_3', 'type': 'signal'},
             {'path': ':CHAN_4', 'type': 'signal'},
             {'path': ':CHAN_5', 'type': 'signal'}]

    stopped = {}
    workers = {}
    fds = {}

    class AsynchStore(threading.Thread):
        def configure(self, lib, nid, segLen, chanNodes, startTime, period):
            self.nid = nid
            self.lib = lib
            self.segLen = segLen
            self.startTime = startTime
            self.period = period
            self.chanNodes = []
            for chan in range(5):
                self.chanNodes.append(chanNodes[chan].copy())

                                          
                                          
        def run(self):
            self.segmentCount = 0
            #When awakened, device has been triggered
            fd = self.lib.rpuartStartStore()
            DataArray = c_byte * self.segLen
            rawChan = []
            rawChan.append(DataArray())
            rawChan.append(DataArray())
            rawChan.append(DataArray())
            rawChan.append(DataArray())
            rawChan.append(DataArray())

            while not RFX_TRIGUART.stopped[self.nid]:
                self.lib.rpuartGetSegment(c_int(fd), c_int(self.segLen), byref(rawChan[0]), byref(rawChan[1]), 
                             byref(rawChan[2]), byref(rawChan[3]), byref(rawChan[4]))
                startTime = MDSplus.Float64(self.startTime + self.segmentCount * self.segLen * self.period)
                endTime = MDSplus.Float64(self.startTime + (self.segmentCount + 1) * self.segLen * self.period)
                dim = MDSplus.Range(startTime, endTime, MDSplus.Float64(self.period))
                for chan in range(5):
                    data = MDSplus.Int8Array(rawChan[chan])
                    self.chanNodes[chan].makeSegment(startTime, endTime, dim, data)
                    print('SEGMENT APPENDED')
                self.segmentCount += 1
            self.lib.rpuartStopStore(c_int(fd))

    def init(self):
        print('================= RPUART Init ===============')
        try:
            lib = CDLL("libredpitaya.so")
            print('library loaded')
        except:
            print('Cannot load redpitaya.so')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            hiDiv = self.hi_div.data()
        except:
            print('Cannot get high divisions')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            loDiv = self.lo_div.data()
        except:
            print('Cannot get low divisions')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        status = lib.rpuartInit(c_int(hiDiv), c_int(loDiv))
        if status < 0:
            print("Error initializing device")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        RFX_TRIGUART.fds[self.getNid()] = -1
        
    def trigger(self):
        lib = CDLL("libredpitaya.so")
        try:
            fd = RFX_TRIGUART.fds[self.nid]
        except:
            fd = -1
        if fd > 0:  #Device already open by readout thread
            lib.rpuartTriggerFd(c_int(fd))
        else:
            lib.rpuartTrigger()
    
    def start_store(self):
        worker = self.AsynchStore()
        lib = CDLL("libredpitaya.so")
        nid = self.getNid()
        segLen = self.seg_size.data()
        self.chan_1.deleteData()
        self.chan_2.deleteData()
        self.chan_3.deleteData()
        self.chan_4.deleteData()
        self.chan_5.deleteData()
        chanNodes=[]
        chanNodes.append(self.chan_1)
        chanNodes.append(self.chan_2)
        chanNodes.append(self.chan_3)
        chanNodes.append(self.chan_4)
        chanNodes.append(self.chan_5)
        startTime = self.trig_time.data()
        period = (self.hi_div.data() + self.lo_div.data()) * 1E-6
        worker.configure(lib, nid, segLen, chanNodes, startTime, period)
        RFX_TRIGUART.workers[nid] = worker
        RFX_TRIGUART.stopped[nid] = False
        
        worker.start()

    def stop_store(self):
        try:
            worker = RFX_TRIGUART.workers[self.getNid()]
            RFX_TRIGUART.stopped[self.getNid()] = True
            print('Waiting worker to stop....')
            worker.join()
            print('Worker done')
        except:
            print('Cannot get worker')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL



 