#
# Copyright (c) 2022, Massachusetts Institute of Technology All rights reserved.
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
import MDSplus
import socket
import threading
import time
import numpy as np
import sys
if sys.version_info < (3,):
    from Queue import Queue, Empty
else:
    from queue import Queue, Empty

class _ACQ2106_435SM(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ435 Digitizers (up to 6) opperating as Slow Monitors.

    32 Channels * number of slots
    Minimum 10Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    carrier_parts = [
        {
            'path': ':NODE',        
            'type': 'text',
            'value': '192.168.0.254',
            'options': ('no_write_shot',)
        },
        {
            'path': ':COMMENT',     
            'type': 'text',
            'options': ('no_write_shot',)
        },
        {
            'path': ':DATA_EVENT',   
            'type': 'text',
            'value': 'SMDATA', 
            'options': ('no_write_shot',)
        },
        {
            'path': ':RUNNING',     
            'type': 'numeric',
            'options': ('no_write_model',)
        },
        {
            'path': ':INIT_ACTION', 
            'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head,'auto'))", 
            'options': ('no_write_shot',)
        },
        {
            'path': ':STOP_ACTION', 
            'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",      
            'options': ('no_write_shot',)
        },
    ]



    # self.empty_buffers.put(Sample(self.nchans))
    # ..
    # try:
    #     sample = self.empty_buffers.get(block=False)
    # except Empty:
    #    sample = Sample(self.nchans)
    #
    # s.recv_into(sample.buffer, len(sample.buffer))
    # sample.time = time.time_ns()
    #
    # self.full_buffers.put(sample)

    class DataWorker(threading.Thread):
        INITIAL_BUFFER_COUNT = 10
        
        # One data point per channel at a given time
        class DataPoint:
            def __init__(self, nchans):
                self.buffer = bytearray(nchans * np.int32(0).nbytes)
                self.time = 0

        def __init__(self, mdsplus_device):
            super(_ACQ2106_435SM.DataWorker, self).__init__(name=mdsplus_device.path)
            
            self.mdsplus_device = mdsplus_device

            self.nchans = self.mdsplus_device.sites * 32

            self.empty_buffers = Queue()
            self.full_buffers  = Queue()

            for i in range(self.INITIAL_BUFFER_COUNT):
                self.empty_buffers.put(self.DataPoint(self.nchans))

            self.acq_thread = self.ACQWorker(self)

        def run(self):
            from influxdb import InfluxDBClient
            self.client = InfluxDBClient('localhost', 8086, 'admin', 'password', 'slowmon435')

            self.mdsplus_device = self.mdsplus_device.copy()

            event_name = self.mdsplus_device.data_event.data()

            channel_list = []
            coefficient_list = []
            offset_list = []

            for i in range(self.nchans):
                channel = getattr(self.mdsplus_device, 'input_%3.3d' % (i + 1))
                channel_list.append(channel)
                coefficient_list.append(channel.COEFFICIENT.data())
                offset_list.append(channel.OFFSET.data())

            self.acq_thread.start()
            running = self.mdsplus_device.running

            while running.on or not self.full_buffers.empty():
                # Stop data acquisition but continue processing full_buffers
                if not running.on and self.acq_thread.running:
                    self.acq_thread.stop()

                start = time.time()
                try:
                    data_point = self.full_buffers.get()
                except Empty:
                    continue
                
                samples = np.right_shift(np.frombuffer(data_point.buffer, dtype='int32'), 8)

                influx_points = []
                mdsplus_points= []

                for i, ch in enumerate(channel_list):
                    if ch.on:
                        calibrated = samples[i] * coefficient_list[i] + offset_list[i]
                        
                        influx_points.append({
                            "measurement": 'INPUT_%3.3d' % (i+1),
                            "time": data_point.time,
                            "fields": {
                                'sample': calibrated
                            }
                        })
                        #mdsplus_points.append('PutRow(%s, %d, %dQ, %d)' % (ch, 1, data_point.time, samples[i]))
                        #ch.putRow(1000, samples[i], MDSplus.Uint64(data_point.time))
                        ch.putRow(3600, samples[i], data_point.time)

                # start = time.time()               
                self.client.write_points(influx_points, time_precision='n')

                # start = time.time()
                # self.mdsplus_device.tree.tdiExecute(','.join(mdsplus_points))            
                # end  = time.time()
                # print('tdiExecute', end - start)
                
                MDSplus.Event.setevent(event_name)

                self.empty_buffers.put(data_point)
                end  = time.time()
                print('putRow and write_points', end - start)

            if self.acq_thread.running:
                self.acq_thread.stop()
        
        class ACQWorker(threading.Thread):

            def __init__(self, data_worker):
                super(_ACQ2106_435SM.DataWorker.ACQWorker, self).__init__()
                self.data_worker = data_worker
                self.node_addr = data_worker.mdsplus_device.node.data()

            def stop(self):
                self.running = False

            def run(self):

                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.node_addr, 53666))
                s.settimeout(6)

                self.running = True
                while self.running:
                    try:
                        sample = self.data_worker.empty_buffers.get(block=False)

                    except Empty:
                        print("NO BUFFERS AVAILABLE. MAKING NEW ONE")
                        sample = self.data_worker.DataPoint(self.data_worker.nchans)

                    first = True
                    toread = len(sample.buffer)
                    try:
                        view = memoryview(sample.buffer)
                        while toread:
                            nbytes = s.recv_into(view, toread)
                            print(nbytes)
                            if first:
                                sample.time = time.time_ns()
                                first = False
                            view = view[nbytes:]  # slicing views is cheap
                            toread -= nbytes

                    except socket.timeout as e:
                        print("Got a timeout.")
                        err = e.args[0]
                        # this next if/else is a bit redundant, but illustrates how the
                        # timeout exception is setup

                        if err == 'timed out':
                            time.sleep(1)
                            print (' recv timed out, retry later')
                            continue
                        else:
                            print (e)
                            break

                    except socket.error as e:
                        # Something else happened, handle error, exit, etc.
                        print("socket error", e)
                        break             

                    else:
                        if toread != 0:
                            print ('orderly shutdown on server end')
                            break
                        else:
                            self.data_worker.full_buffers.put(sample)
    
    def init(self):
        uut = self.getUUT()

        # Fetching all calibration information from every channel.
        uut.fetch_all_calibration()
        coeffs = uut.cal_eslo[1:]
        eoff   = uut.cal_eoff[1:]

        chans = []
        nchans = self.sites * 32
        
        if nchans != uut.nchan():
            print("Number of channels mismatch %d != %d" % (nchans, uut.nchan()))

        for i in range(nchans):
            chans.append(getattr(self, 'INPUT_%3.3d' % (i + 1)))

        for i, ch in enumerate(chans):
            if ch.on:
                ch.OFFSET.putData(float(eoff[i]))
                ch.COEFFICIENT.putData(float(coeffs[i]))

        self.running.on = True

        thread = self.DataWorker(self)
        thread.start()

    INIT = init


    def stop(self):
        self.running.on = False
    STOP = stop

    def getUUT(self):
        import acq400_hapi
        #uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut = acq400_hapi.factory(self.node.data())
        return uut

    def setChanScale(self, num):
        chan = self.__getattr__('INPUT_%3.3d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT, MDSplus.dVALUE()), chan.OFFSET))

def assemble(cls):
    cls.parts = list(_ACQ2106_435SM.carrier_parts)
    for i in range(cls.sites * 32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i + 1,),            
                'type': 'SIGNAL', 
                'valueExpr': 'head.setChanScale(%d)' % (i + 1,),
                'options': ('no_write_model',)
            },
            {
                'path': ':INPUT_%3.3d:COEFFICIENT' % (i + 1,), 
                'type': 'NUMERIC',
                'options': ('no_write_model', 'write_once',)
            },
            {
                'path': ':INPUT_%3.3d:OFFSET' % (i + 1,),    
                'type': 'NUMERIC',
                'options': ('no_write_model', 'write_once',)
            },
        ]


class ACQ2106_435SM_1ST(_ACQ2106_435SM):
    sites = 1


assemble(ACQ2106_435SM_1ST)


class ACQ2106_435SM_2ST(_ACQ2106_435SM):
    sites = 2


assemble(ACQ2106_435SM_2ST)


class ACQ2106_435SM_3ST(_ACQ2106_435SM):
    sites = 3


assemble(ACQ2106_435SM_3ST)


class ACQ2106_435SM_4ST(_ACQ2106_435SM):
    sites = 4


assemble(ACQ2106_435SM_4ST)


class ACQ2106_435SM_5ST(_ACQ2106_435SM):
    sites = 5


assemble(ACQ2106_435SM_5ST)


class ACQ2106_435SM_6ST(_ACQ2106_435SM):
    sites = 6


assemble(ACQ2106_435SM_6ST)

del(assemble)
