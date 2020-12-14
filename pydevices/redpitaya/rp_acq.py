#!/usr/bin/env python
"""RedPitaya-Arbitrary-Waveform-Generator for default image using scpi.

Depends on rp.pr_scpi.
"""

import json
import os
import sys
import socket
import struct
import threading
import time

import numpy
import MDSplus

if sys.version_info < (3,):
    _str = str
    _bytes = bytes
    import urllib2 as urllib
else:
    _str = bytes.decode
    _bytes = str.encode
    import urllib.request as urllib


class rp_acq:
    """Use Stream manager to offload data."""

    max_rate = 125000000
    app_name = 'streaming_manager'
    ws_url = 'ws://%s:80/wss'
    start_app_url = 'http://%s:80/bazaar?start=' + app_name
    stop_app_url = 'http://%s:80/bazaar?stop=' + app_name

    @classmethod
    def _stop(cls, ip):
        return urllib.urlopen(cls.stop_app_url % ip).code

    @classmethod
    def _start(cls, ip):
        return urllib.urlopen(cls.start_app_url % ip).code

    class Socket(socket.socket):
        """Data transfer protocol."""

        PACK_ID = b'STREAMpackIDv1.0'
        PACK_LEN = 16

        def __enter__(self):
            """Enable with."""
            return self

        def __exit__(self, *a):
            """Cleanup with."""
            self.close()

        def packs(self):
            """Receive and yield packs."""
            pack_id = self.recv(self.PACK_LEN)
            while pack_id == self.PACK_ID:
                header = self.recv(52-16)
                idx, lost, osc, size, ch1, ch2, res = struct.unpack(
                    b'<QQiiiii', header)
                # print(idx, lost, osc, size, ch1, ch2, res)
                fmt, sze = ('<i2', 2) if res == 16 else ('<i1', 1)
                out = [None, None]
                for i, ch in enumerate((ch1, ch2)):
                    if ch:
                        buf = bytearray(ch)
                        view = memoryview(buf)
                        while view:
                            read = self.recv_into(view, view.shape[0])
                            view = view[read:]
                        out[i] = buf
                yield (
                    numpy.frombuffer(out[0], fmt),
                    numpy.frombuffer(out[1], fmt),
                    idx*ch1/sze, ch1/sze, osc*8,
                )
                pack_id = self.recv(self.PACK_LEN)

        def _start_streaming_manager(self, ip, port, rate, ch):
            try:
                from websocket import create_connection
            except ImportError:
                print(("ERROR: python package 'websocket'"
                       " required to start streaming_manager."))
                return
            try:
                rp_acq._start(ip)
            except Exception:
                rp_acq._stop(ip)
                rp_acq._start(ip)
            self.ws = create_connection(rp_acq.ws_url % ip)
            self.ws.recv()
            stopcmd = {"parameters": {
                "SS_START": {"value": 0},
                "in_command": {"value": "send_all_params"}}}
            self.ws.send(json.dumps(stopcmd))
            config = {"parameters": {
                "SS_IP_ADDR": {"value": ip},
                "SS_PORT_NUMBER": {"value": port},
                "SS_PROTOCOL": {"value": 1},
                "SS_RESOLUTION": {"value": 2},
                "SS_RATE": {"value": rate},
                "SS_CHANNEL": {"value": ch},
                "SS_USE_FILE": {"value": 0},
                "SS_SAMPLES": {"value": 0},
                "SS_FORMAT": {"value": 0},
                "SS_START": {"value": 0},
                "in_command": {"value": "send_all_params"}}}
            self.ws.send(json.dumps(config))

            startcmd = {"parameters": {
                "SS_START": {"value": 1},
                "in_command": {"value": "send_all_params"}}}
            self.ws.send(json.dumps(startcmd))
            time.sleep(10)

        def connect(self, host, port=8900, rate=625, ch=3):
            """Start stream and connect."""
            ip, port = socket.getaddrinfo(host, port)[0][-1]
            self._start_streaming_manager(ip, port, rate, ch)
            super(rp_acq.Socket, self).connect((ip, port))

        def close(self):
            """Close websocket."""
            try:
                if self.ws:
                    self.ws.close()
            finally:
                super(rp_acq.Socket, self).close()

    def __init__(self, host, port=8900):
        self._host = host
        self._port = port

    def connect(self, rate=625, ch=3):
        """Connect to Stream port."""
        sock = self.Socket()
        sock.connect(self._host, self._port, rate, ch)
        return sock


