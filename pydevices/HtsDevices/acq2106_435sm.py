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
import os

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
        {
            'path': ':FREQ', 
            'type': 'numeric',
            'value': 20000, 
            'options': ('no_write_shot',)
        },
        {
            'path': ':TRIG_MODE', 
            'type': 'text',
            'value': 'master:hard', 
            'options': ('no_write_shot',)
        },
        # PRE and POST samples
        {
            'path':':PRESAMPLES',
            'type':'numeric',   
            'value': int(1e5), 
            'options':('no_write_shot',)
        },
        {
            'path':':POSTSAMPLES',
            'type':'numeric',  
            'value': int(1e5), 
            'options':('no_write_shot',)
        },
        {
            'path':':MEV_PATH',
            'type':'text',  
            'value': '/srv/acq-multi-event/acq2106_161/', 
            'options':('no_write_shot',)
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
    class MultiEventWorker(threading.Thread):
        def __init__(self, mdsplus_device, uut):
            super(_ACQ2106_435SM.MultiEventWorker, self).__init__(name=mdsplus_device.path)
            self.mdsplus_device = mdsplus_device
            self.uut = uut
            self.presamples = mdsplus_device.presamples.data()
            self.postsamples = mdsplus_device.postsamples.data()
            self.clock_period  = 1.0 / mdsplus_device.freq.data()
            self.sites = mdsplus_device.sites
            self.mev_path = mdsplus_device.mev_path.data()

        def run(self):
            self.mdsplus_device = self.mdsplus_device.copy()

            running = self.mdsplus_device.running
            # Note: This value is not cleared between shots.
            last_mev_latest = self.uut.s1.MEV_LATEST

            self.chans = []
            nchans = self.sites * 32
            for i in range(nchans):
                self.chans.append(getattr(self.mdsplus_device, 'INPUT_%3.3d:TRANSIENT'%(i + 1)))

            stride = nchans + 4

            while running.on:
                mev_latest = self.uut.s1.MEV_LATEST

                filename = mev_latest.split(' ')[1]
                filepath = os.path.join(self.mev_path, filename)

                #if mev_latest == last_mev_latest and os.path.exists(mev_latest): 
                if mev_latest == last_mev_latest: 
                    time.sleep(1)
                    continue
                
                time.sleep(2)

                data = None
                data_t = None
                try:
                    data = np.fromfile(filepath, dtype='int32', count=-1, sep='')
                    data_435 = np.right_shift(data, 8)
                except FileNotFoundError:
                    print("Unable to open multi-event data file", filepath)
                    continue
                
                last_mev_latest = mev_latest
                
                tai_cur = np.uint32(data[nchans + 1])
                tai_ver = np.uint32(data[nchans + 2])

                # WR Nanosec per Tick:
                self.wrtd_tickns = float(self.uut.cC.WRTD_TICKNS)
                vernier   = (tai_ver & 0x0FFFFFFF)
                vernier_ns = vernier * self.wrtd_tickns
                tai_time = (tai_cur * 1_000_000_000) + vernier_ns # TAI time in ns

                # For now, TAI time is 37 secs ahead of UTC
                begin = tai_time - 37_000_000_000
                print("Begin   %20d" %(begin,))
                print("Time_ns %20d" %(time.time_ns(),))

                duration = (self.presamples + self.postsamples) * self.clock_period
                deltat = self.clock_period * 1_000_000_000

                end = begin + (duration * 1_000_000_000)
                dim = MDSplus.Range(MDSplus.Uint64(begin), MDSplus.Uint64(end), MDSplus.Uint64(deltat))
                
                for i, ch in enumerate(self.chans):
                    if ch.on:
                        channel_data = data_435[ i :: stride ]
                        ch.makeSegment(begin, end, dim, channel_data)

                os.unlink(filepath)
        
    class SlowMonitorWorker(threading.Thread):
        INITIAL_BUFFER_COUNT = 10
        
        # One data point per channel at a given time
        class DataPoint:
            SPAD_SIZE = np.int32(0).nbytes * 4

            def __init__(self, nchans):
                self.buffer = bytearray(nchans * np.int32(0).nbytes + self.SPAD_SIZE)
                self.time = 0

        def __init__(self, mdsplus_device, uut):
            super(_ACQ2106_435SM.SlowMonitorWorker, self).__init__(name=mdsplus_device.path)
            
            self.mdsplus_device = mdsplus_device

            self.nchans = self.mdsplus_device.sites * 32

            self.empty_buffers = Queue()
            self.full_buffers  = Queue()

            for i in range(self.INITIAL_BUFFER_COUNT):
                self.empty_buffers.put(self.DataPoint(self.nchans))

            self.acq_thread = self.ACQWorker(self, uut)

        def run(self):
            from influxdb import InfluxDBClient
            self.client = InfluxDBClient('localhost', 8086, 'admin', 'password', 'slowmon435')

            self.mdsplus_device = self.mdsplus_device.copy()

            uut = self.mdsplus_device.getUUT()
            wrtd_tickns = float(uut.cC.WRTD_TICKNS)

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

                # start = time.time()
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

                        ch.putRow(3600, samples[i], data_point.time)

                # start = time.time()               
                self.client.write_points(influx_points, time_precision='n')
             
                MDSplus.Event.setevent(event_name)

                self.empty_buffers.put(data_point)

                # end  = time.time()
                # print('putRow and write_points', end - start)

            if self.acq_thread.running:
                self.acq_thread.stop()
        
        class ACQWorker(threading.Thread):

            def __init__(self, data_worker, uut):
                super(_ACQ2106_435SM.SlowMonitorWorker.ACQWorker, self).__init__()
                self.data_worker = data_worker
                self.node_addr = data_worker.mdsplus_device.node.data()
                self.uut = uut

            def stop(self):
                self.running = False

            def run(self):
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.node_addr, 53666))
                s.settimeout(6)
                
                first_sample = True

                self.running = True
                while self.running:
                    try:
                        sample = self.data_worker.empty_buffers.get(block=False)

                    except Empty:
                        print("NO BUFFERS AVAILABLE. MAKING NEW ONE")
                        sample = self.data_worker.DataPoint(self.data_worker.nchans)

                    first_recv = True
                    toread = len(sample.buffer)
                    #print("*toread =", toread)
                    try:
                        view = memoryview(sample.buffer)
                        while toread:
                            nbytes = s.recv_into(view, toread)
                            #print("nbytes =", nbytes)
                            #print("toread =", toread)
                            if first_recv:
                                sample.time = time.time_ns()
                                first_recv = False
                            view = view[nbytes:]  # slicing views is cheap
                            toread -= nbytes

                    except socket.timeout as e:
                        print("Got a timeout.")
                        err = e.args[0]
                        # this next if/else is a bit redundant, but illustrates how the
                        # timeout exception is setup
                        self.data_worker.empty_buffers.put(sample)

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
                            if first_sample:
                                # The very first sample is carried over from previous shot and should be discarded
                                first_sample = False
                                self.data_worker.empty_buffers.put(sample)
                            else:
                                self.data_worker.full_buffers.put(sample)
    
    # The minimum frequency we can operate at                   
    MIN_FREQUENCY = 10000
    # _faster_ than 20kHz will spoil SLOWMON (it will still work, but the average loses 100% data coverage).
    # though, this is more a suggestion.
    MAX_FREQUENCY = 20000

    # These are partial lists of the options we support.
    # For a complete list, consult D-Tacq

    # Trigger Source Options for Signal Highway d0
    TRIG_SRC_OPTS_0 = [
        'ext',          # External Trigger
        'hdmi',         # HDMI Trigger
        'gpg0',         # Gateway Pulse Generator Trigger
        'wrtt0'         # White Rabbit Trigger
    ]

    # Trigger Source Options for Signal Highway d1
    TRIG_SRC_OPTS_1 = [
        'strig',        # Software Trigger
        'hdmi_gpio',    # HDMI General Purpose I/O Trigger
        'gpg1',         # Gateway Pulse Generator Trigger
        'fp_sync',      # Front Panel SYNC
        'wrtt1'         # White Rabbit Trigger
    ]

    def init(self):
        uut = self.getUUT()

        freq = int(self.freq.data())
        # D-Tacq Recommendation: 
        #      - for fast rates, the minimum sample rate is 10kHz.
        #      - for SLOWMON, the max sample rate is 20kHz.
        if freq < self.MIN_FREQUENCY:
            raise MDSplus.DevBAD_PARAMETER(
                "Sample rate should be greater or equal than 10kHz")
        elif freq > self.MAX_FREQUENCY:
            raise MDSplus.DevBAD_PARAMETER(
                "Sample rate should be less or equal than 20kHz for SLOWMON to work correctly (otherwise average would lose %100 data coverage)")

        mode = str(self.trig_mode.data()).lower()
        if mode == 'hard':
            role = 'master'
            trg = 'hard'
        elif mode == 'soft':
            role = 'master'
            trg = 'soft'
        else:
            role = mode.split(":")[0]
            trg  = mode.split(":")[1]

        if self.debug:
            print("Role is %s and %s trigger" % (role, trg))


        src_trg_0 = None
        src_trg_1 = None

        if trg == 'hard':
            trg_dx = 'd0'
            src_trg_0 = 'EXT' # External Trigger
        elif trg == 'soft' or trg == 'automatic':
            trg_dx = 'd1'
            src_trg_1 = 'STRIG' # Soft Trigger
        elif trg in self.TRIG_SRC_OPTS_0:
            trg_dx = 'd0'
            src_trg_0 = trg
        elif trg in self.TRIG_SRC_OPTS_1:
            trg_dx = 'd1'
            src_trg_1 = trg
        elif trg != 'none':
            raise MDSplus.DevBAD_PARAMETER("TRIG_MODE does not contain a valid trigger source")
 
        uut.s0.sync_role = '%s %s TRG:DX=%s' % (role, self.freq.data(), trg_dx)

        # snyc_role will set a default trigger source, we need to override it to the selected trigger source
        # These must be uppercase
        if src_trg_0:
            uut.s0.SIG_SRC_TRG_0 = src_trg_0.upper()

        if src_trg_1:
            uut.s0.SIG_SRC_TRG_1 = src_trg_1.upper()

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


        uut.s1.MEV_PRE = self.presamples.data()
        uut.s1.MEV_POST = self.postsamples.data()
        uut.s1.MEV_MAX = 10

        # Trigger ACQ to starts:
        uut.s0.CONTINUOUS = 1

        # Setting the slow monitoring frequency to 1Hz
        uut.s0.SLOWMON_FS = 1 #Hz

        # Setting Capture/Transient trigger event:
        uut.s1.EVENT0          = 'enable'
        uut.s1.EVENT0_DX       = 'd1'
        uut.s1.EVENT0_SENSE    = 'rising'
        uut.s0.SIG_EVENT_SRC_1 = 'TRG'

        self.running.on = True

        mev_thread = self.MultiEventWorker(self, uut)
        mev_thread.start()

        sm_thread = self.SlowMonitorWorker(self, uut)
        sm_thread.start()

    INIT = init


    def trigger_mev(self):
        uut = self.getUUT()
        uut.s0.soft_trigger
    TRIGGER_MEV = trigger_mev
        
    def stop(self):
        uut = self.getUUT()
        self.running.on = False
        # Trigger ACQ to stop:
        uut.s0.CONTINUOUS = 0
    STOP = stop

    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.factory(self.node.data())
        return uut

    def setChanScale(self, num):
        chan = self.__getattr__('INPUT_%3.3d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT, MDSplus.dVALUE()), chan.OFFSET))
    
    def setTransientChanScale(self, num):
        chan = self.__getattr__('INPUT_%3.3d' % num)
        chan.TRANSIENT.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT, MDSplus.dVALUE()), chan.OFFSET))
    

def assemble(cls):
    cls.parts = list(_ACQ2106_435SM.carrier_parts)
    for i in range(cls.sites * 32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i + 1,),            
                'type': 'SIGNAL', 
                'valueExpr': 'head.setChanScale(%d)' % (i + 1,),
                #'options': ('no_write_model',) # this seems to be broken
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
            {
                 # Capture transient signal goes here:
                'path': ':INPUT_%3.3d:TRANSIENT' % (i + 1,),
                'type': 'SIGNAL',
                'valueExpr': 'head.setTransientChanScale(%d)' % (i + 1,),
                #'options': ('no_write_model',)
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
