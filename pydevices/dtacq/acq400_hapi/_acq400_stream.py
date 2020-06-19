#!/usr/bin/env python
# Copyright (c) 2020, Massachusetts Institute of Technology All rights reserved.
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

import MDSplus

import numpy
import socket
import sys
if sys.version_info < (3,):
    import Queue as queue
    from fractions import gcd
else:
    import queue
    from math import gcd

import _acq400


class WRITER(_acq400.WRITER):
    """Write data to tree."""

    NUM_BUFFERS = 20
    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.dev = dev.copy() # make run thread safe
        # self.nchan = dev.nchan
        self.nchan = dev.uut_nchan
        # nchan may be different form len(chans)
        self.chans = dev.list_for_all(dev.input_node)
        # cache tree info
        self.freq = dev._freq
        self.dt = 1./self.freq
        self.seg_length = dev._stream_seg_length
        self.event_name = dev._stream_event
        self.decim = dev.list_for_all(dev._input_decimate)
        self.max_samples = dev._trigger_post_samples
        self.t0 = dev._trigger
        # align segment length with all decimators
        dec = 1
        for b in self.decim:
            dec = dec * b // gcd(dec, b)
        overhead = self.seg_length % dec
        if overhead > 0:
            self.seg_length += dec - overhead
        self.range = dev.list_for_all(dev.get_input_range_expr)

        self.queue = queue.Queue()
        self.reader = READER(dev, self)
        # store scaling
        for i, ch in enumerate(self.chans):
            if ch.on:
                ch.setSegmentScale(dev.get_input_scaling_expr(i))

    def write(self):
        """Consume data and write to tree."""
        self.reader.start()
        try:
            max_begin = 0
            while max_begin < self.max_samples and not self.was_aborted:
                try:
                    buf = self.queue.get(block=True, timeout=1)
                except queue.Empty:
                    # check if there can be more
                    if self.was_stopped or not self.reader.is_alive():
                        self.dprint(2, "stopped=%s", self.was_stopped)
                        self.dprint(2, "reader=%s", self.reader.is_alive())
                        return
                    continue
                buffer = numpy.frombuffer(buf, dtype='int16')
                max_end = max_begin + buffer.size // self.nchan
                begin = max_begin * self.dt + self.t0
                end = (max_end-1) * self.dt + self.t0
                for i, ch in enumerate(self.chans):
                    if ch.on:
                        first= max_begin // self.decim[i]
                        last = max_end // self.decim[i] - 1
                        data = buffer[i::self.nchan*self.decim[i]]
                        dim  = self.dev.get_dimension_expr(
                            first, last, self.range[i])
                        ch.makeSegment(begin, end, dim, data)
                max_begin = max_end
                if self.event_name:
                    MDSplus.Event.setevent(self.event_name)
            else:
                self.dprint(2, "SAMPLES=%s", max_begin)
        finally:
            self.exception = self.reader.exception


class READER(_acq400.WORKER):
    """Read data from device and queue for writer."""

    def __init__(self, dev, writer):
        super(READER, self).__init__(dev)
        self.writer = writer
        self.buffer_size = writer.seg_length*writer.nchan*2 # 16bit
        self.max_bytes = writer.max_samples*writer.nchan*2
        self.connect = dev.get_stream()

    def work(self):
        """Read from device and put buffer in queue."""
        recv_into = self.connect()
        tot_bytes = 0
        while (tot_bytes < self.max_bytes
               and self.writer.is_alive()
               and not self.was_aborted):
            buf = bytearray(self.buffer_size)
            toread = self.buffer_size
            view = memoryview(buf)
            try:
                while toread and not self.was_aborted:
                    try:
                        nbytes = recv_into(view, toread)
                    except Exception as e:
                        self.dprint(nbytes, e)
                    except socket.timeout:
                        if toread < self.buffer_size:
                            self.dprint(2, "TIMEOUT: %s", toread)
                        # check if we should wait for more
                        if self.was_stopped:
                            return
                        continue
                    if nbytes:
                        view = view[nbytes:] # slicing views is cheap
                        toread -= nbytes
                    else:
                        self.dprint(2, "NBYTES=0")
                        return
            finally:
                read = self.buffer_size - toread
                if toread:
                    if read > 0:
                        self.writer.queue.put(buf[:read])
                    return
                else:
                    self.writer.queue.put(buf)
        else:
            if self.was_aborted:
                self.dprint(2, "ABORTED")


class CLASS(_acq400.CLASS):
    """Adds STREAM function to ACQ400."""

    # pre is not used but set it to default values
    _trigger_pre_samples = 0

    _WORKER = WRITER

PARTS = _acq400.PARTS[:-1] + [
    {'path': ':STREAM',
    },
    {'path': ':STREAM:SEG_LENGTH',
     'type': 'numeric',
     'options': ('no_write_shot',),
     'filter': int,
     'value': 0x8000,
    },
    {'path': ':STREAM:EVENT',
     'type': 'text',
     'options': ('no_write_shot',),
     'help': "MdsEvent name send on segment arrival or NoData to disable",
     'filter': str,
     'default': None,
    },
]

INPUTS = _acq400.INPUTS

BUILD = _acq400.GENERATE_BUILD(CLASS, PARTS, INPUTS, "ST")

# tests
if __name__ == '__main__':
    import os
    import time
    MDSplus.setenv("MDS_PYDEVICE_PATH", os.path.dirname(__file__))
    MDSplus.setenv("test_path", '/tmp')
    with MDSplus.Tree("test",-1,'new') as t:
        # MDSplus.Device.findPyDevices()['ACQ2106_ST_32'].Add(t, 'DEV')
        d = t.addDevice("DEVICE","ACQ2106_ST_8")
        t.write()
    t.open()
    d.ACTION.record = 'ACTION_SERVER'
    d.UUT.record = "192.168.44.255"
    t.createPulse(1)
    t.shot=1
    t.open()
    d.debug = 5
    try:
        d.init()
        d.arm()
        time.sleep(.1)
        d.soft_trigger()
        time.sleep(.2)
        d.store()
    finally:
        d.deinit()
    print(d.INPUT.CH001.getNumSegments(), d.INPUT.CH001.getSegment(1))
