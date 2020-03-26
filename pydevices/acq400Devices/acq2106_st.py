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


import numpy as np
import MDSplus
import threading
# import Queue
from queue import Queue, Empty
import socket
import time
import inspect
import acq400_base


try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

INPFMT = ':INPUT_%3.3d'

class _ACQ2106_ST(acq400_base._ACQ400_ST_BASE):
    """
    D-Tacq ACQ2106 stream support.

    """
    #
    # class MDSWorker(threading.Thread):
    #     NUM_BUFFERS = 20
    #
    #     def __init__(self,dev):
    #         super(_ACQ2106_ST.MDSWorker,self).__init__(name=dev.path)
    #         threading.Thread.__init__(self)
    #
    #         self.dev = dev.copy()
    #
    #         self.chans = []
    #         self.decim = []
    #         # self.nchans = self.dev.sites*32
    #         print("Inside MDSWorker")
    #         uut = acq400_hapi.Acq400(self.dev.node.data())
    #         self.nchans = uut.nchan()
    #
    #         print("Inside MDSWorker2")
    #         for i in range(self.nchans):
    #             self.chans.append(getattr(self.dev, 'INPUT_%3.3d'%(i+1)))
    #             self.decim.append(getattr(self.dev, 'INPUT_%3.3d:DECIMATE' %(i+1)).data())
    #         print("Inside MDSWorker3")
    #         self.seg_length = self.dev.seg_length.data()
    #         self.segment_bytes = self.seg_length*self.nchans*np.int16(0).nbytes
    #
    #         print("Inside MDSWorker4")
    #         self.empty_buffers = Queue()
    #         self.full_buffers  = Queue()
    #         print("Inside MDSWorker5")
    #
    #         for i in range(self.NUM_BUFFERS):
    #             self.empty_buffers.put(bytearray(self.segment_bytes))
    #         print("Inside MDSWorker6")
    #         self.device_thread = self.DeviceWorker(self)
    #
    #     def run(self):
    #         def lcm(a,b):
    #             from fractions import gcd
    #             return (a * b / gcd(int(a), int(b)))
    #
    #         def lcma(arr):
    #             ans = 1.
    #             for e in arr:
    #                 ans = lcm(ans, e)
    #             return int(ans)
    #
    #         print("MDSWorker running")
    #         if self.dev.debug:
    #             print("MDSWorker running")
    #
    #         event_name = self.dev.seg_event.data()
    #
    #         dt = 1./self.dev.freq.data()
    #
    #         decimator = lcma(self.decim)
    #
    #         if self.seg_length % decimator:
    #              self.seg_length = (self.seg_length // decimator + 1) * decimator
    #
    #         self.device_thread.start()
    #
    #         segment = 0
    #         running = self.dev.running
    #         max_segments = self.dev.max_segments.data()
    #         while running.on and segment < max_segments:
    #             try:
    #                 buf = self.full_buffers.get(block=True, timeout=1)
    #             except Empty:
    #                 continue
    #
    #             buffer = np.frombuffer(buf, dtype='int16')
    #             i = 0
    #             for c in self.chans:
    #                 slength = self.seg_length/self.decim[i]
    #                 deltat  = dt * self.decim[i]
    #                 if c.on:
    #                     b = buffer[i::self.nchans*self.decim[i]]
    #                     begin = segment * slength * deltat
    #                     end   = begin + (slength - 1) * deltat
    #                     dim   = MDSplus.Range(begin, end, deltat)
    #                     c.makeSegment(begin, end, dim, b)
    #                 i += 1
    #             segment += 1
    #             MDSplus.Event.setevent(event_name)
    #
    #             self.empty_buffers.put(buf)
    #
    #         self.dev.trig_time.record = self.device_thread.trig_time - ((self.device_thread.io_buffer_size / np.int16(0).nbytes) * dt)
    #         self.device_thread.stop()
    #
    #     class DeviceWorker(threading.Thread):
    #         running = False
    #
    #         def __init__(self,mds):
    #             threading.Thread.__init__(self)
    #             self.debug = mds.dev.debug
    #             self.node_addr = mds.dev.node.data()
    #             self.seg_length = mds.dev.seg_length.data()
    #             self.segment_bytes = mds.segment_bytes
    #             self.freq = mds.dev.freq.data()
    #             self.nchans = mds.nchans
    #             self.empty_buffers = mds.empty_buffers
    #             self.full_buffers = mds.full_buffers
    #             self.trig_time = 0
    #             self.io_buffer_size = 4096
    #
    #         def stop(self):
    #             self.running = False
    #
    #         def run(self):
    #             if self.debug:
    #                 print("DeviceWorker running")
    #
    #             self.running = True
    #
    #             s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #             s.connect((self.node_addr,4210))
    #             s.settimeout(6)
    #
    #             # trigger time out count initialization:
    #             first = True
    #             while self.running:
    #                 try:
    #                     buf = self.empty_buffers.get(block=False)
    #                 except Empty:
    #                     # print("NO BUFFERS AVAILABLE. MAKING NEW ONE")
    #                     buf = bytearray(self.segment_bytes)
    #
    #                 toread =self.segment_bytes
    #                 try:
    #                     view = memoryview(buf)
    #                     while toread:
    #                         nbytes = s.recv_into(view, min(self.io_buffer_size,toread))
    #                         if first:
    #                             self.trig_time = time.time()
    #                             first = False
    #                         view = view[nbytes:] # slicing views is cheap
    #                         toread -= nbytes
    #
    #                 except socket.timeout as e:
    #                     print("Got a timeout.")
    #                     err = e.args[0]
    #                     # this next if/else is a bit redundant, but illustrates how the
    #                     # timeout exception is setup
    #
    #                     if err == 'timed out':
    #                         time.sleep(1)
    #                         print (' recv timed out, retry later')
    #                         continue
    #                     else:
    #                         print (e)
    #                         break
    #                 except socket.error as e:
    #                     # Something else happened, handle error, exit, etc.
    #                     print("socket error", e)
    #                     self.full_buffers.put(buf[:self.segment_bytes-toread])
    #                     break
    #                 else:
    #                     if toread != 0:
    #                         print ('orderly shutdown on server end')
    #                         break
    #                     else:
    #                         self.full_buffers.put(buf)


    def arm(self):
        print("Capturing now.")
        self.running.on=True
        thread = self.MDSWorker(self)
        thread.start()
        # print("Finished capture.")
    ARM=arm

    def stop(self):
        self.running.on = False
    STOP = stop


