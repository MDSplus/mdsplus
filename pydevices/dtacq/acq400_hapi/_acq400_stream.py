#
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

import math
import numpy
import socket
import sys
if sys.version_info < (3,):
    import Queue as queue
else:
    import queue

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
            dec = dec * b // math.gcd(dec, b)
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
        max_begin = 0
        while max_begin < self.max_samples and not self.was_aborted:
            try:
                buf = self.queue.get(block=True, timeout=1)
            except queue.Empty:
                # check if there can be more
                if self.was_stopped or not self.reader.is_alive():
                    return
                continue
            max_end = max_begin + self.seg_length
            buffer = numpy.frombuffer(buf, dtype='int16')
            begin = max_begin * self.dt + self.t0
            end = (max_end-1) * self.dt + self.t0
            self.seg_length
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


class READER(_acq400.WORKER):
    """Read data from device and queue for writer."""

    def __init__(self, dev, writer):
        super(READER, self).__init__(dev)
        self.writer = writer
        self.buffer_size = writer.seg_length*writer.nchan*2 # 16bit
        self.connect = dev._connector()

    def work(self):
        """Read from device and put buffer in queue."""
        recv_into = self.connect()
        try:
            self.dprint(1, "Stream-Worker running")
            while self.writer.is_alive() and not self.was_aborted:
                buf = bytearray(self.buffer_size)
                toread = self.segment_bytes
                view = memoryview(buf)
                try:
                    while toread and not self.was_aborted:
                        try:
                            nbytes = recv_into(view, toread)
                        except socket.timeout:
                            # check if we should wait for more
                            if self.was_stopped:
                                raise
                            continue
                        if nbytes:
                            view = view[nbytes:] # slicing views is cheap
                            toread -= nbytes
                        else:
                            break
                finally:
                    read = self.buffer_size - toread
                    if toread:
                        if read > 0:
                            self.writer.queue.put(buf[:read])
                        break
                    else:
                        self.writer.queue.put(buf)
        except Exception as e:
            self.exception = e
            self.dprint(0, "Reader error %s", e)
        finally:
            self.writer.stop()

class CLASS(_acq400.CLASS):
    """Adds STREAM function to ACQ400."""

    # pre is not used but set it to default values
    _trigger_pre_samples = 0

    _WORKER = WRITER

    def _connector(self, port=4210):
        """Allow to induce a mockup connection."""
        host = self._uut
        def connect():
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((host, port))
            sock.settimeout(1)
            def recv_into(self, view, toread):
                return sock.recv_into(view, toread)
            return recv_into
        return connect


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
