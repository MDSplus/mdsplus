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


import sys
import threading
import socket
import time
import inspect

import numpy as np
import MDSplus

if MDSplus.version.ispy3:
    from queue import Queue, Empty
else:
    from Queue import Queue, Empty


class _ACQ400_BASE(MDSplus.Device):
    """
    D-Tacq ACQ400 Base parts and methods.

    All other carrier/function combinations use this class as a parent class.
    """

    base_parts = [
        # The user will need to change the hostname to the relevant hostname/IP.
        {'path': ':NODE', 'type': 'text', 'value': 'acq1001_999',
            'options': ('no_write_shot',)},
        {'path': ':SITE', 'type': 'numeric',
            'value': 1, 'options': ('no_write_shot',)},
        {'path': ':TRIG_MODE', 'type': 'text',
            'value': 'role_default', 'options': ('no_write_shot',)},
        {'path': ':ROLE', 'type': 'text', 'value': 'master',
            'options': ('no_write_shot',)},
        {'path': ':FREQ', 'type': 'numeric', 'value': int(
            1e6), 'options': ('no_write_shot',)},
        {'path': ':SAMPLES', 'type': 'numeric', 'value': int(
            1e5), 'options': ('no_write_shot',)},
        {'path': ':INIT_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options': ('no_write_shot',)},
        {'path': ':ARM_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','INIT',51,None),Method(None,'ARM',head))", 'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','INIT',52,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)},
    ]

    trig_types = ['hard', 'soft', 'automatic']

    def setChanScale(self, num):
        chan = self.__getattr__('INPUT_%3.3d' % num)

    def init(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        trig_types = ['hard', 'soft', 'automatic']
        trg = self.trig_mode.data()

        if trg == 'hard':
            trg_dx = 0
        elif trg == 'automatic':
            trg_dx = 1
        elif trg == 'soft':
            trg_dx = 1

        # The default case is to use the trigger set by sync_role.
        if self.trig_mode.data() == 'role_default':
            uut.s0.sync_role = "%s %s" % (self.role.data(), self.freq.data())
        else:
            # If the user has specified a trigger.
            uut.s0.sync_role = '%s %s TRG:DX=%s' % (
                self.role.data(), self.freq.data(), trg_dx)

        # Now we set the trigger to be soft when desired.
        if trg == 'soft':
            uut.s0.transient = 'SOFT_TRIGGER=0'
        if trg == 'automatic':
            uut.s0.transient = 'SOFT_TRIGGER=1'

        uut.s0.transient = "POST=%s" % (self.samples.data())

    INIT = init


class _ACQ400_ST_BASE(_ACQ400_BASE):

    """
    A sub-class of _ACQ400_BASE that includes classes for streaming data
    and the extra nodes for streaming devices.
    """

    st_base_parts = [
        {'path': ':RUNNING',     'type': 'numeric',
            'options': ('no_write_model',)},
        {'path': ':SEG_LENGTH',  'type': 'numeric',
            'value': 8000,   'options': ('no_write_shot',)},
        {'path': ':MAX_SEGMENTS', 'type': 'numeric',
            'value': 1000,   'options': ('no_write_shot',)},
        {'path': ':SEG_EVENT',   'type': 'text',
            'value': 'STREAM', 'options': ('no_write_shot',)},
        {'path': ':TRIG_TIME',   'type': 'numeric',
            'options': ('write_shot',)}
    ]

    def arm(self):
        self.running.on = True
        thread = self.MDSWorker(self)
        thread.start()
    ARM = arm

    def stop(self):
        self.running.on = False
    STOP = stop

    class MDSWorker(threading.Thread):
        NUM_BUFFERS = 20

        def __init__(self, dev):
            super(_ACQ400_ST_BASE.MDSWorker, self).__init__(name=dev.path)
            import acq400_hapi
            threading.Thread.__init__(self)
            self.dev = dev.copy()

            self.chans = []
            self.decim = []
            # self.nchans = self.dev.sites*32
            uut = acq400_hapi.Acq400(self.dev.node.data())
            self.nchans = uut.nchan()

            for i in range(self.nchans):
                self.chans.append(getattr(self.dev, 'INPUT_%3.3d' % (i+1)))
                self.decim.append(
                    getattr(self.dev, 'INPUT_%3.3d:DECIMATE' % (i+1)).data())
            self.seg_length = self.dev.seg_length.data()
            self.segment_bytes = self.seg_length*self.nchans*np.int16(0).nbytes

            self.empty_buffers = Queue()
            self.full_buffers = Queue()

            for i in range(self.NUM_BUFFERS):
                self.empty_buffers.put(bytearray(self.segment_bytes))
            self.device_thread = self.DeviceWorker(self)

        def run(self):
            def lcm(a, b):
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

            dt = 1./self.dev.freq.data()

            decimator = lcma(self.decim)

            if self.seg_length % decimator:
                self.seg_length = (self.seg_length //
                                   decimator + 1) * decimator

            self.device_thread.start()

            segment = 0
            running = self.dev.running
            max_segments = self.dev.max_segments.data()
            while running.on and segment < max_segments:
                try:
                    buf = self.full_buffers.get(block=True, timeout=1)
                except Empty:
                    continue

                buffer = np.frombuffer(buf, dtype='int16')
                i = 0
                for c in self.chans:
                    slength = self.seg_length/self.decim[i]
                    deltat = dt * self.decim[i]
                    if c.on:
                        b = buffer[i::self.nchans*self.decim[i]]
                        begin = segment * slength * deltat
                        end = begin + (slength - 1) * deltat
                        dim = MDSplus.Range(begin, end, deltat)
                        c.makeSegment(begin, end, dim, b)
                    i += 1
                segment += 1
                MDSplus.Event.setevent(event_name)

                self.empty_buffers.put(buf)

            self.dev.trig_time.record = self.device_thread.trig_time - \
                ((self.device_thread.io_buffer_size / np.int16(0).nbytes) * dt)
            self.device_thread.stop()

        class DeviceWorker(threading.Thread):
            running = False

            def __init__(self, mds):
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

                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.node_addr, 4210))
                s.settimeout(6)

                # trigger time out count initialization:
                first = True
                while self.running:
                    try:
                        buf = self.empty_buffers.get(block=False)
                    except Empty:
                        buf = bytearray(self.segment_bytes)

                    toread = self.segment_bytes
                    try:
                        view = memoryview(buf)
                        while toread:
                            nbytes = s.recv_into(
                                view, min(self.io_buffer_size, toread))
                            if first:
                                self.trig_time = time.time()
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
                        self.full_buffers.put(buf[:self.segment_bytes-toread])
                        break
                    else:
                        if toread != 0:
                            print ('orderly shutdown on server end')
                            break
                        else:
                            self.full_buffers.put(buf)