def assemble(cls):
    cls.parts = list(_ACQ2106_ST.base_parts)
    total_parts = list(_ACQ1001_ST.base_parts) + list(_ACQ1001_ST.st_base_parts)
    cls.parts = total_parts
    for ch in range(1, cls.nchan+1):
        cls.parts.append({'path':INPFMT%(ch,), 'type':'signal','options':('no_write_model','write_once',),
                          'valueExpr':'head.setChanScale(%d)' %(ch,)})
        cls.parts.append({'path':INPFMT%(ch,)+':DECIMATE', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':COEFFICIENT','type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':OFFSET', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
    # cls.parts.append({'path':':RUNNING',     'type':'numeric',                  'options':('no_write_model',)})
    # cls.parts.append({'path':':SEG_LENGTH',  'type':'numeric', 'value': 8000,   'options':('no_write_shot',)})
    # cls.parts.append({'path':':MAX_SEGMENTS','type':'numeric', 'value': 1000,   'options':('no_write_shot',)})
    # cls.parts.append({'path':':SEG_EVENT',   'type':'text',   'value': 'STREAM','options':('no_write_shot',)})
    # cls.parts.append({'path':':TRIG_TIME',   'type':'numeric',                  'options':('write_shot',)})


chan_combos = [8, 16, 24, 32, 40, 48, 64, 80, 96, 128, 160, 192]
class_ch_dict = {}

for channel_count in chan_combos:
    name_str = "ACQ2106_ST_{}".format(str(channel_count))
    class_ch_dict[name_str] = type(name_str, (_ACQ2106_ST,), {"nchan": channel_count})
    assemble(class_ch_dict[name_str])

for key,val in class_ch_dict.items():
        exec("{} = {}".format(key, "val"))

