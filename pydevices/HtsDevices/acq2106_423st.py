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
import MDSplus
from MDSplus import Event,Range
import threading
import Queue
import time
import socket
import math
import numpy as np

try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

class _ACQ2106_423ST(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    32 Channels * number of slots
    Minimum 2Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    Software sample decimation

    Settable segment length in number of samples

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    carrier_parts=[
        {'path':':NODE',        'type':'text',                     'options':('no_write_shot',)},
        {'path':':COMMENT',     'type':'text',                     'options':('no_write_shot',)},
        {'path':':TRIGGER',     'type':'numeric', 'value': 0.0,    'options':('no_write_shot',)},
        {'path':':TRIG_MODE',   'type':'text',    'value': 'hard', 'options':('no_write_shot',)},
        {'path':':EXT_CLOCK',   'type':'axis',                     'options':('no_write_shot',)},
        {'path':':FREQ',        'type':'numeric', 'value': 16000,  'options':('no_write_shot',)},
        {'path':':DEF_DECIMATE','type':'numeric', 'value': 1,      'options':('no_write_shot',)},
        {'path':':SEG_LENGTH',  'type':'numeric', 'value': 8000,   'options':('no_write_shot',)},
        {'path':':MAX_SEGMENTS','type':'numeric', 'value': 1000,   'options':('no_write_shot',)},
        {'path':':SEG_EVENT',   'type':'text',   'value': 'STREAM','options':('no_write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric',                  'options':('write_shot',)},
        {'path':':TRIG_STR',    'type':'text',   'valueExpr':"EXT_FUNCTION(None,'ctime',head.TRIG_TIME)",'options':('nowrite_shot',)},
        {'path':':RUNNING',     'type':'numeric',                  'options':('no_write_model',)},
        {'path':':LOG_FILE',    'type':'text',   'options':('write_once',)},
        {'path':':LOG_OUTPUT',  'type':'text',   'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head,'auto'))",'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",      'options':('no_write_shot',)},
    ]

    data_socket = -1

    trig_types=[ 'hard', 'soft', 'automatic']

    class MDSWorker(threading.Thread):
        NUM_BUFFERS = 20

        def __init__(self,dev):
            super(_ACQ2106_423ST.MDSWorker,self).__init__(name=dev.path)
            threading.Thread.__init__(self)

            self.dev = dev.copy()

            self.chans = []
            self.decim = []
            self.nchans = self.dev.sites*32

            for i in range(self.nchans):
                self.chans.append(getattr(self.dev, 'input_%3.3d'%(i+1)))
                self.decim.append(getattr(self.dev, 'input_%3.3d_decimate' %(i+1)).data())

            self.seg_length = self.dev.seg_length.data()
            segment_bytes = self.seg_length*self.nchans*np.int16(0).nbytes

            self.empty_buffers = Queue.Queue()
            self.full_buffers = Queue.Queue()

            for i in range(self.NUM_BUFFERS):
                self.empty_buffers.put(bytearray(segment_bytes))

            self.device_thread = self.DeviceWorker(self.dev,self.empty_buffers,self.full_buffers)

        def run(self):
            def lcm(a,b):
                from fractions import gcd
                return (a * b / gcd(int(a), int(b)))

            def lcma(arr):
                ans = 1.
                for e in arr:
                    ans = lcm(ans, e)
                return int(ans)

            def truncate(f, n):
                return math.floor(f * 10 ** n) / 10 ** n

            if self.dev.debug:
                print("MDSWorker running")

            event_name = self.dev.seg_event.data()

            dt = 1./self.dev.freq.data()

            decimator = lcma(self.decim)

            if self.seg_length % decimator:
                self.seg_length = (self.seg_length // decimator + 1) * decimator

            self.dims = []
            for i in range(self.nchans):
                self.dims.append(Range(0., truncate((self.seg_length-1)*dt,3), dt*self.decim[i]))

            self.device_thread.start()

            segment = 0
            running = self.dev.running
            max_segments = self.dev.max_segments.data()
            while running.on and segment < max_segments:
                try:
                    buf = self.full_buffers.get(block=True, timeout=1)
                except Queue.Empty:
                    continue

                buffer = np.frombuffer(buf, dtype='int16')
                i = 0
                for c in self.chans:
                    if c.on:
                        b = buffer[i::self.nchans*self.decim[i]]
                        c.makeSegment(self.dims[i].begin, self.dims[i].ending, self.dims[i], b)
                        self.dims[i] = Range(self.dims[i].begin + truncate(self.seg_length*dt,3), self.dims[i].ending + truncate(self.seg_length*dt,3), dt*self.decim[i])
                    i += 1
                segment += 1
                Event.setevent(event_name)

                self.empty_buffers.put(buf)

            self.dev.trig_time.record = self.device_thread.trig_time
            self.device_thread.stop()

        class DeviceWorker(threading.Thread):
            running = False

            def __init__(self,dev,empty_buffers,full_buffers):
                threading.Thread.__init__(self)
                self.debug = dev.debug
                self.node_addr = dev.node.data()
                self.seg_length = dev.seg_length.data()
                self.freq = dev.freq.data()
                self.nchans = dev.sites*32
                self.empty_buffers = empty_buffers
                self.full_buffers = full_buffers
                self.trig_time = 0

            def stop(self):
                self.running = False

            def run(self):
                if self.debug:
                    print("DeviceWorker running")

                self.running = True

                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.node_addr,4210))
                s.settimeout(6)

                segment_bytes = self.seg_length*self.nchans*np.int16(0).nbytes

                # trigger time out count initialization:
                first = True
                while self.running:
                    try:
                        buf = self.empty_buffers.get(block=False)
                    except Queue.Empty:
                        print("NO BUFFERS AVAILABLE. MAKING NEW ONE")
                        buf = bytearray(segment_bytes)
                        
                    toread = segment_bytes
                    try:
                        view = memoryview(buf)
                        while toread:
                            nbytes = s.recv_into(view, min(4096,toread))
                            if first:
                                self.trig_time = time.time()
                                first = False
                            view = view[nbytes:] # slicing views is cheap
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
                        self.full_buffers.put(buf[:segment_bytes-toread])
                        break
                    else:
                        if toread != 0:
                            print ('orderly shutdown on server end')
                            break
                        else:
                            self.full_buffers.put(buf)

    def init(self):
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('set_abort', '1')
        if self.ext_clock.length > 0:
            uut.s0.set_knob('SYS_CLK_FPMUX', 'FPCLK')
            uut.s0.set_knob('SIG_CLK_MB_FIN', '1000000')
        else:
            uut.s0.set_knob('SYS_CLK_FPMUX', 'ZCLK')
        freq = int(self.freq.data())
        uut.s0.set_knob('sync_role', 'master %d TRG:DX=d0' % freq)

        try:
            slots = [uut.s1]
            slots.append(uut.s2)
            slots.append(uut.s3)
            slots.append(uut.s4)
            slots.append(uut.s5)
            slots.append(uut.s6)
        except:
            pass
        for card in range(self.sites):
            coeffs =  map(float, slots[card].AI_CAL_ESLO.split(" ")[3:] )
            offsets =  map(float, uut.s1.AI_CAL_EOFF.split(" ")[3:] )
            for i in range(32):
                coeff = self.__getattr__('input_%3.3d_coefficient'%(card*32+i+1))
                coeff.record = coeffs[i]
                offset = self.__getattr__('input_%3.3d_offset'%(card*32+i+1))
                offset.record = offsets[i]
        if self.trig_mode.data() == 'hard':
            uut.s1.set_knob('trg', '1,0,1')
        else:
            uut.s1.set_knob('trg', '1,1,1')

        self.running.on=True
        thread = self.MDSWorker(self)
        thread.start()
    INIT=init

    def stop(self):
        self.running.on = False
    STOP=stop

    def trig(self):
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('soft_trigger','1')
    TRIG=trig

    def setChanScale(self,num):
        chan=self.__getattr__('INPUT_%3.3d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT,MDSplus.dVALUE()),chan.OFFSET))


def assemble(cls):
    cls.parts = list(_ACQ2106_423ST.carrier_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {'path':':INPUT_%3.3d'%(i+1,),            'type':'SIGNAL', 'valueExpr':'head.setChanScale(%d)' %(i+1,),'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:DECIMATE'%(i+1,),   'type':'NUMERIC','valueExpr':'head.def_decimate',            'options':('no_write_shot',)},
            {'path':':INPUT_%3.3d:COEFFICIENT'%(i+1,),'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:OFFSET'%(i+1,),     'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},
        ]

class ACQ2106_423_1ST(_ACQ2106_423ST): sites=1
assemble(ACQ2106_423_1ST)
class ACQ2106_423_2ST(_ACQ2106_423ST): sites=2
assemble(ACQ2106_423_2ST)
class ACQ2106_423_3ST(_ACQ2106_423ST): sites=3
assemble(ACQ2106_423_3ST)
class ACQ2106_423_4ST(_ACQ2106_423ST): sites=4
assemble(ACQ2106_423_4ST)
class ACQ2106_423_5ST(_ACQ2106_423ST): sites=5
assemble(ACQ2106_423_5ST)
class ACQ2106_423_6ST(_ACQ2106_423ST): sites=6
assemble(ACQ2106_423_6ST)

del(assemble)
