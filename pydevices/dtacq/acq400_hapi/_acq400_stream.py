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
import time
if sys.version_info < (3,):
    import Queue as queue
    from fractions import gcd
else:
    import queue
    from math import gcd

import _acq400

class STREAM:
    """Adds get_stream to CLASS."""

    def get_stream(self):
        """Return a connect method returning a recv_into methods."""
        if self.use_mockup:
            return self._mockup_stream()
        else:
            return self._acq400_stream(self._uut)

    @staticmethod
    def _acq400_stream(host, port=4210):
        sock = socket.socket()
        sock.connect((host, port))
        sock.settimeout(1)
        return sock

    @staticmethod
    def _mockup_stream(*a):
        class socket_mockup:
            @staticmethod
            def recv_into(view, toread):
                time.sleep(.065)
                return min(toread, 65535)
            @staticmethod
            def close():
                pass
        return socket_mockup

class WRITER(_acq400.WRITER):
    """Write data to tree."""

    def get_state(self):
        """Work around state issue to determine if device is armed."""
        statestr = self.uut.s0.sr('state;shot').split('\n')
        if len(statestr) > 1:
            self.dev.dprint(5, "STATE %s", statestr[0])
            return int(statestr[0].split(' ', 1)[0])
        else:
            self.dev.dprint(5, "STATE None")
            return 0

    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.dev = dev.copy() # make run thread safe
        self.uut = dev.acq400()
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
        # wait for device to be armed
        state = self.get_state()
        while state == 0 or state > 4:
            time.sleep(.1)
            state = self.get_state()

    def stop(self):
        """Stop the stream."""
        self.reader.stop()
        super(WRITER, self).stop()

    def abort(self):
        """Stop the stream."""
        self.reader.abort()
        super(WRITER, self).abort()

    def write(self):
        """Consume data and write to tree."""
        self.reader.start()
        for i, ch in enumerate(self.chans):
            if ch.on:
                ch.setSegmentScale(self.dev.get_input_scaling_expr(i))
        try:
            max_begin = 0
            while max_begin < self.max_samples and not self.was_aborted:
                try:
                    buf = self.queue.get(block=True, timeout=1)
                except queue.Empty:
                    # check if there can be more
                    if self.was_stopped:
                        self.dprint(2, "STOPPED")
                        return
                    if not self.reader.is_alive():
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
            self.reader.abort()
            self.reader.join()
            self.exception = self.reader.exception


class READER(_acq400.WORKER):
    """Read data from device and queue for writer."""

    def __init__(self, dev, writer):
        super(READER, self).__init__(dev)
        self.writer = writer
        self.buffer_size = writer.seg_length*writer.nchan*2 # 16bit
        self.max_bytes = writer.max_samples*writer.nchan*2
        self.stream = dev.get_stream()  # this arm the device

    def work(self):
        """Read from device and put buffer in queue."""
        try:
            tot_bytes = 0
            while (tot_bytes < self.max_bytes):
                if self.was_aborted:
                    self.dprint(2, "ABORTED")
                    return
                if not self.writer.is_alive():
                    self.dprint(2, "WRITER DIED")
                    return
                self.dprint(3, "READING %dB / %dB", tot_bytes, self.max_bytes)
                buf = bytearray(self.buffer_size)
                toread = self.buffer_size
                view = memoryview(buf)
                try:
                    while toread:
                        if self.was_aborted:
                            self.dprint(2, "ABORTED")
                            return
                        try:
                            nbytes = self.stream.recv_into(view, toread)
                        except socket.timeout:
                            if toread < self.buffer_size:
                                self.dprint(2, "TIMEOUT: %s", toread)
                            else:
                                self.dprint(4, "TIMEOUT")
                            # check if we should wait for more
                            if self.was_stopped:
                                self.dprint(2, "STOPPED")
                                return
                            continue
                        except Exception as e:
                            self.dprint(0, "NBYTES=%d %s", nbytes, e)
                        if nbytes:
                            view = view[nbytes:] # slicing views is cheap
                            toread -= nbytes
                        else:
                            self.dprint(2, "NBYTES=0")
                            return
                finally:
                    read = self.buffer_size - toread
                    tot_bytes += read
                    if toread:
                        if read > 0:
                            self.writer.queue.put(buf[:read])
                        self.dprint(2, "READ %dB / %dB", read, self.buffer_size)
                        return
                    else:
                        self.writer.queue.put(buf)
        finally:
            self.stream.close()


class CLASS(_acq400.CLASS, STREAM):
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
    import sys
    use_mockup = len(sys.argv) < 2
    MDSplus.setenv("MDS_PYDEVICE_PATH", os.path.dirname(__file__))
    MDSplus.setenv("test_path", '/tmp')
    with MDSplus.Tree("test", -1, 'new') as t:
        # MDSplus.Device.findPyDevices()['ACQ2106_ST_32'].Add(t, 'DEV')
        d = t.addDevice("DEVICE","ACQ2106_ST_8")
        t.write()
    t.open()
    d.ACTION.record = 'ACTION_SERVER'
    d.debug = 5
    if use_mockup:
        d.use_mockup = True
    else:
        d.UUT.record = sys.argv[1]
    d.TRIGGER.MODE.record = 'soft'
    t.createPulse(1)
    t.open(shot=1)
    try:
        d.init()
        d.arm()
        time.sleep(1)
        d.soft_trigger()
        time.sleep(5)
        d.store()
    finally:
        d.deinit()
    print(d.INPUT.CH001.getNumSegments(), d.INPUT.CH001.getSegment(1))