class _ACQ400_TR_BASE(_ACQ400_BASE):
    """
    A child class of _ACQ400_BASE that contains the specific methods for
    taking a transient capture.
    """

    def arm(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())
        shot_controller = acq400_hapi.ShotController([uut])
        shot_controller.run_shot()
    ARM = arm

    def store(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())
        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d' % (ii+1)))

        uut.fetch_all_calibration()
        eslo = uut.cal_eslo[1:]
        eoff = uut.cal_eoff[1:]
        channel_data = uut.read_channels()

        for ic, ch in enumerate(self.chans):
            if ch.on:
                ch.putData(channel_data[ic])
                ch.EOFF.putData(float(eoff[ic]))
                ch.ESLO.putData(float(eslo[ic]))
                expr = "%s * %f + %f" % (ch, ch.ESLO, ch.EOFF)

                ch.CAL_INPUT.putData(MDSplus.Data.compile(expr))
    STORE = store


def int_key_chan(elem):
    return int(elem.split('_')[2])


def print_generated_classes(class_dict):
    print("# public classes created in this module")
    key1 = None
    for key in sorted(class_dict.keys(), key=int_key_chan):
        if key1 is None:
            key1 = key
        print("# %s" % (key))
    print(key1)
    for p in class_dict[key1].parts:
        print(p)


INPFMT3 = ':INPUT_%3.3d'
#INPFMT2 = ':INPUT_%2.2d'

ACQ2106_CHANNEL_CHOICES = [8, 16, 24, 32, 40, 48, 64, 80, 96, 128, 160, 192]
ACQ1001_CHANNEL_CHOICES = [8, 16, 24, 32, 40, 48, 64]


def assemble(cls):
    inpfmt = INPFMT3
# probably easier for analysis code if ALWAYS INPUT_001
#    inpfmt = INPFMT2 if cls.nchan < 100 else INPFMT3
    for ch in range(1, cls.nchan+1):
        cls.parts.append({'path': inpfmt % (ch,), 'type': 'signal', 'options': ('no_write_model', 'write_once',),
                          'valueExpr': 'head.setChanScale(%d)' % (ch,)})
        cls.parts.append({'path': inpfmt % (
            ch,)+':DECIMATE', 'type': 'NUMERIC', 'value': 1, 'options': ('no_write_shot')})
        cls.parts.append({'path': inpfmt % (ch,)+':COEFFICIENT',
                          'type': 'NUMERIC', 'value': 1, 'options': ('no_write_shot')})
        cls.parts.append({'path': inpfmt % (
            ch,)+':OFFSET', 'type': 'NUMERIC', 'value': 1, 'options': ('no_write_shot')})
        cls.parts.append({'path': inpfmt % (
            ch,)+':EOFF', 'type': 'NUMERIC', 'value': 1, 'options': ('no_write_shot')})
        cls.parts.append({'path': inpfmt % (
            ch,)+':ESLO', 'type': 'NUMERIC', 'value': 1, 'options': ('no_write_shot')})
        cls.parts.append(
            {'path': inpfmt % (ch,)+':CAL_INPUT', 'type': 'signal'})
    return cls


def create_classes(base_class, root_name, parts, channel_choices):
    my_classes = {}
    for nchan in channel_choices:
        class_name = "%s_%s" % (root_name, nchan)
        my_parts = list(parts)
        my_classes[class_name] = assemble(
            type(class_name, (base_class,), {"nchan": nchan, "parts": my_parts}))
        # needed because of lockout in mdsplus code file
        # python/MDSplus/tree.py function findPyDevices
        my_classes[class_name].__module__ = base_class.__module__
    return my_classes
