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
import numpy as np
import MDSplus
import threading
import Queue
import socket
import time
import inspect


try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

class ACQ2106_MGT(MDSplus.Device):
    """
    D-Tacq ACQ2106_MGT support.

    """

    parts=[
        # The user will need to change the hostname to the relevant hostname/IP.
        {'path':':NODE','type':'text','value':'acq2106_157', 'options':('no_write_shot',)},
        {'path':':SITE','type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':TRIG_MODE','type':'text', 'value': 'role_default', 'options':('no_write_shot',)},
        {'path':':ROLE','type':'text', 'value': 'master', 'options':('no_write_shot',)},
        {'path':':FREQ','type':'numeric', 'value': int(1e7), 'options':('no_write_shot',)},
        {'path':':HW_FILTER','type':'numeric', 'value':0, 'options':('no_write_shot',)},
        {'path':':SEG_LENGTH','type':'numeric', 'value': 2*(2**20), 'options':('no_write_shot',)},
        {'path':':MAX_SEGMENTS','type':'numeric', 'value': 2002, 'options':('no_write_shot',)},
        {'path':':SEG_EVENT','type':'text', 'value': 'MGT_HOST_PULL', 'options':('no_write_shot',)},
        {'path':':TRIG_TIME','type':'numeric', 'options':('write_shot',)},
        {'path':':TRIG_STR','type':'text', 'options':('nowrite_shot',),'valueExpr':"EXT_FUNCTION(None,'ctime',head.TRIG_TIME)"},
        {'path':':RUNNING','type':'any', 'options':('no_write_model',)},
        ]

    debug=None

    trig_types=[ 'hard', 'soft', 'automatic']

    class MDSWorker(threading.Thread):
        NUM_BUFFERS = 20

        def __init__(self,dev):
            super(ACQ2106_MGT.MDSWorker,self).__init__(name=dev.path)
            threading.Thread.__init__(self)

            self.dev = dev.copy()

            self.chans = []
            self.decim = []
            self.uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
            self.nchans = uut.nchan()

            for i in range(self.nchans):
                parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',),
                      'valueExpr':'head.setChanScale(%d)' %(i+1,)})
                parts.append({'path':':INPUT_%2.2d:DECIMATE'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
                parts.append({'path':':INPUT_%2.2d:COEFFICIENT'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
                parts.append({'path':':INPUT_%2.2d:OFFSET'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
                self.chans.append(getattr(self.dev, 'INPUT_%2.2d'%(i+1)))
                self.decim.append(getattr(self.dev, 'INPUT_%2.2d:DECIMATE' %(i+1)).data())

            self.seg_length = self.dev.seg_length.data()
            self.segment_bytes = self.seg_length*self.nchans*np.int16(0).nbytes

            self.empty_buffers = Queue.Queue()
            self.full_buffers = Queue.Queue()

            for i in range(self.NUM_BUFFERS):
                self.empty_buffers.put(bytearray(self.segment_bytes))

            self.device_thread = self.DeviceWorker(self)

        def run(self):
            def lcm(a,b):
                from fractions import gcd
                return (a * b / gcd(int(a), int(b)))

            def lcma(arr):
                ans = 1.
                for e in arr:
                    ans = lcm(ans, e)
                return int(ans)

            if self.dev.debug:
                print("MDSWorker running")

            event_name = self.dev.seg_event.data()

            if self.dev.hw_filter.length > 0:
                dt = 1./self.dev.freq.data() * 2 ** self.dev.hw_filter.data()
            else:
                dt = 1./self.dev.freq.data()

            decimator = lcma(self.decim)

            if self.seg_length % decimator:
                self.seg_length = (self.seg_length // decimator + 1) * decimator

            self.dims = []
            for i in range(self.nchans):
                self.dims.append(MDSplus.Range(0., (self.seg_length-1)*dt, dt*self.decim[i]))

            self.device_thread.start()

            segment = 0
            first = True
            running = self.dev.running
            max_segments = self.dev.max_segments.data()
            while running.on and segment < max_segments:
                try:
                    buf = self.full_buffers.get(block=True, timeout=1)
                except Queue.Empty:
                    continue

                i = 0
                cycle = 1
                for c in self.chans:
                    if c.on:
                        b = buf[i::self.nchans]
                        c.makeSegment(self.dims[i].begin, self.dims[i].ending, self.dims[i], b)
                        self.dims[i] = MDSplus.Range(self.dims[i].begin + self.seg_length*dt, self.dims[i].ending + self.seg_length*dt, dt*self.decim[i])
                    i += 1
                segment += 1
                MDSplus.Event.setevent(event_name)

                self.empty_buffers.put(buf)

            self.dev.trig_time.record = self.device_thread.trig_time - ((self.device_thread.io_buffer_size / np.int16(0).nbytes) * dt)
            self.device_thread.stop()

        class DeviceWorker(threading.Thread):
            running = False

            def __init__(self,mds):
                threading.Thread.__init__(self)
                self.debug = mds.dev.debug
                self.node_addr = mds.dev.node.data()
                self.seg_length = mds.dev.seg_length.data()
                self.segment_bytes = mds.segment_bytes
                self.freq = mds.dev.freq.data()
                self.nchans = mds.nchans
                self.empty_buffers = mds.empty_buffers
                self.full_buffers = mds.full_buffers
                self.trig_time = 0
                self.io_buffer_size = 4096

            def stop(self):
                self.running = False

            def run(self):
                if self.debug:
                    print("DeviceWorker running")

                self.running = True

                first = True

                # trigger time out count initialization:
                rc = acq400_hapi.MgtDramPullClient(self.node_addr)

                try:
                    for buf in rc.get_blocks(16, ncols=(2**22)/16/2, data_size=2):
                        self.full_buffers.put(buf)
                except socket.timeout as e:
                    print("Got a timeout.")
                    err = e.args[0]
                    # this next if/else is a bit redundant, but illustrates how the
                    # timeout exception is setup

                    if err == 'timed out':
                        time.sleep(1)
                        print (' recv timed out, retry later')
                        # continue                  else:
                        print (e)
                        # break
                except socket.error as e:
                    # Something else happened, handle error, exit, etc.
                    print("socket error", e)
                    self.full_buffers.put(buf[:self.segment_bytes-toread])


    def setChanScale(self,num):
        chan=self.__getattr__('INPUT_%2.2d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT,MDSplus.dVALUE()),chan.OFFSET))

    def init(self):
        print('Running init')

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        for ch in range(1, uut.nchan()+1):
            parts.append({'path':':INPUT_%2.2d'%(ch,),'type':'signal','options':('no_write_model','write_once',),
                          'valueExpr':'head.setChanScale(%d)' %(ch,)})
            parts.append({'path':':INPUT_%2.2d:DECIMATE'%(ch,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
            parts.append({'path':':INPUT_%2.2d:COEFFICIENT'%(ch,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
            parts.append({'path':':INPUT_%2.2d:OFFSET'%(ch,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})

        trig_types=[ 'hard', 'soft', 'automatic']
        trg = self.trig_mode.data()

        if trg == 'hard':
            trg_dx = 0
        elif trg == 'automatic':
            trg_dx = 1
        elif trg == 'soft':
            trg_dx = 1

        # The default case is to use the trigger set by sync_role.
        if self.trig_mode.data() == 'role_default':
            uut.s0.sync_role = "{} {}".format(self.role.data(), self.freq.data())
        else:
            # If the user has specified a trigger.
            uut.s0.sync_role = '{} {} TRG:DX={}'.format(self.role.data(), self.freq.data(), trg_dx)

        # Now we set the trigger to be soft when desired.
        if trg == 'soft':
            uut.s0.transient = 'SOFT_TRIGGER=0'
        if trg == 'automatic':
            uut.s0.transient = 'SOFT_TRIGGER=1'

        print('Finished init')

    def capture(self):
        print("Capturing now.")
        uut = acq400_hapi.Acq2106_Mgtdram8(self.node.data())
        uut.s14.mgt_taskset = '1'
        uut.s14.mgt_run_shot = str(int(250 + 2))
        uut.run_mgt()
        print("Finished capture.")
    CAPTURE=capture

    def soft_trigger(self):
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.soft_trigger = 1


    def pull(self):
        print("Starting host pull now.")
        import os
        import sys
        from threading import Thread
        import tempfile
        import subprocess

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('set_abort', '1')
        self.running.on=True
        thread = self.MDSWorker(self)
        thread.start()
    PULL=pull


    def stop(self):
        self.running.on = False
    STOP=stop

