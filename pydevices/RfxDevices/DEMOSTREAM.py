from MDSplus import Device, Int16Array, Float64, Range, Event, Float32
from threading import Thread
from ctypes import CDLL, c_int, c_char_p, c_short, byref, c_float
from datetime import datetime
import time

class DEMOSTREAM(Device):
    """A Demo 1 Channel, 16 bit stream digitizer"""
    parts = [
        {'path': ':ADDR', 'type': 'text'}, {
            'path': ':COMMENT', 'type': 'text'},
        {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 10000},
        {'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0},
        {'path': ':SIG', 'type': 'signal'},
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER','INIT',50,None),Method(None,'init',head))",
         'options': ('no_write_shot',)},
        {'path': ':START_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER','STORE',50,None),Method(None,'start_store',head))",
         'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER','STORE',100,None),Method(None,'stop_store',head))",
         'options': ('no_write_shot',)},
    ]
    workers = {}
    NUM_CHUNK_SAMPLES = 1000


############################################################
    def saveWorker(self):
        self.workers[self.nid] = self.worker

    def checkWorker(self):
        if self.nid in self.workers:
            if self.workers[self.nid].isAlive():
                print('Avcquisition already running!!')
                return False
        return True


# AsynchStore class
    class AsynchStore(Thread):
        def configure(self, deviceLib, addr, period, dataNode, numChunkSamples,triggerTime, event=None):
            self.deviceLib = deviceLib
            self.addr = addr
            self.dataNode = dataNode
            self.period = period
            self.currTime = triggerTime
            self.stopped = False
            self.prevSecond = 0
            self.NUM_CHUNK_SAMPLES = numChunkSamples

        def run(self):
            DataArray = c_short * self.NUM_CHUNK_SAMPLES
            rawChan = DataArray()
            self.dataNode.deleteData()

            while not self.stopped:
                status = self.deviceLib.acquireChunk(
                    c_char_p(self.addr.encode()), byref(rawChan), c_int(self.NUM_CHUNK_SAMPLES))
                if status == -1:
                    print ('Acquisition Failed')
                    return

                dataArr = Int16Array(rawChan)
                startTime = Float64(self.currTime)
                endTime = Float64(
                    self.currTime + self.period * self.NUM_CHUNK_SAMPLES)
                dim = Range(startTime, endTime, Float64(self.period))
                self.dataNode.makeSegment(startTime, endTime, dim, dataArr)
                self.currTime += self.period * self.NUM_CHUNK_SAMPLES

                currSecond = time.time()
                if currSecond > self.prevSecond + 2:
                    Event.setevent('DEMOSTREAM_READY')
                    self.prevSecond = currSecond

        def stop(self):
            self.stopped = True

################################################################################

    def start_store(self):
        try:
            deviceLib = CDLL("libDemoAdcShr.so")
        except:
            print ('Cannot link to device library')
            return 0

        try:
            addr = self.addr.data()
        except:
            print ('Missing Address in device')
            return 0
        try:
            frequency = self.clock_freq.data()
            period = 1./frequency
        except:
            print ('Missing clock frequency')
            return 0
        try:
            trigTime = self.trig_source.data()
        except:
            print ('Missing trigger time')
            return 0

        if not self.checkWorker():
            return 0

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.configure(deviceLib, addr, period, self.sig, self.NUM_CHUNK_SAMPLES, trigTime)
        self.saveWorker()
        
        status = deviceLib.initializeStream(c_char_p(addr.encode()), c_float(frequency), c_float(trigTime))
        self.worker.start()
        return 1

    def stop_store(self):
        try:
            self.workers[self.nid].stop()
        except:
            print('Cannot stop worker!!')
#        self.worker.stop()

#############################################################

    def init(self):
        try:
            deviceLib = CDLL("libDemoAdcShr.so")
        except:
            print ('Cannot link to device library')
            return 0

        try:
            addr = self.addr.data()
        except:
            print ('Missing Address in device')
            return 0

        try:
            frequency = self.clock_freq.data()
        except:
            print ('Missing clock frequency')
            return 0
        try:
            trigTime = self.trig_source.data()
        except:
            print ('Missing trigger time')
            return 0

        return 1