@MDSplus.with_mdsrecords
class RP_ACQ(MDSplus.Device):
    """RedPitaya-Arbitrary-Waveform-Generator for default image using scpi."""

    parts = [
        {'path': ':ACTIONSERVER',
         'options': ('no_write_shot', 'write_once'),
         'type': 'TEXT',
         },
        {'path': ':ACTIONSERVER:INIT',
         'options': ('no_write_shot', 'write_once'),
         'type': 'ACTION',
         'valueExpr': ('Action('
                       'Dispatch(head.ACTIONSERVER, "INIT", 51),'
                       'Method(None, "init", head))'),
         },
        {'path': ':ACTIONSERVER:DEINIT',
         'options': ('no_write_shot', 'write_once'),
         'type': 'ACTION',
         'valueExpr': ('Action('
                       'Dispatch(head.ACTIONSERVER, "DEINIT", 51),'
                       'Method(None, "deinit", head))'),
         },
        {'path': ':HOST',
         'options': ('no_write_shot',),
         'type': 'TEXT',
         'filter': str,
         },
        {'path': ':DECIM',
         'options': ('no_write_shot',),
         'type': 'NUMERIC',
         'filter': int,
         'default': 625,
         },
        {'path': ':IN1',
         'options': ('no_write_model', 'write_once'),
         'type': 'SIGNAL',
         },
        {'path': ':IN2',
         'options': ('no_write_model', 'write_once'),
         'type': 'SIGNAL',
         },
    ]

    class Store(threading.Thread):
        """Store stream data to tree."""

        def __init__(self, dev):
            threading.Thread.__init__(self, name="RP_ACQ.Store")
            self.dev = dev.copy()
            self._ready = threading.Event()
            self._stopped = threading.Event()

        def stop(self):
            """Request stop."""
            self._stopped.set()

        def start_waited(self):
            """Wait until thread is started."""
            self.start()
            while not self._ready.is_set():
                self._ready.wait(1)

        def run(self):
            """Read Stream and store."""
            self.dev.tree.open()
            c1, c2 = self.dev.in1.on, self.dev.in2.on
            ch = (3 if c2 else 1) if c1 else (2 if c2 else 0)
            with rp_acq(self.dev._host).connect(self.dev._decim, ch) as con:
                self.dev.dprint(1, "ready")
                self._ready.set()
                i0 = 0
                for d1, d2, i0, l, dt in con.packs():
                    i1 = i0 + l-1
                    t0, t1 = i0*dt, i1*dt
                    dim = MDSplus.Dimension(
                         MDSplus.Window(i0, i1, 0),
                         MDSplus.Range(None, None, MDSplus.Int64(dt)),
                    )
                    if c1:
                        self.dev.in1.makeSegment(t0, t1, dim, d1)
                    if c2:
                        self.dev.in2.makeSegment(t0, t1, dim, d2)
                    if self._stopped.is_set():
                        self.dev.dprint(1, "stopped")
                        return

    def get_input(self, n):
        """Return node of input n or member."""
        return getattr(self, "in%d" % n)

    def init(self):
        """Initialize device - configure and enable outputs."""
        self.deinit()
        stream = self.Store(self)
        self.persistent = stream
        stream.start_waited()

    def deinit(self):
        """Deinitialize device - disable outputs."""
        stream = self.persistent
        if stream is not None:
            stream.stop()
            stream.join()
            self.persistent = None


def main(args=None):
    """Run some tests."""
    if args is None:
        args = sys.argv[1:]
    ip = args[0] if args else 'RP-F0432C'
    ip = socket.getaddrinfo(ip, 0)[0][-1][0]
    offslo = (-20, 1/7348.98), (--25, 1/7428.72)
    from matplotlib.pyplot import plot
    if 0:
        calib = args[1] if len(args) > 1 else False
        with rp_acq(ip).connect() as con:
            start = end = 0
            for pack in con.packs():
                ch1, ch2 = pack[:2]
                read = ch1.size
                end += read
                if calib:
                    print(
                        numpy.mean(ch1),
                        numpy.mean(ch2),
                    )
                plot(range(start, end), (ch1+offslo[0][0])*offslo[0][1])
                plot(range(start, end), (ch2+offslo[1][0])*offslo[1][1])
                start = end
                if end > 1e5:
                    break
    else:
        srcdir = os.path.realpath(os.path.dirname(__file__))
        sys.path.insert(0, srcdir)
        try:
            MDSplus.setenv('MDS_PYDEVICE_PATH', srcdir)
            MDSplus.setenv('test_path', '/tmp')
            with MDSplus.Tree('test', -1, 'NEW') as t:
                d = t.addDevice('RP_ACQ', 'RP_ACQ')
                t.write()
            t.open()
            d.HOST.record = ip
            d.DECIM.record = 1250
            for i, scl in enumerate(offslo):
                node = d.get_input(i+1)
                node.no_write_model = False
                node.setSegmentScale(
                    MDSplus.MULTIPLY(
                        MDSplus.ADD(
                            MDSplus.dVALUE(),
                            MDSplus.Int32(scl[0])
                        ),
                        MDSplus.Float32(scl[1])
                    )
                )
                node.no_write_model = True
            t.createPulse(1)
            t.open(shot=1)
            d.debug = 1
            d.init()
            time.sleep(3)
            d.deinit()
            sig = d.IN1.record
            x, y = sig.dim_of().data(), sig.data()
            plot(x, y)
        finally:
            del(sys.path[0])


if __name__ == '__main__':
    sys.exit(main())
