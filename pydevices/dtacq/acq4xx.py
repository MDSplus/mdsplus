#!/usr/bin/env python
"""D-TACQ gen 4 driver.

Author: Timo Schroeder

Todo:
----
    - server side demuxing for normal opperation not reliable, why?
    - properly check if the clock is in range
    + ACQ480: maxADC=80MHz, maxBUS=50MHz, minBUS=10MHz, maxFPGA_FIR=25MHz
"""

import os
import sys
import time
import ctypes
import threading
import traceback
import socket
import inspect
import json
import re

import numpy
_string = (type(b""), type(u""))
if sys.version_info < (3,):
    import Queue as queue
    _zipped = zip
    ABC = object

    def _nop(arg):
        return arg
    s = b = abstractmethod = abstractproperty = _nop
else:
    import queue
    xrange = range
    def _zipped(*a, **k): return list(zip(*a, **k))

    def s(b):
        """Convert to str if bytes."""
        return b if isinstance(b, str) else b.decode('utf-8')

    def b(s):
        """Convert to bytes if str."""
        return s if isinstance(s, bytes) else s.encode('utf-8')
    from abc import ABC, abstractmethod, abstractproperty


try:
    import hashlib
except ModuleNotFoundError:
    hashlib = None


class _cached_property(object):
    def __init__(self, method):
        self.method = method

    def __call__(self, method):
        # allows to override prop with const; used for buflen in Stream_mgt
        return self.method

    def __get__(self, inst, cls):
        if inst is None:
            return self.method
        if self.method not in inst.__dict__:
            inst.__dict__[self.method] = self.method.__get__(inst)()
        return inst.__dict__[self.method]


def _hasitems(obj):
    return not isinstance(obj, _string) and hasattr(obj, '__getitem__')


run_test = False
plot_channel = -1
debug = 0

if plot_channel >= 0:
    try:
        import matplotlib.pyplot as pp
    except ModuleNotFoundError:
        plot_channel = -1


def dprint(fmt, *args):
    """Print debug message with timestamp."""
    now = time.time()
    line = str(fmt) if not args else (fmt % args)
    sys.stderr.write('%.3f: %s\n' % (now, line))


_state_port = 2235
_bigcat_port = 4242
_aggr_port = 4210  # used for streaming...
# requires rate to be sufficiently low for port ~32 MB/s e.g. 8CH @ 2MHz
# or subset of channels via --subset (acq480) or channel mask (acq425)
# e.g. acq480 for 4 channels, 3..6:
# > echo 'STREAM_OPTS="--subset=3,4"'>>/mnt/local/sysconfig/acq400_streamd.conf
_sys_port = 4220
_gpgw_port = 4541
_gpgr_port = 4543
_data_port = 53000
_mgt_log_port = 53990
_ao_oneshot_cs = 54200  # sha1sum
_ao_oneshot = 54201
_ao_oneshot_re = 54202
_zclk_freq = 33333300


class _es_marker:
    r"""Event marker information.

    hexdump
    0000000 llll hhhh f154 aa55 LLLL HHHH f154 aa55
    0000010 0000 0000 f15f aa55 LLLL HHHH f15f aa55
    *
    w00     llll   current memory_row_index (low word)
    w01     hhhh   current memory_row_index (high word)
    w02,06  f154   marker for master module
    w03:04: aa55   static marker
    w04:08: LLLL   current sample_clock_count (low word)
    w05:08: HHHH   current sample_clock_count (high word)
    w10:04: f15f   marker for slave module
    -----------------------------------------------
    hexdump -e '"%08_ax %08x |%08x %08x |%08x\n"'
    00000000 IIIIIIIII |aa55f154 CCCCCCCC |aa55f154
    00000010 000000000 |aa55f15f CCCCCCCC |aa55f15f
    *
    IIIIIIII:	sample_count	<4u
    CCCCCCCC:	clock_count	<4u
    """

    index_l = slice(0, 1, 1)
    index_h = slice(1, 2, 1)
    index = slice(0, 2, 1)
    master = slice(2, 8, 4)
    static = slice(3, None, 4)
    count_l = slice(4, None, 8)
    count_h = slice(5, None, 8)
    count = slice(4, 6, 1)
    slave = slice(10, None, 4)  # always has all set

    class uint16:
        static = 0xaa55
        mask = 0xf150
        event0 = 0xf151
        event1 = 0xf152
        ev_rgm = 0xf154
        all = 0xf15f

    class int16:
        static = 0xaa55 - (1 << 16)


class AO_MODE:
    """Define AO modes."""

    CONTINUOUS = 0
    ONE_SHOT = 1
    ONE_SHOT_REARM = 2

# -----------------
#  nc base classes
# -----------------


class _nc(object):
    """Core n-etwork c-onnection to the DTACQ appliance.

    This class provides the methods for sending and receiving information/data
    through a network socket. All communication with the D-TACQ devices will
    require this communication layer
    """

    _chain = None
    _server = None
    __stop = None
    @staticmethod
    def _tupletostr(value):
        """Remove the spaces between elements that is inherent to tuples."""
        return ','.join(map(str, tuple(value)))

    def __init__(self, server):
        """Init with server, a tuple of host & port.

        For example: ('acq2106_064', 4220)
        """
        self._server = server
        self._nc__stop = threading.Event()

    def __str__(self):
        """Return a readable tag of the server name & port.

        This is what appears when you, for example, use print()
        """
        name = self.__class__.__name__
        return "%s(%s:%d)" % (name, self._server[0], self._server[1])

    def __repr__(self):
        """Return the name in, for example, the interactive shell."""
        return str(self)

    def chain_start(self):
        if self._chain is not None:
            raise Exception('chain already started')
        self._chain = []

    def chain_abort(self):
        self._chain = None

    def chain_send(self):
        chain, self._chain = self._chain, None
        self._com('\n'.join(chain))

    @property
    def sock(self):
        """Create a socket object and return it."""
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect(self._server)
        return sock

    @property
    def on(self):
        return not self._nc__stop.is_set()

    def stop(self):
        self._nc__stop.set()

    def lines(self, timeout=60):
        sock = self.sock
        sock.settimeout(timeout)

        def gen():
            while self.on:
                c = sock.recv(1)
                if not c:
                    break
                yield c
                if c == b'\n':
                    break

        try:
            while self.on:
                line = b''.join(gen())
                if not line:
                    break
                yield line
        finally:
            sock.close()

    def _com(self, cmd, ans=False, timeout=None, dbg=[None, None]):
        if _hasitems(cmd):
            cmd = '\n'.join(cmd)
        cmd = cmd.strip()
        if not (self._chain is None or ans):
            self._chain.append(cmd)
            return
        if isinstance(self._server, (_nc,)):
            return self._server._com(cmd)
        dbg[0] = cmd
        if cmd.startswith('_'):
            traceback.print_stack()
        sock = self.sock
        def recvstr(): return s(sock.recv(1024))
        try:
            if debug >= 3:
                dprint('cmd: %s' % cmd)
            sock.sendall(b(cmd))  # sends the command
            sock.shutdown(socket.SHUT_WR)
            sock.settimeout(5 if timeout is None else timeout)
            dbg[1] = ''.join(iter(recvstr, '')).strip()
        finally:
            sock.close()
        if ans:
            return dbg[1]

    def __call__(self, cmd, value=None, timeout=5, ans=False):
        """Block of code associated with the self calling.

        General use for calling PV knob values directly.
        """
        ans = ans or value is None
        if _hasitems(value):
            value = _nc._tupletostr(value)
        command = cmd if value is None else '%s %s' % (cmd, value)
        res = self._com(command, ans, timeout=timeout)
        if res is not None:
            if res.startswith('ERROR'):
                raise Exception(res)
            if res.startswith(cmd):
                res = res[len(cmd)+1:]
            return res.strip()


class _channel_nc(_nc):
    """Open a data channel to stream out data from the carrier.

    Call it independently as: ch = channel([num],host)
    """

    def __str__(self): return "Channel(%s,%d)" % self._server

    def __init__(self, ch, server='localhost'):
        super(_channel_nc, self).__init__((server, _data_port+ch))

    def buffer(self, nbytes=4194304, format='int16'):
        sock = self.sock

        def recv(sock, size):
            buf = bytearray(size)
            view = memoryview(buf)
            while size > 0 and self.on:
                nbytes = sock.recv_into(view, size)
                if not nbytes:
                    break
                view = view[nbytes:]
                size -= nbytes
            return buf[:-size] if size > 0 else buf
        try:
            while self.on:
                bytesin = recv(sock, nbytes)
                if not bytesin:
                    break  # is empty
                yield numpy.frombuffer(bytesin, format)
        finally:
            sock.close()

    def read(self, format='int16'):
        ans = [buf for buf in self.buffer()]
        return numpy.concatenate(ans, 0)

    def raw(self, rang=None):
        if rang is None:
            return self.read()
        if isinstance(rang, slice):
            slic = rang
        elif _hasitems(rang):
            slic = slice(*rang)
        else:
            slic = slice(None, None, rang)
        return self.read()[slic]


class _custom_awg(_nc):
    def sigCong(self, nsignals, *signals):
        # singals must be [-1. .. 1.]
        maxsize = max((s.size for s in signals))
        # initial function times the maximum value (32767)
        w = [(s.flatten()*32767).astype(numpy.int16) for s in signals]
        w = [numpy.pad(v, (0, maxsize-v.size), 'edge') for v in w]
        if len(w) < nsignals:
            dflt = numpy.array([numpy.int16(0)]*maxsize)
            w = w + ([dflt]*(nsignals-len(w)))
        return numpy.array(w).tostring('F')

    def loadSig(self, bitstring, rearm=False):
        sent, tosend = 0, len(bitstring)
        mv = memoryview(b(bitstring))
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((self._server[0], _ao_oneshot+int(bool(rearm))))
        try:
            sock.recv(1024)
            while sent < tosend:
                n = sock.send(mv[sent:])
                if n == 0:
                    print("receiver did no accept any more bytes: " +
                          "still to send %d bytes" % (tosend,))
                    break
                sent += n
            sock.shutdown(socket.SHUT_WR)
            try:
                sock.settimeout(3+tosend//1000000)
                if sock.recv(1024):
                    sock.settimeout(3)
                    while sock.recv(1024):
                        pass
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception as e:
                print(e)
        finally:
            sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((self._server[0], _ao_oneshot_cs))
        try:
            sha1sum = sock.recv(40)
        finally:
            sock.close()
        if debug > 0 and hashlib is not None:
            sha1sum_self = hashlib.sha1(bitstring).hexdigest()
            dprint('checksums: %s <> %s', sha1sum_self, sha1sum)
        else:
            dprint(sha1sum)


class _stream_nc(_nc):
    """Used to stream data from participating modules.

    data reduction either through duty cycle or subset
    set.site 0 STREAM_OPTS --subset=3,4 # for chans 3,4,5,6
    """

    def __init__(self, server='localhost'):
        super(_stream_nc, self).__init__((server, _aggr_port))


class _gpg_nc(_nc):
    """Used to  program the General Pulse Generator."""

    def __init__(self, server='localhost'):
        super(_gpg_nc, self).__init__((server, _gpgw_port))

    def write(self, sequence, loop=False):
        if _hasitems(sequence):
            seqstrs = []
            gate = False
            for tm in sequence:
                gate = not gate
                seqstrs.append("%08d,%s" % (tm, ('f' if gate else '0')))
            seqstrs.append("EOFLOOP\n" if loop else "EOF\n")
            sequence = "\n".join(seqstrs)
        if debug:
            dprint(sequence)
        sequence = memoryview(b(sequence))
        to_send = len(sequence)
        sent = self.sock.send(sequence)
        while sent < to_send:
            sent += self.sock.send(sequence[sent:])

    def write_gate(self, high, period=None, delay=5):
        """Generate a gate with fixed witdh.

        If period is set the gate will repeat infinitely
        The hardware requires the delay top be at least 5 ticks_parent
        """
        if delay < 5:
            sys.stderr.write("gpg: warning - delay must be at least 5 ticks")
            delay = 5
        high += delay+2
        if period is None:
            self.write((delay, high))
        else:  # translate period and set up as loop
            if period >= 0xffffff:
                raise Exception("gpg: error - " +
                                "ticks must be in range [5,16777214]")
            self.write((delay, high, period+1), loop=True)


class _line_logger_nc(_nc):
    def lines(self, timeout=60):
        sock = self.sock
        sock.settimeout(timeout)

        def gen():
            while self.on:
                c = sock.recv(1)
                if not c:
                    break  # is empty
                yield c
                if c == b'\n':
                    break

        try:
            while self.on:
                line = b''.join(gen())
                if not line:
                    break
                yield line
        finally:
            sock.close()


class STATE:
    """Translate state ids from state, transient_state, etc."""

    CLEANUP = 'CLEANUP'
    STOP = 'STOP'
    ARM = 'ARM'
    PRE = 'PRE'
    POST = 'POST'
    FIN1 = 'FIN1'
    FIN2 = 'FIN2'
    names = [STOP, ARM, PRE, POST, FIN1, FIN2]
    _loggers = {}
    _lock = threading.Lock()
    @classmethod
    def get_name(cls, id):
        """Return name of state id."""
        return cls.names[int(id)]

    class logger(threading.Thread):
        """Thread listening for the state updates on the log port."""

        _initialized = False
        _com = None
        _re_state = re.compile(b"([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)")

        def __new__(cls, host='localhost', *arg, **kwarg):
            """Ensure only one logger per uut."""
            with STATE._lock:
                if host in STATE._loggers:
                    return STATE._loggers[host]
                self = super(STATE.logger, cls).__new__(cls)
                STATE._loggers[host] = self
                return self

        def __init__(self, host='localhost', debug=0):
            """Ensure only logger only initializes once per uut."""
            if self._initialized:
                return
            super(STATE.logger, self).__init__(name=host)
            self._initialized = True
            self._stopped = threading.Event()
            self.daemon = True
            self.cv = threading.Condition()
            self.debug = debug
            self._state = set()
            self.reset()
            self.start()

        @property
        def on(self):
            """Return if logger still running."""
            return not self._stopped.is_set()

        def stop(self):
            """Request stop."""
            self._stopped.set()

        def run(self):
            """Capture states and update state history."""
            def new_com():
                com = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                com.settimeout(3)
                com.connect((self.name, _state_port))
                com.settimeout(1)
                return com

            sock_timeout = socket.timeout
            sock_error = socket.error
            try:
                com = new_com()
                try:
                    while self.on:
                        try:
                            msg = com.recv(1024)
                            if not msg:
                                raise sock_error
                            msg = msg.strip(b'\r\n')
                            if self.debug > 2:
                                dprint(msg)
                        except (SystemExit, KeyboardInterrupt):
                            raise
                        except sock_timeout:
                            continue
                        except sock_error:
                            com.close()
                            com = new_com()
                            time.sleep(1)
                            continue
                        match = self._re_state.match(msg)
                        if match is None:
                            continue
                        with self.cv:
                            if self.debug > 1:
                                dprint(match.group(0))
                            statid = int(match.group(1))
                            if statid > 5:
                                stat = statid
                            else:
                                stat = STATE.get_name(statid)
                            self._state.add(stat)
                            self._pre = int(match.group(2))
                            self._post = int(match.group(3))
                            self._elapsed = int(match.group(4))
                            self._reserved = int(match.group(5))
                            self.cv.notify_all()
                finally:
                    com.close()
            finally:
                del(STATE._loggers[self.name])
                with self.cv:
                    self.stop()
                    self.cv.notify_all()

        def reset(self):
            """Reset state history before waiting for state."""
            with self.cv:
                self._state.clear()
                self._pre = -1
                self._post = -1
                self._elapsed = -1
                self._reserved = -1
                self.cv.notify_all()

        def wait4state(self, state):
            """Wait until state happened."""
            if state not in STATE._map:
                raise Exception("No such state %s" % state)
            with self.cv:
                while (state not in self._state) and self.on:
                    self.cv.wait(1)
            return self.on

        @property
        def state(self):
            """Return current state."""
            with self.cv:
                return set(self._state)

        @property
        def pre(self):
            """Return current pre."""
            with self.cv:
                return self._pre

        @property
        def post(self):
            """Return current post."""
            with self.cv:
                return self._post

        @property
        def elapsed(self):
            """Return current elapsed."""
            with self.cv:
                return self._elapsed

        @property
        def reserved(self):
            """Return current reserved."""
            with self.cv:
                return self._reserved


# -----------------
#  HINT 0 nc properties
# -----------------
RO, RW = True, False


class _KNOBS(object):
    _knobs = {
        '_cnt': ('grp', {
            'RESET': ('exe',),
            'COUNT': (int, RO),
            'FREQ': (float, RO),
            'ACTIVE': (bool, RO),
        }),
    }

    def __new__(cls, m):
        knobs = m._knobs
        del(m._knobs)
        return cls.setup(m, knobs)

    def __init__(self, *a, **kw): pass

    @classmethod
    def setup(cls, m, knobs, sigs=()):
        for k, v in knobs.items():
            try:
                setattr(m, k.replace(':', '_').replace('.', '_'),
                        cls.prop(k, *v))
            except Exception as e:
                print(e, m, k)
        return m

    @classmethod
    def prop(cls, name, hint, *args):
        if hint is str:
            return cls._str(name, *args)
        if hint is int:
            return cls._int(name, *args)
        if hint is float:
            return cls._float(name, *args)
        if hint is bool:
            return cls._bool(name, *args)
        if isinstance(hint, list):
            if hint[0] is int:
                return cls._list_int(name, *args)
            if hint[0] is float:
                return cls._list_float(name, *args)
        if hint == 'exe':
            return cls.prop_grp(name, cls._exe, *args)
        if hint == 'grp':
            return cls.prop_grp(name, cls._grp, *args)
        if hint == 'idx':
            return cls.prop_grp(name, cls._idx, *args)
        if hint == 'cnt':
            return cls.prop_grp(name, cls._cnt, *args)
        if hint == 'state':
            return cls._state(name, *args)
        if hint == 'aggr':
            return cls._aggr(name, *args)
        raise Exception("Hint '%s' is not recognized.")

    @classmethod
    def prop_grp(cls, name, prop, knobs={}, fields={}):
        return cls.setup(type(name, (prop,), fields), knobs)

    class _str(object):
        ro = False
        _rcast = str
        _wcast = str

        def __get__(self, inst, cls):
            if inst is None:
                return self
            return self._rcast(inst(cmd=self._cmd))

        def __set__(self, inst, value):
            if self.ro:
                raise AttributeError
            return inst(cmd=self._cmd, value=self._wcast(value))

        def __init__(self, cmd, ro=False, doc=None, cmd2=None):
            if doc is not None:
                self.__doc__ = doc
            self.ro = ro
            self._cmd = cmd2 if cmd2 else cmd

    class _aggr(_str):
        ro = True
        @staticmethod
        def _rcast(val): return dict(p.split('=') for p in val.split(' '))

    class _bool(_str):
        @staticmethod
        def _rcast(val): return float(val) != 0
        @staticmethod
        def _wcast(val): return 1 if val else 0

    class _int(_str):
        _rcast = int
        _wcast = int

    class _float(_str):
        _rcast = float
        _wcast = float

    class _list_int(_str):
        @staticmethod
        def _rcast(val):
            return tuple(int(v) for v in val.split(' ', 1)[0].split(','))

    class _list_float(_str):
        @staticmethod
        def _rcast(val):
            return tuple(float(v) for v in val.split(' ', 1)[1].split(' '))

    class _grp(object):
        _parent = None

        def __call__(self, cmd, value=None, **kw):
            # allows for nested structures, e.g. acq.SYS.CLK.COUNT
            return self._parent(
                cmd='%s:%s' % (self.__class__.__name__, cmd),
                value=value, **kw)

        def __str__(self):
            if isinstance(self._parent, _KNOBS._grp):
                return '%s:%s' % (self._parent, self.__class__.__name__)
            return self.__class__.__name__

        def __repr__(self):
            return '%r:%s' % (self._parent, self.__class__.__name__)

        def __init__(self, parent, doc=None):
            if isinstance(parent, _string):
                raise Exception("ERROR: _KNOBS._grp as initializer")
            self._parent = parent
            if doc is not None:
                self.__doc__ = doc
            for k, v in self.__class__.__dict__.items():
                if isinstance(v, type) and issubclass(v, (_KNOBS._grp,)):
                    self.__dict__[k] = v(self)

        def __getattribute__(self, name):
            v = super(_KNOBS._grp, self).__getattribute__(name)
            if name.startswith('_'):
                return v
            if isinstance(v, type):
                if issubclass(v, (_KNOBS._grp,)):
                    return v(self)
                elif issubclass(v, (_KNOBS._str,)):
                    return v(self).__get__(self, self.__class__)
            return v

        def __getattr__(self, name):
            if name.startswith('_'):
                return super(_dtacq, self).__getattribute__(name)
            return self(cmd=name)

        def __setattr__(self, name, value):
            """Try to set for super/class and assume nc entry on failure."""
            try:
                if hasattr(self.__class__, name):
                    super(_KNOBS._grp, self).__setattr__(name, value)
            except AttributeError:
                return self(cmd=name, value=value)

    class _exe(_grp):
        _args = '1'
        _timeout = None
        _cmd = None
        _ans = False

        def __call__(self, *a, **kw):
            cmd = kw.pop('cmd', None)
            if cmd is not None:
                return super(_KNOBS._exe, self).__call__(cmd, *a, **kw)
            cmd = self.__class__.__name__ if self._cmd is None else self._cmd
            value = ' '.join(map(str, a)) if a else self._args
            return self._parent(
                cmd=cmd, value=value, timeout=self._timeout, ans=self._ans)

    class _idx(_grp, _str):
        _format_idx = '%d'
        _strip = ''
        _idxsep = ':'

        def get(self, idx):
            return '%s%s%s' % (self.__class__.__name__,
                               self._idxsep,
                               self._format_idx % idx)

        def set(self, idx, value):
            return '%s %s' % (self.get(idx), self._wcast(value))

        def __getitem__(self, idx):
            return self._rcast(self._parent(self.get(idx)).strip(self._strip))

        def __setitem__(self, idx, value):
            return self._parent(self.set(idx, value))

        def setall(self, value):
            return [self.set(i+1, v) for i, v in enumerate(value)]

    class _state(object):
        def __get__(self, inst, cls):
            if inst is None:
                return self
            ans = inst(self._cmd)
            if not ans:
                return {'state': 'CLEANUP'}
            try:
                res = list(map(int, ans.split(' ')))+[0]*4
            except ValueError:
                return {'state': 'CLEANUP'}
            stat = res[0]
            if stat <= 5:
                stat = STATE.get_name(stat)
            return {
                'state': stat,
                'pre': res[1],
                'post': res[2],
                'elapsed': res[3],
                'reserved': tuple(res[4:]),
            }

        def __init__(self, cmd, doc=None):
            if doc is not None:
                self.__doc__ = doc
            self._cmd = cmd


_KNOBS(_KNOBS)


class _KNOBS_AND_SIGS(_KNOBS):
    def __new__(cls, m):
        _KNOBS.__new__(cls, m)
        sigs = m._sigs
        del(m._sigs)
        for sig in sigs:
            cls.add_sig(m, sig)
        return m

    @classmethod
    def add_sig(cls, m, sig):
        lower = sig.lower()
        # setstr = '%s %%s\n%s:DX %%s\n%s:SENSE %%s' % (sig, sig, sig)
        setstr = '%s %%s,%%s,%%s' % (lower,)  # captized version is not shared
        setattr(m, lower, cls._list_int(lower, RW))
        setattr(m, sig, cls._str(sig, RW, 'external or internal'))
        setattr(m, sig+'_DX', cls._str(sig+':DX', RW, 'd0 through d7'))
        setattr(m, sig+'_SENSE', cls._str(sig+':SENSE', RW,
                                          'rising or falling'))

        def fget(self):
            return getattr(self, lower)

        def fset(self, value):
            self(setstr % (value[0], value[1], value[2]))
        setattr(m, sig+'_ALL', property(fget, fset))


# ------------------
# HINT 1 dtacq_knobs
# ------------------
class _help(object):
    @property
    def help(self):
        return self('help', timeout=13).split('\n')

    @property
    def helpA(self):
        return dict(
            tuple(map(str.strip, a.split(' : ')))
            for a in self('helpA', timeout=13).split('\n')
        )


@_KNOBS
class _hwmon_knobs(_help):
    _knobs = {
        'in1': (int, RO),
        'in2': (int, RO),
        'in3': (int, RO),
        'in4': (int, RO),
        'temp': (int, RO),
    }


@_KNOBS
class _dtacq_knobs(_help):
    _knobs = {
        'bufferlen': (int, RO),
        'data32': (bool, RO),
        'MODEL': (str, RO),
        'NCHAN': (int, RO),
        'SERIAL': (str, RO),
    }


@_KNOBS_AND_SIGS
class _carrier_knobs(_dtacq_knobs):
    _sigs = 'GPG_CLK', 'GPG_SYNC', 'GPG_TRG'
    _knobs = {
        'acq480_force_training': (bool, RW),
        'acq480_train': ('exe', {}, {'_timeout': 15}),
        'aggregator': ('aggr',),
        'distributor': ('aggr',),
        'fit_rtm_translen_to_buffer': ('exe',),
        'gpg_enable': (int, RW),
        'gpg_mode': (int, RW),
        'live_mode': (int, RW, "Streaming: {0:off, 1:free-run, 2:pre-post}"),
        'live_pre': (int, RW, "Streaming: pre samples for pre-post mode"),
        'live_post': (int, RW, "Streaming: post samples for pre-post mode"),
        'reboot': ('exe', {}, {
            '__doc__': "Reboot carrier with sync;sync;reboot.",
            '_args': 3210,
        }),
        'run0': ('exe', {}, {
            '_args': '', '_ans': True,
            '__doc__': "Initialize the aggregator for ai"}),
        'play0': ('exe', {}, {
            '_args': '', '_ans': True,
            '__doc__': "Initialize the distributor for ao"}),
        'streamtonowhered_start': ('exe', {}, {
            '_cmd': 'streamtonowhered', '_args': 'start'}),
        'streamtonowhered_stop': ('exe', {}, {
            '_cmd': 'streamtonowhered', '_args': 'stop'}),
        'CONTINUOUS_start': ('exe', {}, {
            '_cmd': 'CONTINUOUS', '_args': 'start'}),
        'CONTINUOUS_stop': ('exe', {}, {
            '_cmd': 'CONTINUOUS', '_args': 'stop'}),
        'set_arm': ('exe',),
        'soft_trigger': ('exe',),
        'state': ('state',),
        'transient_state': ('state',),
        'streamtonowhered': (str, RW, 'start|stop'),
        'shot': (int, RW),
        'HN': (str, RO, "Return the UUT of the device."),
        'CONTINUOUS:STATE': ('state',),
        'CONTINUOUS:STATUS': (str, RO),
        'CONTINUOUS': (str, RW, "reliable way to start|stop stream"),
        'GPG': ('grp', {
            'ENABLE': (str, RW, "enable Gate Pulse Generator"),
            'MODE': (str, RW, "set GPG mode One Shot | Loop | Loop/Wait"),
            'DBG': ('grp', {
                'CTR': (int, RO),
                'DEF': (str, RO),
                'OSTATE': (int, RO),
                'RSTATE': (int, RO),
                'STATE': (str, RO),
                'UNTIL': (int, RO),
            }),
        }),
        'LIVE': ('grp', {
            'MODE': (str, RO, "Live mode {0:off, 1:free-run, 2:pre-post}"),
        }),
        'SET_AO_EXPR': ('exe', {}, {
            '_args': AO_MODE.ONE_SHOT, '_timeout': 10}),
        'SIG': ('grp', {
            'SOFT_TRIGGER': ('exe',),
            'ZCLK_SRC': (str, RW, 'INT33M, CLK.d0 - CLK.d7'),
            'CLK_EXT': ('cnt',),
            'CLK_MB': ('cnt', {
                'FIN': (int, RW, 'External input frequency'),
                'SET': (int, RW, 'Set desired MB frequency'),
                'READY': (int, RO, 'Clock generation ready'),
            }),
            'EVENT_SRC': ('idx',),
            'EVT_EXT': ('idx',),
            'EVT_MB': ('idx',),
            'EVT_S1': ('idx',),
            'SYN_EXT': ('idx',),
            'SYN_MB': ('idx',),
            'SYN_S1': ('idx',),
            'TRG_MB': ('idx',),
            'TRG_S1': ('idx',),
            'FP': ('grp', {
                'CLKOUT': (str, RW),
                'SYNC': (str, RW),
                'TRG': (str, RW),
            }),
            'SRC': ('grp', {
                'CLK': ('idx',),
                'SYNC': ('idx',),
                'TRG': ('idx',),
            }),
            'CLK': ('grp', {
                'TRAIN_REQ': (bool, RO),
            }),
            'CLK_S1': ('grp', {
                'FREQ': (float, RW),
            }),
        }),
        'SYS': ('grp', {
            'CLK': ('grp', {
                'FPMUX': (str, RW, "OFF, XCLK, FPCLK, ZCLK"),
            }),
        }),
        'TRANSIENT': ('exe', {
            'PRE': (int, RW, 'Number of pre-trigger samples'),
            'POST': (int, RW, 'Number of post-trigger samples'),
            'OSAM': (int, RW, 'Subrate data monitoring'),
            'SOFT_TRIGGER': (bool, RW, 'Initial soft_trigger call'),
            'REPEAT': (int, RW, 'Number of shots before set_abort'),
            'DELAYMS': (int, RW, 'Number of ms to wait before soft trigger'),
            'SET_ARM': ('exe', {}, {
                '__doc__': "Put carrier in armed state; go to ARM."}),
            'SET_ABORT': ('exe', {}, {
                '__doc__': "Abort current capture; go to STOP."}),
        }),
    }

    def off_slo(self, *sites):  # custom"Reboot carrier with sync;sync;reboot."
        if len(sites) == 1 and _hasitems(sites[0]):
            sites = sites[0]
        res = self('off_slo %s' % (self._tupletostr(sites),)).strip()
        off_slo = [[float(c) for c in r.split(' ')] for r in res.split('\n')]
        return _zipped(*off_slo)

    def set_abort(self, keep_repeat=0):
        """Use keep_repeat 1 to only abort the arming on a single sequence."""
        if keep_repeat == 0:
            self.TRANSIENT_REPEAT = 0
        self('set_abort')
        return self

    def transient(
            self, pre=None, post=None, osam=None, soft_out=None, demux=None):
        cmd = 'transient'
        if pre is not None:
            cmd += ' PRE=%d' % (pre,)
        if post is not None:
            cmd += ' POST=%d' % (post,)
        if osam is not None:
            cmd += ' OSAM=%d' % (osam,)
        if soft_out is not None:
            cmd += ' SOFT_TRIGGER=%d' % (1 if soft_out else 0,)
        if demux is not None:
            cmd += ' DEMUX=%d' % (1 if demux else 0,)
        ans = self(cmd)
        if cmd.endswith('transient'):
            glob = {}
            exec(ans.split('\n')[-1].replace(' ', ';')) in {}, glob
            return glob

    def TRANSIENT_ALL(self, pre, post, osam, soft_out, repeat, demux):
        self._com('\n'.join([
            'TRANSIENT:PRE %d' % pre,
            'TRANSIENT:POST %d' % post,
            'TRANSIENT:OSAM %d' % osam,
            'TRANSIENT:SOFT_TRIGGER %d' % (1 if soft_out else 0),
            'TRANSIENT:REPEAT %d' % repeat,
        ]))
        self.transient(pre, post, osam, soft_out, demux)

    def STREAM_OPTS(self, raw=None, subset=None, nowhere=False):  # package.w7x
        if nowhere:
            return self('STREAM_OPTS --null-copy')
        params = ['STREAM_OPTS']
        if raw is not None:
            params.append(raw)
        if subset is not None:
            params.append('--subset=%s' % (self._tupletostr(subset),))
        if len(params) == 1:
            params.append('""')
        return self(' '.join(params))


class _acq1001_knobs(_carrier_knobs):
    pass


@_KNOBS
class _acq2106_knobs(_carrier_knobs):
    _bypass = '1-1_bypass'
    _knobs = {
        'data_engine_2': ('aggr', RW, 'aggregator for mgt482 line A'),
        'data_engine_3': ('aggr', RW, 'aggregator for mgt482 line B'),
        'load.si5326': ('exe',),
        'run0_ready': (bool, RO),
        'set_si5326_bypass': (bool, RW),
        'si5326_step_phase': ('exe',),
        'si5326_step_state': ('state',),
        'si5326bypass': (str, RO),
        'si5326config': (str, RO),
        'sites': ([int], RO),
        'spadstart': (int, RO),
        'ssb': (int, RO),
        'TRANS_ACT': ('grp', {
            'STATE_NOT_IDLE': (int, RO),
            'POST': (int, RO),
            'PRE': (str, RO),
            'STATE': (str, RO),
            'TOTSAM': (int, RO),
            'FIND_EV': ('grp', {
                'CUR': (int, RO),
                'NBU': (int, RO),
                'STA': (str, RO),
            }),
        }),
    }

    class SYS(_carrier_knobs.SYS):
        @_KNOBS
        class CLK(_carrier_knobs.SYS.CLK):
            _knobs = {
                'BYPASS': (int, RW),
                'C1B': (str, RO),
                'C2B': (str, RO),
                'CONFIG': (str, RO),
                'LOL': (bool, RO, 'Jitter Cleaner - Loss of Lock'),
                'OE_CLK1_ZYNQ': (int, RW),
                'OE_CLK1_ELF1': ('exe',),
                'Si5326': ('grp', {
                    'PLAN': (str, RW, '{10M, 20M, 24M, 40M, 50M, 80M}'),
                    'PLAN_EN': (bool, RO),
                }),
                'Si570_OE': ('exe',),
            }


@_KNOBS
class _site_knobs(_dtacq_knobs):
    _knobs = {
        'nbuffers': (int, RO),
        'modalias': (str, RO),
        'module_name': (str, RO),
        'module_role': (str, RO),
        'module_type': (int, RO),
        'site': (int, RO),
        'AGIX': (int, RW),
        'MANUFACTURER': (str, RO),
        'MTYPE': (int, RO),
        'PART_NUM': (str, RO),
    }


@_KNOBS_AND_SIGS
class _module_knobs(_site_knobs):
    _sigs = 'CLK', 'EVENT0', 'EVENT1', 'RGM', 'SYNC', 'TRG'
    _knobs = {
        'clkdiv': (int, RW),
        'CLKDIV': (int, RW),
        'SIG': ('grp', {
            'CLK_COUNT': ('cnt',),
            'sample_count': ('cnt',),
            'SAMPLE_COUNT': ('cnt', {
                'RUNTIME': (str, RW),
            }),
        }),
    }


@_KNOBS
class _acq4xx_knobs(_module_knobs):
    _knobs = {
        'rtm_translen': (int, RW, (
            'samples per trigger in RTM; should align with N buffers')),
        'RTM_TRANSLEN': (int, RW, (
            'samples per trigger in RTM; should align with N buffers')),
        'simulate': (bool, RW),
        'es_enable': (int, RW, 'data will include an event sample'),
        'AI': ('grp', {
            'CAL': ('grp', {
                'EOFF': ([float], RO),
                'ESLO': ([float], RO),
            }),
        }),
    }


@_KNOBS
class _acq425_knobs(_acq4xx_knobs):
    _knobs = {
        'MAX_KHZ': (int, RO),
    }


@_KNOBS
class _acq480_knobs(_acq4xx_knobs):
    _knobs = {
        'acq480_loti': (bool, RO, 'Jitter Cleaner - Loss of Time'),
        'train': ('exe', {}, {'__doc__': 'train clocks'}),
        'ACQ480': ('grp', {
            'TRAIN': (str, RO),
            'FIR': ('idx', {
                'DECIM': (int, RO)
            }, {'_format_idx': '%02d'}),
            'FPGA': ('idx', {
                'DECIM': (int, RO)
            }, {'_format_idx': '%02d'}),
            'INVERT': ('idx', {}, {'_format_idx': '%02d'}),
            'GAIN': ('idx', {}, {'_format_idx': '%02d'}),
            'HPF': ('idx', {}, {'_format_idx': '%02d'}),
            'LFNS': ('idx', {}, {'_format_idx': '%02d'}),
            'T50R': ('idx', {}, {'_format_idx': '%02d'}),
            'T50R_ALL': (int, RW, 'set T50R for all channels', 'T50R'),
        }),
        'JC_LOL': (bool, RO, 'Jitter Cleaner - Loss of Lock'),
        'JC_LOS': (bool, RO, 'Jitter Cleaner - Loss of Signal'),
    }

    @_KNOBS
    class SIG(_acq4xx_knobs.SIG):
        _knobs = {
            'CLK': ('grp', {
                'TRAIN_BSY': (bool, RO, 'Clock sync currently training'),
            }),
        }


@_KNOBS
class _ao420_knobs(_module_knobs):
    _knobs = {
        'run': (bool, RO),
        'task_active': (bool, RO),
        'ACC': (str, RW),
        'D': ('idx', {}, {'_idxsep': ''}),
        'G': ('idx', {}, {'_idxsep': ''}),
        'AO': ('grp', {
            'GAIN': ('grp', {
                'CH': ('idx',),
            }),
            'EXPR': ('grp', {
                'CH': ('idx', {}, {'_wcast': lambda s, x: repr(str(x))}),
            }),
        }),
        'AWG': ('grp', {
            'D': ('idx',),
            'G': ('idx',),
        }),
    }


# --------------------
# HINT 2 dtacq nc classes
# --------------------
class _dtacq_nc(_nc, _dtacq_knobs):
    _exclude = []
    _transient = ['state', 'shot']
    _help = None
    _helpA = None
    cache = None
    _cache = {}

    @classmethod
    def add_cache(cls, host):
        cls._cache[host] = {}

    @classmethod
    def remove_cache(cls, host):
        return cls._cache.pop(host)

    def __init__(self, server, site):
        super(_dtacq_nc, self).__init__((server, _sys_port+site))
        for cls in self.__class__.mro():
            for k, v in cls.__dict__.items():
                if isinstance(v, type) and k not in self.__dict__:
                    if issubclass(v, (_KNOBS._grp,)):
                        self.__dict__[k] = v(self)
        self.cache = self._cache.get(server, None)
        if self.cache is not None:
            self.cache = self.cache.setdefault(str(site), {})

    def filter_result(self, cmd, res, val):
        cmd = cmd.strip()
        if (
            cmd in self._exclude or
            cmd in self._transient or
            cmd.endswith(':RESET')
        ):
            return cmd, None
        res = res.strip()
        if res.startswith(cmd):
            res = res[len(cmd)+1:]
        if not res:
            res = None if not val else val[0]
        return cmd, res

    def _com(self, cmd, ans=False, timeout=5):
        dbg = [None, None]
        super(_dtacq_nc, self)._com(cmd, ans, timeout, dbg)
        if (
            dbg[0] is not None and
            dbg[1] is not None and
            self.cache is not None
        ):
            rows = dbg[0].split('\n')
            rres = dbg[1].split('\n')
            sync = len(rows) == len(rres)
            for i in range(len(rows)):
                cmd = rows[i].strip().split(' ', 2)
                res = rres[i] if sync else ''
                cmd, res = self.filter_result(cmd[0], res, cmd[1:])
                if res is not None:
                    self.cache[cmd] = res
        if ans:
            return dbg[1]

    @staticmethod
    def _callerisinit(ignore):
        stack = inspect.stack()[2:]
        for call in stack:
            if call[3] == ignore:
                continue
            if call[3] == '__init__':
                return True
            break
        return False

    def _getfromdict(self, name):
        for cls in self.__class__.mro():
            if cls is _nc:
                raise AttributeError
            try:
                return cls.__dict__[name]
            except KeyError:
                continue

    def __getattribute__(self, name):
        v = _nc.__getattribute__(self, name)
        if name.startswith('_'):
            return v
        if name in ('_server', '_com', '_getfromdict'):
            return v
        if isinstance(v, type):
            if issubclass(v, (_KNOBS._grp,)):
                return v(self)
            elif issubclass(v, (_KNOBS._str,)):
                return v(self).__get__(self, self.__class__)
        return v

    def __getattr__(self, name):
        if name.startswith('_'):
            return super(_dtacq_nc, self).__getattribute__(name)
        try:
            return self._getfromdict(name)
        except AttributeError:
            return self(name)

    def __setattr__(self, name, value):
        """Try to set for super/class and assume netcat entry on failure."""
        try:
            if hasattr(self.__class__, name):
                super(_dtacq_nc, self).__setattr__(name, value)
        except AttributeError:
            if not _dtacq_nc._callerisinit('__setattr__'):
                return self(name, value)
            return super(_dtacq_nc, self).__setattr__(name, value)


class _carrier_nc(_dtacq_nc, _carrier_knobs):
    _log = None
    ai_sites = ao_sites = None  # preset so setattr does not try knob
    _exclude = _dtacq_nc._exclude + [
        'get.site', 'reboot',
        'soft_transient', 'soft_trigger', 'SIG:SOFT_TRIGGER',
        'streamtonowhered', 'CONTINUOUS', 'TRANSIENT',
        'TRANSIENT:SOFT_TRIGGER', 'TRANSIENT:PRE', 'TRANSIENT:POST',
        'TRANSIENT:OSAM', 'TRANSIENT:SET_ARM', 'TRANSIENT:SET_ABORT',
        'set_arm', 'set_abort',
    ]

    def __init__(self, server, ai_sites=None, ao_sites=None):
        super(_carrier_nc, self).__init__(server, 0)
        self.ai_sites = ai_sites
        self.ao_sites = ao_sites

    @property
    def log(self):
        if self._log is None:
            self._log = STATE.logger(self._server[0], debug)
        return self._log

    def channel(self, i):
        return _channel_nc(i, self._server[0])

    def wait(self, timeout, condition, breakcond):
        if timeout is not None:
            timeout = time.time()+timeout
        ok = [True]  # python way of defining a pointer
        with self.log.cv:
            if condition(self, ok):
                while self.log.on:
                    if breakcond(self, ok):
                        break
                    if timeout is not None and (time.time() > timeout):
                        ok[0] = False
                        break
                    self.log.cv.wait(1)
                else:
                    raise Exception('logger terminated')
            if debug:
                dprint(self.state)
        return self.log.on and ok[0]

    def wait4state(self, state, timeout=None):
        if state not in STATE.names:
            raise Exception("No such state %s" % state)

        def condition(self, ok):
            if self.state['state'] == state:
                return False
            if state in self.log._state:
                self.log._state.remove(state)
            return True

        def breakcond(self, ok):
            return state in self.log._state

        return self.wait(timeout, condition, breakcond)

    def wait4arm(self, timeout=None):
        waitstates = (STATE.STOP, STATE.CLEANUP)

        def condition(self, ok):
            if not self.state['state'] in waitstates:
                return False
            if STATE.ARM in self.log._state:
                self.log._state.remove(STATE.ARM)
            self.TRANSIENT.SET_ARM()
            return True

        def breakcond(self, ok):
            if (
                STATE.ARM in self.log._state or
                self.state['state'] not in waitstates
            ):
                return True
            self.TRANSIENT.SET_ARM()
            return False
        return self.wait(timeout, condition, breakcond)

    def wait4post(self, post, timeout=None):
        waitstates = (STATE.ARM, STATE.PRE, STATE.POST)

        def condition(self, ok):
            if (
                self.state['state'] not in waitstates or
                self.state['post'] >= post
            ):
                return False
            if STATE.STOP in self.log._state:
                self.log._state.remove(STATE.STOP)
            return True

        def breakcond(self, ok):
            state = self.state
            if (
                STATE.STOP in self.log._state or
                state['state'] == STATE.STOP or
                state['post'] >= post
            ):
                return True
            return False

        return self.wait(timeout, condition, breakcond)

    def wait4abort(self, timeout=None):
        self.CONTINUOUS_stop()
        self.streamtonowhered_stop()
        self.TRANSIENT.SET_ABORT()

        def condition(self, ok):
            if self.state['state'] == STATE.STOP:
                return False
            if self.state['state'] == STATE.FIN2:
                time.sleep(1)
                if self.state['state'] == STATE.FIN2:
                    self.set_arm()
                    self.set_abort()
            self.log._state.clear()
            return True

        def breakcond(self, ok):
            if (
                STATE.STOP in self.log._state or
                self.state['state'] == STATE.STOP
            ):
                return True
            self.TRANSIENT.SET_ABORT()
            return False

        return self.wait(timeout, condition, breakcond)

    def __call__(self, cmd, value=None, site=0, **kw):
        if not cmd.strip():
            return ""
        if site > 0:
            cmd = 'set.site %d %s' % (site, cmd)
        return super(_carrier_nc, self).__call__(cmd, value, **kw)

    def _init(self, ext, mb_fin, mb_set, pre, post, soft_out, demux, shot=1):
        if not self.wait4abort(timeout=30):
            raise Exception('Could not abort.')
        if self.ai_sites is not None:
            sites = self._tupletostr(self.ai_sites)
            if sites != self.aggregator['sites']:
                self.run0(sites)
                self.aggregator
        if self.ao_sites is not None:
            sites = self._tupletostr(self.ao_sites)
            if sites != self.distributor['sites']:
                self.play0(sites)
                self.distributor
        if shot is not None:
            self.shot = shot
        self._setup_clock(ext, mb_fin, mb_set)
        self._setup_trigger(pre, post, soft_out, demux)

    def _setup_clock(self, ext, mb_fin, mb_set):
        if ext:
            self.SYS.CLK.FPMUX = 'FPCLK'
            if debug:
                dprint('Using external clock source')
        else:
            self.SIG.ZCLK_SRC = 0  # INT33M
            self.SYS.CLK.FPMUX = 'ZCLK'
            if debug:
                dprint('Using internal clock source')
        self.SIG.CLK_MB.FIN = mb_fin
        self.SIG.CLK_MB.SET = mb_set  # = clk * clkdiv
        self.SIG.SRC.CLK[0] = 0
        self.SIG.SRC.CLK[1] = 0  # MCLK

    def _setup_trigger(self, pre, post, soft_out, demux):
        # setup Front Panel TRIG port
        self.SIG.FP.TRG = 'INPUT'
        # setup signal source for external trigger and software trigger
        self.SIG.SRC.TRG[0] = 0  # 'EXT'
        self.SIG.SRC.TRG[1] = 0  # 'STRIG'
        soft_out = 1 if pre > 0 else soft_out
        self.live_mode = 2 if pre else 1
        self.live_pre = pre
        self.live_post = post
        self.TRANSIENT_ALL(pre=pre,
                           post=post,
                           osam=1,
                           repeat=0,
                           soft_out=soft_out,
                           demux=demux)
        if debug:
            dprint('PRE: %d, POST: %d', pre, post)


class _acq1001_nc(_carrier_nc, _acq1001_knobs):
    _mclk_clk_min = 4000000


class _acq2106_nc(_carrier_nc, _acq2106_knobs):
    def _setup_clock(self, ext, mb_fin, mb_set):
        if self.SYS.CLK.CONFIG == _acq2106_nc._bypass:
            # ensure disabled bypass
            self.SYS.CLK.BYPASS = 0
            t = 20
            while self.SYS.CLK.CONFIG == _acq2106_nc._bypass:
                t -= 1
                if t < 0:
                    break
                time.sleep(.2)
        set_plan = not ext and mb_set in [
            10000000, 20000000, 24000000, 40000000, 50000000, 80000000]
        super(_acq2106_nc, self)._setup_clock(ext, mb_fin, mb_set)
        if set_plan:
            self.SYS.CLK.Si5326.PLAN = '%02dM' % (mb_set//1000000,)
            self.SIG.CLK_MB.SET = 0  # auto
        self.SYS.CLK.OE_CLK1_ZYNQ = 1


class _module_nc(_dtacq_nc, _module_knobs):
    _clkdiv_adc = 1
    _clkdiv_fpga = 1
    is_master = False

    def __init__(self, server, site=1):
        super(_module_nc, self).__init__(server, site)
        self.is_master = site == 1

    @property
    def slo(self): return self.AI.CAL.ESLO[1:]
    @property
    def off(self): return self.AI.CAL.EOFF[1:]
    @property
    def off_slo(self): return _zipped(self.off, self.slo)

    def _trig_mode(self, mode=None):
        if mode is None:
            return 0
        elif isinstance(mode, _string):
            mode = mode.upper()
            if mode == 'RGM':
                return 2
            elif mode == 'RTM':
                return 3
            else:
                return 0
        return mode

    def _init(self, shot=None):
        if shot is not None:
            self.shot = shot
        self.data32 = 0

    def _init_master(self, pre, soft):
        self.SIG.sample_count.RESET()
        t_src = 1 if pre | soft else 0
        e_src = 1 if soft else 0
        e_on = 1 if pre else 0
        self.TRG_ALL = (1, t_src, 1)
        self.EVENT0_ALL = (e_on, e_src, 1)
        self.CLK_ALL = (1, 1, 1)


class _acq425_nc(_module_nc, _acq425_knobs):
    class GAIN(_KNOBS._idx):
        _format_idx = '%02d'
        _rcast = int
        @staticmethod
        def _wcast(val): return '%dV' % (val,)
        ALL = _KNOBS._str('ALL')

    def gain(self, channel=0, *arg):
        if channel > 0:
            return int(self('gain%d' % (channel,), *arg))
        else:
            return int(self('gain', *arg))

    def range(self, channel=0):
        channel = str(channel) if channel > 0 else ''
        return 10. / (1 << int(self('gain%s' % (channel,))))

    def _init_master(self, pre, soft, clkdiv, mode=None, translen=None):
        mode = self._trig_mode(mode)
        ese = 1 if mode else 0
        self.chain_start()
        # use soft trigger to start stream for R?M: soft|ese
        super(_acq425_nc, self)._init_master(pre, soft | ese)
        if mode == 3 and translen is not None:
            self.rtm_translen = translen*self._clkdiv_fpga
        # self.EVENT1_ALL = (ese, 1 if soft else 0, ese)
        # marker is anyway set with f154
        self.RGM_ALL = (mode, 1 if soft else 0, 1)
        self.es_enable = ese
        self.chain_send()
        time.sleep(1)
        self.clkdiv = self.CLKDIV = clkdiv

    def _init(self, gain=None, shot=None):
        self.chain_start()
        super(_acq425_nc, self)._init(shot)
        if gain is not None:
            self._com(self.GAIN.setall(gain))
        self.chain_send()


class _acq480_nc(_module_nc, _acq480_knobs):
    _FIR_DECIM = [1, 2, 2, 4, 4, 4, 4, 2, 4, 8, 1]

    def _max_clk_dram(self, num_modules):
        """DRAM: max_clk = 900M / Nmod / 8 / 2."""
        limit = [50000000, 28000000, 18000000,
                 14000000, 11000000, 9000000][num_modules-1]
        return limit

    def max_clk(self, num_modules):
        limit = self._max_clk_dram(num_modules)
        adc_div = self._clkdiv_adc(self._firmode)
        if adc_div > 1:
            limit = min(limit, 80000000 // adc_div)
        fpga_div = self._clkdiv_fpga
        if fpga_div > 1:
            limit = min(limit, 25000000 // fpga_div)
        return limit

    def _clkdiv_adc(self, fir): return self._FIR_DECIM[fir]
    @_cached_property
    def _clkdiv_fpga(self): return self.ACQ480.FPGA.DECIM
    @_cached_property
    def _firmode(self): return self.ACQ480.FIR[1]
    def _clkdiv(self, fir): return self._clkdiv_adc(fir)*self._clkdiv_fpga

    def _init(self, gain=None, invert=None, hpf=None, lfns=None, t50r=None,
              shot=None):
        self.chain_start()
        super(_acq480_nc, self)._init(shot)
        if gain is not None:
            self._com(self.ACQ480.GAIN.setall(gain))
        if invert is not None:
            self._com(self.ACQ480.INVERT.setall(invert))
        if hpf is not None:
            self._com(self.ACQ480.HPF.setall(hpf))
        if lfns is not None:
            self._com(self.ACQ480.LFNS.setall(lfns))
        if t50r is not None:
            self._com(self.ACQ480.T50R.setall(t50r))
        self.chain_send()

    def _init_master(self, pre, soft, mode=None, translen=None, fir=0):
        mode = self._trig_mode(mode)
        ese = 1 if mode else 0
        self.chain_start()
        # use soft trigger to start stream for R?M: soft|ese
        super(_acq480_nc, self)._init_master(pre, soft | ese)
        if mode == 3 and translen is not None:
            self.rtm_translen = translen*self._clkdiv_fpga
        # self.EVENT1_ALL = (ese, 1 if soft else 0, ese)
        # marker is anyway set with f154
        self.RGM_ALL = (mode, 1 if soft else 0, 1)
        self.es_enable = ese
        self.ACQ480.FIR[1] = int(fir == 0)
        self.ACQ480.FIR[1] = fir
        self.chain_send()
        decim = 1 << int(fir == 0)
        while self.ACQ480.FIR.DECIM == decim:
            time.sleep(.2)


class _ao420_nc(_module_nc, _ao420_knobs):
    def set_gain_offset(self, ch, gain, offset):
        if gain+abs(offset) > 100:
            gain = gain/2.
            offset = offset/2.
            self.AO.GAIN.CH[ch] = 1
        else:
            self.AO.GAIN.CH[ch] = 0
        self.set_gain(ch, gain)
        self.set_offset(ch, offset)

    def set_gain(self, ch, gain):
        gain = min(100, max(0, gain))
        self.G[ch] = int(round(gain*32767/100.))

    def set_offset(self, ch, offset):
        offset = min(100, max(-100, offset))
        self.D[ch] = int(round(offset*32767/100.))

    def set_expr(self, ch, expr):
        ans = self('AO:EXPR:CH:%d "%s"' % (ch, expr))
        ans = tuple(int(a) for a in ans.split(' ', 3))
        if self.cache is not None:
            self.cache['AO:EXPR:CH:%d' % ch] = str(expr)
            self.cache['D%d' % ch] = ans[0]
            self.cache['G%d' % ch] = ans[1]
            self.cache['AO:GAIN:CH:%d' % ch] = 2 if ans[3] else ans[2]
        return ans

    def _init(self, exprs, shot=None):
        self.chain_start()
        super(_ao420_nc, self)._init(shot)
        self.chain_send()
        return tuple(self.set_expr(i+1, exprs[i]) for i in range(4))

    def _init_master(self, soft, clkdiv):
        self.chain_start()
        super(_ao420_nc, self)._init_master(0, soft)
        self.clkdiv = clkdiv
        self.EVENT1_ALL = (0, 0, 0)
        self.chain_send()


class _acq1001s_nc(_dtacq_nc):
    continuous_reader = _KNOBS._str('continuous_reader', RO)
    diob_src = _KNOBS._str('diob_src', RO)
    optimise_bufferlen = _KNOBS._str('optimise_bufferlen', RO)
    driver_override = _KNOBS._str('driver_override', RO)


class _mgt482_nc(_dtacq_nc):
    _linemap = (13, 12)
    _sites = set()
    _line = 0
    _devid = None

    def __init__(self, server='localhost', line=0):
        super(_mgt482_nc, self).__init__(server, self._linemap[line])
        self._line = line
    spad = _KNOBS._int('spad')
    @property
    def aggregator(self):
        sites = self('aggregator').split(' ')[1].split('=')[1]
        return [] if sites == 'none' else [int(v) for v in sites.split(',')]

    @aggregator.setter
    def aggregator(self, sites):
        self('aggregator sites=%s' % (','.join(map(str, sites))))

    def init(self, sites, devid=0):
        self.setup(sites, devid)
        self._init()

    def setup(self, sites, devid=0):
        self._sites = set() if sites is None else set(int(s) for s in sites)
        self._devid = int(devid)

    def _init(self):
        self.spad = 0
        self.aggregator = self._sites


class _mgtdram_nc(_dtacq_nc):
    get_async = None

    class run_shot_logger(threading.Thread):
        _count = -1
        _pid = None
        @property
        def error(self):
            with self._error_lock:
                return tuple(self._error)

        @property
        def pid(self):
            with self._busy_lock:
                return self._pid

        @property
        def count(self):
            with self._busy_lock:
                return self._count

        def __init__(self, host, autostart=True, debug=False, daemon=True):
            threading.Thread.__init__(self, name='mgt_run_shot(%s)' % host)
            self.daemon = daemon
            self.mgt = _line_logger_nc((host, _mgt_log_port))
            self._busy_lock = threading.Lock()
            self._error_lock = threading.Lock()
            self._error = []
            self._debug = True  # bool(debug)
            if autostart:
                self.start()

        def debug(self, line):
            if self._debug:
                sys.stdout.write("%16.6f: ")
                sys.stdout.write(line)
                sys.stdout.flush()

        def run(self):
            re_busy = re.compile(
                    '^BUSY pid ([0-9]+) SIG:SAMPLE_COUNT:COUNT ([0-9]+)')
            re_idle = re.compile('^.* SIG:SAMPLE_COUNT:COUNT ([0-9]+)')
            re_error = re.compile('^ERROR: (.*)')
            for line in self.mgt.lines():
                if line is None or len(line) == 0:
                    continue
                line = s(line)
                hits = re_busy.findall(line)
                if hits:
                    pid, count = int(hits[0][0]), int(hits[0][1])
                    if self._count < 0 and count > 0 or self._count == count:
                        continue
                    self.debug(line)
                    with self._busy_lock:
                        self._count = count
                        self._pid = pid
                    continue
                hits = re_error.findall(line)
                if hits:
                    self.debug(line)
                    with self._error_lock:
                        self._error.append(hits[0])
                    continue
                hits = re_idle.findall(line)
                if hits:
                    count = int(hits[0])
                    self.debug(line)
                    with self._busy_lock:
                        self._count = count
                    continue
                self.debug(line)
            if self._count == 0:
                self._count = 1

        def stop(self): self.mgt.stop()

    class TRANS(_nc):
        def __init__(self, server='localhost'):
            super(_mgtdram_nc.TRANS, self).__init__((server, _mgt_log_port))

    def mgt_run_shot_log(self, num_blks, debug=False):
        self.mgt_run_shot(num_blks)
        return self.run_shot_logger(self._server[0], debug=debug)

    def __init__(self, server='localhost'):
        super(_mgtdram_nc, self).__init__(server, 14)
        self.trans = self.TRANS(server)

    def mgt_offload(self, *blks):
        if not blks:
            self('mgt_offload')
        elif len(blks) == 1:
            self('mgt_offload %d' % (int(blks[0])))
        else:
            self('mgt_offload %d-%d' % (int(blks[0]), int(blks[1])))

    def mgt_run_shot(self, num_blks):
        self('mgt_run_shot %d' % (int(num_blks),))

    def mgt_taskset(self, *args): self('mgt_taskset', *args)
    def mgt_abort(self): self.sock.send('mgt_abort\n')


# -------------------------
#  3 _dtacq device classes
# -------------------------
class _dtacq(object):
    @property
    def settings(self): return self.nc.settings

# STREAMING


class STREAM_MODE:
    """Define streaming modes."""

    OFF = 0
    ETH = 1
    MGT = 2


class TRIG_MODE:
    """Define trigger modes."""

    RUN = 0
    RGM = 2
    RTM = 3


class _streaming(ABC):
    @abstractmethod
    def get_dt(self): pass
    @abstractproperty
    def es_clock_div(self): pass
    active_streams = (0,)
    __streams = {}
    use_eth = False
    use_mgt = False
    has_es = False
    @property
    def id(self): return "%s_%03d" % (self._setting_host, self._setting_shot)
    @property
    def _streams(self): return self._streaming__streams.get(self.id, None)
    @_streams.setter
    def _streams(self, value):
        if isinstance(value, (set,)):
            self._streaming__streams[self.id] = value
        elif self.id in self._streaming__streams:
            self._streaming__streams.pop(self.id)

    class Stream(threading.Thread, ABC):
        @abstractmethod
        def buffers(self): pass  # generator yielding
        # numpy.array([sample, channel], '<i2', order='C')
        @abstractproperty
        def clock(self): pass  # in Hz
        @abstractproperty
        def nSamples(self):
            """Get number of Channels."""
            pass  # chunk size

        @abstractproperty
        def nChannels(self):
            """Get number of Channels."""
            pass

        def store(self, i0, block):
            """Store block[sample,channel]."""
            self.dev.dprint(
                0, '%s.store %s', self.__class__.__name__, NotImplemented)
        triggered = False

        def __init__(self, dev, name, share):
            if name is None:
                name = "%s(%s)" % (self.__class__.__name__, dev)
            super(_streaming.Stream, self).__init__(name=name)
            self.stopped = threading.Event()
            self.aborted = threading.Event()
            self.name = name
            self.dev = dev
            self.share = share

        @property
        def has_es(self): return self.dev.has_es
        @property
        def on(self): return not self.stopped.is_set()
        def stop(self): self.stopped.set()

        def abort(self):
            traceback.print_stack()
            self.aborted.set()
            self.stop()

        def run(self):
            if self.has_es:
                return self.run_es()
            self.cur_idx = 0
            for a in self.buffers():
                self.store(self.cur_idx, a)
                self.cur_idx += a.shape[0]

        def run_es(self):
            clock_div = self.dev.es_clock_div
            self.cur_idx = 0
            self.trg_count = None

            def reset_remaining():
                self.es_remaining = numpy.zeros((0, self.nChannels), 'int16')

            def store_if():
                if self.es_remaining.shape[0] >= self.nSamples:
                    self.store(self.cur_idx,
                               self.es_remaining[:self.nSamples, :])
                    self.cur_idx += self.nSamples
                    self.es_remaining = self.es_remaining[self.nSamples:, :]

            def store_rest():
                if self.es_remaining.size > 0:
                    self.store(self.cur_idx, self.es_remaining)
                    reset_remaining()

            def find_marks(a):
                if a.size == 0:
                    return numpy.empty(0, 'bool')
                return numpy.nonzero(
                        (a[:, _es_marker.static] == _es_marker.int16.static)
                        .all(1)
                    )[0]

            def get_count(a, mark):
                return int(
                    numpy.frombuffer(a[mark, _es_marker.count].data, 'int32')
                    [0])
            reset_remaining()
            for a in self.buffers():
                pos = 0
                for mark in find_marks(a):
                    count = get_count(a, mark)
                    if self.trg_count is None:
                        self.trg_count = count
                    else:
                        self.es_remaining = numpy.concatenate(
                                (self.es_remaining, a[pos:mark, :]), 0)
                        store_if()
                        store_rest()
                        self.cur_idx = (count-self.trg_count)//clock_div
                    pos = mark + 1
                if pos < self.nSamples:
                    self.es_remaining = numpy.concatenate(
                            (self.es_remaining, a[pos:, :]), 0)
                    store_if()
            store_rest()

    def streaming_arm(self, before, stream, after):
        if self._streams is not None:
            raise Exception("Streams already initialized.")
        self.share = {'lock': threading.Lock()}
        before()
        streams = set([])
        for idx in self.active_streams:
            streams.add(stream(self, idx, self.share))
        for stream in streams:
            stream.start()
        self._streams = streams
        after()

    def streaming_store(self, stop, store, storeif, deinit):
        try:
            stop()
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception:
            pass
        store()
        if self._streams is None:
            raise Exception("INV_SETUP")
        streams = list(self._streams)
        triggered = any(stream.triggered for stream in streams)
        if triggered:
            storeif()
            for stream in streams:
                stream.stop()
        else:
            for stream in streams:
                stream.abort()
        for stream in streams:
            stream.join()
        self._streams = None
        deinit()
        if not triggered:
            raise Exception("NOT_TRIGGERED")

    def streaming_deinit(self, abort, deinit):
        try:
            abort()
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception:
            pass
        if self._streams is not None:
            streams = list(self._streams)
            for stream in streams:
                stream.abort()
            for stream in streams:
                stream.join()
            self._streams = None
        deinit()


class _streaming_eth(_streaming):
    _setting_stream = STREAM_MODE.OFF
    @property
    def use_stream(self): return self._setting_stream > 0

    def before_stream_eth(self):
        self.nc.STREAM_OPTS(subset=self.subset, nowhere=False)
        self.nc.OVERSAMPLING = 0

    def after_stream_eth(self):
        def state():
            ste = self.nc('state')
            if not ste:
                return 0
            return int(ste.split(' ', 1)[0])
        t = 15
        while state() < 1:
            t -= 1
            if t <= 0:
                break
            time.sleep(1)
        if self._master._soft_out:
            self.soft_trigger()

    def stop_stream_eth(self):
        self.nc.CONTINUOUS_start()
        self.nc.CONTINUOUS_stop()

    def deinit_stream_eth(self): pass

    class Stream_eth(_streaming.Stream):
        nSamples = 0x40000
        nChannels = "set in __init__"
        clock = "set in __init__"

        def __init__(self, dev, idx, share):
            super(_streaming_eth.Stream_eth, self).__init__(dev, "eth", share)
            self.first_channel, self.nChannels = self.dev.subset
            self.clock = self.dev._setting_clock
            self.sock = _stream_nc(self.dev._setting_host).sock
            time.sleep(2)

        @property
        def chanlist(self):
            """List active channels indexes starting with 0."""
            return range(self.first_channel-1,
                         self.first_channel-1 + self.nChannels)

        @property
        def trigger(self): return int(self.dev._setting_trigger)

        def recv(self, sock, req_bytes, timeout=True):
            buf = bytearray(req_bytes)
            view = memoryview(buf)
            rem_bytes = req_bytes
            while rem_bytes > 0 and not self.aborted.is_set():
                try:
                    nbytes = sock.recv_into(view, rem_bytes)
                except socket.timeout:
                    if timeout:
                        if self.stopped.is_set():
                            raise
                        continue
                    if rem_bytes == req_bytes:
                        continue
                    else:
                        break
                if not nbytes:
                    break
                view = view[nbytes:]
                rem_bytes -= nbytes
            if rem_bytes:
                read = req_bytes-rem_bytes
                return read, buf[:read]
            return req_bytes, buf

        def buffers(self):
            buflen = self.nSamples*self.nChannels*2
            # should be multiple of bufferlen
            shape = (self.nSamples, self.nChannels)
            try:
                read = 0
                self.sock.settimeout(1)
                # wait for trigger
                read, buf = self.recv(self.sock, buflen, False)
                if read == 0:
                    return
                self.triggered = True
                while read == buflen:
                    yield numpy.frombuffer(buf, '<i2').reshape(shape)
                    self.dev.dprint(5, "stream_eth yielded %r", shape)
                    if self.aborted.is_set():
                        return
                    read, buf = self.recv(self.sock, buflen, True)
                else:  # store rest
                    samples = read//self.nChannels//2
                    if samples > 0:
                        yield numpy.frombuffer(
                            buf[:self.nChannels*samples*2], '<i2'
                        ).reshape((self.nChannels, samples))
                        self.dev.dprint(3, "stream_eth yielded %r", (
                            self.nChannels, samples))
            except socket.timeout:
                self.dev.dprint(3, "stream_eth timeout")
            finally:
                self.sock.close()

    def store_sites(self):
        for site in self._active_mods:
            self.getmodule(site).store()
        self._master.store_master()

    def store_scale(self):
        off_slo = self.off_slo
        for idx in range(self._num_channels):
            ch = idx+1
            node = self.getchannel(ch)
            if node.on:
                print("x * %g + %g" % off_slo[idx])

    def streaming_arm(self):
        return _streaming.streaming_arm(
            self,
            before=self.before_stream_eth,
            stream=self.Stream_eth,
            after=self.after_stream_eth,
        )

    def streaming_store(self):
        return _streaming.streaming_store(
            self,
            stop=self.stop_stream_eth,
            store=self.store_sites,
            storeif=self.store_scale,
            deinit=self.deinit_stream_eth,
        )

    def streaming_deinit(self):
        return _streaming.streaming_deinit(
            self,
            abort=self.stop_stream_eth,
            deinit=self.deinit_stream_eth,
        )


class _streaming_mgt482(_streaming_eth):
    @_cached_property
    def mgt(self):
        return (
            _mgt482_nc(self._setting_host, 0),
            _mgt482_nc(self._setting_host, 1),
        )

    def init_mgt(self, A=None, B=None, a=0, b=1):
        self.mgt[0].setup(A, a)
        self.mgt[1].setup(B, b)
        if self.use_mgt:
            self.mgt[0]._init()
            self.mgt[1]._init()

    def get_port_name(self, port): return 'AB'[port]
    def get_sites(self, i): return self.mgt[i]._sites
    def get_devid(self, i): return self.mgt[i]._devid
    @_cached_property
    def active_streams(self):
        if self.use_mgt:
            return tuple(i for i in range(2) if self.get_sites(i))
        return super(_streaming_mgt482, self).active_streams

    @property
    def use_eth(self): return self._setting_stream == STREAM_MODE.ETH
    @property
    def use_mgt(self): return self._setting_stream == STREAM_MODE.MGT
    def _blockgrp(self, i): return len(self.get_sites(i))
    @classmethod
    def get_mgt_sites(cls):
        ai = set()
        for l in cls._mgt.values():
            for s in l:
                ai.add(s)
        return tuple(ai)

    class Stream_mgt(_streaming.Stream):
        nSamples = "set in buffers"
        nChannels = "set in __init__"
        clock = "set in __init__"
        _folder = "/data"
        dostore = False
        post = -1  # for infinite
        chanlist = None
        _lib = None
        @property
        def lib(self):
            if _streaming_mgt482.Stream_mgt._lib is None:
                _streaming_mgt482.Stream_mgt._lib = ctypes.CDLL(
                    'librtm-t-stream.so')
            return _streaming_mgt482.Stream_mgt._lib

        def wait4ready(self):
            while not self._ready:
                time.sleep(1)

        def stop(self):
            super(_streaming_mgt482.Stream_mgt, self).stop()
            self.lib.RtmStreamStop(self.HANDLE)

        def __init__(self, dev, lane, share):
            self.devid = dev.get_devid(lane)
            super(_streaming_mgt482.Stream_mgt, self).__init__(
                    dev, "mgt%d" % self.devid, share)
            self.post = self.dev._setting_post
            self.blockgrp = self.dev._blockgrp(lane)
            self.chanlist = []
            num_channels = 0
            for s in self.dev.get_sites(lane):
                off = self.dev._channel_offset[s-1]
                num = self.dev.getmodule(s)._num_channels
                self.chanlist.extend(range(off, off+num))
                num_channels += num
            # blockgrp blocks of typically 4MB
            self.nChannels = num_channels
            self.clock = self.dev._setting_clock
            self.HANDLE = ctypes.c_void_p()

        def buffers(self):
            # start stream
            buflen = ctypes.c_int32(-1)
            if self.lib.RtmStreamStart(
                    ctypes.byref(self.HANDLE),
                    self.devid, 0, ctypes.byref(buflen)):
                raise Exception("start_stream")
            try:
                if debug > 0:
                    dprint("[%s] enter main loop: blockgrp=%d",
                           self.name, self.blockgrp)
                BUFLEN = int(buflen.value)
                size = BUFLEN * self.blockgrp
                self.nSamples = size//self.nChannels//2
                idx, max_idx = 0, self.post//self.nSamples
                shape = (self.nSamples, self.nChannels)
                while not self.aborted.is_set():
                    buffer = (ctypes.c_char*size)()
                    for off in range(0, size, BUFLEN):
                        if self.lib.RtmStreamGetBuffer(
                            self.HANDLE, ctypes.byref(buffer, off), buflen):
                                return
                        self.triggered = True
                    yield numpy.frombuffer(
                        buffer, dtype=numpy.int16).reshape(shape)
                    if self.post >= 0 and idx >= max_idx:
                        break
                    idx += 1
            except Exception:
                dprint("[%s] exception while block %d was processed",
                       self.name, idx)
                traceback.print_exc()
            finally:
                dprint("[%s] block %d stored", self.name, idx)
                if debug > 0:
                    dprint("[%s] exited main loop\n\t" +
                           "post = %d, idx = %d, max_idx = %d\n\t" +
                           "aborted = %s, stopped = %s",
                           self.name, self.post, idx, max_idx, self.aborted.is_set(), self.stopped.is_set())
                self.lib.RtmStreamClose(self.HANDLE)

    def before_stream_mgt(self):
        subset = 0+1, self._num_channels  # TODO: subset
        self.nc.STREAM_OPTS(subset=subset, nowhere=True)
        self.nc.OVERSAMPLING = 0

    def after_stream_mgt(self):
        self.nc.CONTINUOUS_start()
        self.after_stream_eth()

    def stop_stream_mgt(self):
        self.nc.CONTINUOUS_stop()

    def deinit_stream_mgt(self):
        pass

    def streaming_arm(self):
        if self.use_eth:
            return _streaming_eth.streaming_arm(self)
        return _streaming.streaming_arm(
            self,
            before=self.before_stream_mgt,
            stream=self.Stream_mgt,
            after=self.after_stream_mgt,
        )

    def streaming_store(self):
        if self.use_eth:
            return _streaming_eth.streaming_store(self)
        return _streaming.streaming_store(
            self,
            stop=self.stop_stream_mgt,
            store=self.store_sites,
            storeif=self.store_scale,
            deinit=self.deinit_stream_mgt,
        )

    def streaming_deinit(self):
        if self.use_eth:
            return _streaming_eth.streaming_deinit(self)
        return _streaming.streaming_deinit(
            self,
            abort=self.stop_stream_mgt,
            deinit=self.deinit_stream_mgt,
        )


class _carrier(_dtacq, _streaming):
    is_test = True
    _setting_host = 'dtacq'
    _setting_trigger = 0
    _setting_trigger_edge = 'rising'
    _setting_clock_src = None
    _setting_clock = 1000000
    _setting_shot = 0
    _setting_pre = 0
    _setting_post = 0
    ai_sites = None
    ao_sites = None
    @property
    def es_clock_div(self): return self._master._clkdiv_fpga

    def __init__(self, host='dtacq', **kw):
        self._setting_host = host
        for fun, args in kw.items():
            if not fun.startswith('setup_'):
                continue
            if not hasattr(self, fun):
                continue
            setup = getattr(self, fun)
            if not callable(setup):
                continue
            setup(*args)
    _nc_class = _carrier_nc
    @_cached_property
    def nc(self):
        return self._nc_class(self._setting_host, self.ai_sites, self.ao_sites)

    @property
    def simulate(self): return self._master.nc.simulate
    @simulate.setter
    def simulate(self, val): self._master.nc.simulate = val
    @property
    def _default_clock(self): self._master._default_clock
    @property
    def is_ext_clk(self): return self._setting_clock_src is not None
    @property
    def clock_ref(self): return self._setting_clock
    @property
    def has_es(self): return self._master._es_enable

    @staticmethod
    def _assemble(cls, module, num_modules=1):
        cls.module_class = module
        cls.num_modules = num_modules
        if module._is_ai:
            cls.ai_sites = range(1, cls.num_modules+1)
            def trigger_pre(self): return self._setting_pre
            cls.trigger_pre = property(trigger_pre)
            def trigger_post(self): return self._setting_post
            cls.trigger_post = property(trigger_post)
            cls.arm = cls._arm_acq
            cls.store = cls._store_acq
            cls.deinit = cls._deinit_acq
        else:  # is ao420
            cls.ao_sites = range(1, cls.num_modules+1)
            cls.arm = cls._arm_ao
        cls._num_channels = 0
        cls._channel_offset = []
        for m in range(1, cls.num_modules+1):
            cls._channel_offset.append(cls._num_channels)
            def getmodule(self): return self.getmodule(m)
            setattr(cls, 'module%d' % (m), property(getmodule))
            cls._num_channels += module._num_channels

    @property
    def max_clk(self): return self._master.max_clk
    @_cached_property
    def _active_mods(self):
        return tuple(i for i in self.modules if self.getmodule(i).on)

    @property
    def modules(self): return range(1, self.num_modules+1)
    @_cached_property
    def _optimal_sample_chunk(self):
        return self.nc.bufferlen//self.nc.nchan//2

    @property
    def _num_active_mods(self): return len(self._active_mods)

    def _channel_port_offset(self, site):
        offset = 0
        for i in self.modules:
            if i == site:
                break
            offset += self.getmodule(i)._num_channels
        return offset

    def getmodule(self, site):
        return self.module_class(self, site=site)

    def getchannel(self, idx, *suffix):
        site = (idx-1)//self.module_class._num_channels+1
        ch = (idx-1) % self.module_class._num_channels+1
        return self.getmodule(site).getchannel(ch, *suffix)

    @property
    def subset(self):
        return 0+1, self._num_channels

    @property
    def _master(self): return self.getmodule(1)
    @property
    def state(self): return self.nc.state['state']
    def _slice(self, ch): return slice(None, None, 1)

    def soft_trigger(self):
        """Send out a trigger on the carrier's internal trigger line (d1)."""
        self.nc.soft_trigger()  # SIG.SOFT_TRIGGER()

    def reboot(self):
        self.nc.reboot()

    def filter_commands(self, dic):
        if 0 in dic:
            c = dic[0]
            for k in self.nc._exclude:
                try:
                    del(c[k])
                except KeyError:
                    pass
        return dic

    def store_commands(self, dic):
        self.commands = dic

    def init(self, ext_clk=None, clock=1000000, pre=0, post=(1 << 20),
             soft_out=False, di=1):
        del(self.nc)
        _dtacq_nc.add_cache(self._setting_host)
        self._setting_clock = int(clock)
        self._setting_clock_src = ext_clk
        self._setting_pre = int(pre)
        self._setting_post = int(post)
        self._master._setting_soft_out = bool(soft_out)
        self._init()
        dic = _dtacq_nc.remove_cache(self._setting_host)
        self.store_commands(self.filter_commands(dic))

    def _init(self):
        """Initialize all device settings."""
        ext = self.is_ext_clk
        mb_fin = self._setting_clock_src if ext else _zclk_freq
        clock = self._setting_clock
        mb_set = self._master.getMB_SET(clock)
        soft_out = self._master._soft_out
        post, pre = self._setting_post, self._setting_pre
        threads = [threading.Thread(
            target=self.nc._init,
            args=(ext, mb_fin, mb_set, pre, post,
                  soft_out, self._demux, self._setting_shot)
        )]
        for i in self._active_mods:
            threads.append(self.getmodule(i).init_thread(self))
        for thread in threads:
            thread.start()
        for thread in threads:
            thread.join()
        self._master.init_master(pre, self.is_test, int(mb_set/clock))
        if issubclass(self.module_class, _acq480):
            self.nc.acq480_train()
        if debug > 0:
            dprint('Device is initialized with modules %s',
                   str(self._active_mods))

    def _arm_acq(self, timeout=50):
        """Arm the device for acq modules."""
        if self.use_stream:
            return self.streaming_arm()
        timeout = int(timeout)
        return self.nc.wait4arm(timeout)

    def _arm_ao(self, ao_mode=AO_MODE.ONE_SHOT):
        """Arm the device for ao modules."""
        try:
            self.nc.SET_AO_EXPR(ao_mode)
        finally:
            self.nc.set_arm()

    def _store_acq(self, abort=False):
        if self.use_stream:
            return self.streaming_store()
        for site in self._active_mods:
            self.getmodule(site).store()
        self._master.store_master()
        for i in range(5):
            state = self.state
            if not (state == STATE.ARM or state == STATE.PRE):
                break
            time.sleep(1)
        else:
            return False
        if abort:
            self.nc.wait4abort()
        else:
            self.nc.wait4post(self._setting_post)
            self.nc.wait4state(STATE.STOP)
        if self._demux:
            if self._master._es_enable:
                self._transfer_demuxed_es()
            else:
                self._transfer_demuxed()
        else:
            self._transfer_raw()
        return True

    def _deinit_acq(self):
        if self.use_stream:
            return self.streaming_deinit()
        self.nc.wait4abort(30)

    @property
    def slo(self): return numpy.concatenate(
                [self.getmodule(i).data_scales for i in self.modules])

    @property
    def off(self): return numpy.concatenate(
            [self.getmodule(i).data_offsets for i in self.modules])

    @property
    def off_slo(self): return self.nc.off_slo(self.modules)

    class _Downloader(threading.Thread):
        def __init__(self, dev, chanlist, lock, queue):
            super(_carrier._Downloader, self).__init__()
            self.nc = dev.nc
            self.list = chanlist
            self.lock = lock
            self.queue = queue

        def run(self):
            while True:
                with self.lock:
                    if not self.list:
                        break
                    ch, slice = self.list.popitem()
                    slice, on = slice
                raw = None
                if on:
                    try:
                        raw = self.nc.channel(ch).raw(slice)
                    except (SystemExit, KeyboardInterrupt):
                        raise
                    except Exception:
                        traceback.print_exc()
                self.queue.put((ch, raw, on))

    def _get_chanlist(self):
        """Get a list of available channels with their corresponding module."""
        nchan = self._num_channels
        chanlist = {}
        for ch in range(1, nchan+1):
            chanlist[ch] = (self._slice(ch), self.getchannel(ch).on)
        return (chanlist, queue.Queue(nchan))

    def _start_threads(self, chanlist, queue):
        """Start up to 8 threads for pulling the data."""
        lock = threading.Lock()
        threads = []
        for t in range(2):
            threads.append(self._Downloader(self, chanlist, lock, queue))
        for t in threads:
            t.start()
        return threads

    def get_dt(self): return 1000000000/self._setting_clock

    def _get_dim_slice(self, i0, start, end, pre, mcdf=1):
        """Calculate the time-vector."""
        first = int(i0-pre)
        dt = self.get_dt()
        trg = self._setting_trigger
        def dmx(i): return int(i*dt+trg)

        def dim(first, last):
            for i in range(first, last+1):
                yield dmx(i)
        return slice(start, end), first, dim, dmx

    def _store_channels_from_queue(self, dims_slice, queue):
        """Store data in queue and time_vector to tree with linked segments.

        dims_slice: tuple
            range of the data vector.
        queue: Queue
            dict of ch:raw data.
        """
        import matplotlib.pyplot as pp
        chunksize = 1 << 18
        off_slo = self.off_slo
        for i in range(queue.maxsize):
            ch, value, on = queue.get()
            if value is None or not on:
                continue
            node = self.getchannel(ch)
            def scale(x): return x*off_slo[ch-1][1]+off_slo[ch-1][0]
            for slc, start, dimfun, dmx in dims_slice:
                val = value[slc]
                dlen = val.shape[0]
                for seg, is0 in enumerate(range(0, dlen, chunksize)):
                    is1 = min(is0+chunksize, dlen)-1
                    i0, i1 = start+is0, start+is1
                    dim, dm0, dm1 = dimfun(i0, i1), dmx(i0), dmx(i1)
                    if debug > 1:
                        dprint("segment (%7.1fms,%7.1fms)", dm0/1e6, dm1/1e6)
                    print((node.idx, scale(val[is0]), dm0, dm1,
                           off_slo[ch-1][0], off_slo[ch-1][1],
                           dim, val[is0:is1+1].shape))
                    if ch == 1:
                        pp.plot(list(dim), val[is0:is1+1])
        pp.show()

    def _transfer_demuxed_es(self):
        """Grab the triggered channels, opens a socket and reads out data."""
        if debug:
            dprint('transfer_demuxed_es')
        chanlist, queue = self._get_chanlist()

        def findchaninlist(slc, skip=0):
            channels = range(1, len(chanlist)+1)[slc][skip:]
            for n in channels:
                if n in chanlist:
                    return n
            return channels[0]
        # 1&2 sample index, 5&6 clock count
        # TODO: how to do clock count in 4CH mode of 480
        lo = findchaninlist(_es_marker.count_l)  # 5
        hi = findchaninlist(_es_marker.count_h)  # 6
        s0 = findchaninlist(_es_marker.static, 0)  # 3
        s1 = findchaninlist(_es_marker.static, 1)  # 7
        if s0 == s1:
            s0 = 4

        def get_and_queue_put(ch):
            slice, on = chanlist.pop(ch)
            raw = self.nc.channel(ch).raw(slice)
            queue.put((ch, raw, on))
            return raw
        loa = get_and_queue_put(lo)
        hia = get_and_queue_put(hi)
        s0a = get_and_queue_put(s0)
        s1a = get_and_queue_put(s1)
        threads = self._start_threads(chanlist, queue)
        mask = ((s0a == _es_marker.int16.static) &
                (s1a == _es_marker.int16.static))
        index = numpy.nonzero(mask)[0].astype('int32').tolist()
        index.append(loa.shape[0]-1)
        loa = loa[mask].astype('uint16').astype('uint32')
        hia = hia[mask].astype('uint16').astype('uint32')
        tt0 = (loa+(hia << 16))//self.es_clock_div
        tt0 = (tt0-tt0[0]).tolist()
        pre = self._setting_pre
        # i0 to shift time vector as well, index[i]+1 to skip marker
        dims_slice = [
            self._get_dim_slice(i0, index[i]+1, index[i+1], pre)
            for i, i0 in enumerate(tt0)
        ]
        self._store_channels_from_queue(dims_slice, queue)
        for t in threads:
            t.join()

    def _transfer_demuxed(self):
        """Grab the triggered channels, opens a socket and reads out data."""
        if debug:
            dprint('transfer_demuxed')
        chanlist, queue = self._get_chanlist()
        threads = self._start_threads(chanlist, queue)
        pre = self._setting_pre
        dlen = pre+self._setting_post
        dims_slice = [self._get_dim_slice(0, 0, dlen, pre)]
        self._store_channels_from_queue(dims_slice, queue)
        for t in threads:
            t.join()


class _acq1001(_carrier, _streaming_eth):
    _nc_class = _acq1001_nc
    @property
    def _demux(self): return 0 if self.use_stream else 1


class _acq2106(_carrier, _streaming_mgt482):
    _nc_class = _acq2106_nc
    @property
    def _demux(self): return 0 if self.use_mgt or self.use_eth else 1

    def _arm_acq(self, *a, **kw):
        if self.use_mgt:
            return self.streaming_arm()
        super(_acq2106, self)._arm_acq(*a, **kw)

    def _store_acq(self, *a, **kw):
        if self.use_mgt:
            return self.streaming_store()
        super(_acq2106, self)._store_acq(*a, **kw)

    def _deinit_acq(self, *a, **kw):
        if self.use_mgt:
            return self.streaming_deinit()
        super(_acq2106, self)._deinit_acq(*a, **kw)

    def calc_Mclk(self, clk, maxi, mini, decim):
        """Calculate the Mclk.

        based on user-specified clock and the available filter decim values.
        To be used for future check/automatic Mclk calculation functionality
        """
        dec = 2**int(numpy.log2(maxi/clk))
        Mclk = clk*dec
        if dec < min(decim) or dec > max(decim) or Mclk < mini or Mclk > maxi:
            raise ValueError("Impossible clock value, " +
                             "try another value in range " +
                             "[1.25, %d] MHz" % int(maxi/1e6))
        return int(Mclk), int(dec)

    def check(self):
        fir = self.getmodule(0).nc.ACQ480.FIR[1]
        for mod in self._active_mods:
            try:
                if self.getmodule(mod+1).nc.ACQ480.FIR[1] != fir:
                    print('INCORRECT DECIMATION VALUES')
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception:
                pass

    def arm(self, timeout=50):
        try:
            super(_acq2106, self).arm(timeout)
        except Exception:
            if self.nc.SYS.CLK.LOL:
                dprint('MCLK: Loss Of Lock!')
            raise


class _module(_dtacq):
    on = True
    _clkdiv_fpga = 1
    _es_enable = False
    _setting_soft_out = False
    head = None
    site = None
    _init_done = False

    def __init__(self, head, site):
        self.head = head
        self.site = site
        self._init_done = True
    _nc_class = _module_nc
    @_cached_property
    def nc(self): return self._nc_class(self._setting_host, self.site)
    @property
    def _soft_out(self): return self._setting_soft_out

    def getchannel(self, ch, *suffix):
        chan = getattr(self, "channel%02d" % ch)
        if not suffix:
            return chan
        return getattr(chan, '_'.join(suffix))

    def setchannel(self, ch, val, *suffix):
        chan = getattr(self, "channel%02d" % ch)
        if not suffix:
            return chan
        return setattr(chan, '_'.join(suffix), val)

    @property
    def max_clk(self): return self._max_clk
    @property
    def _default_clock(self): return self.max_clk
    @property
    def is_master(self): return self.site == 1
    @property
    def _carrier(self): return self.head  # Node pointing to host carrier nid
    @property
    def _trigger(self): return self._carrier._trigger
    @_cached_property
    def _setting_host(self): return self._carrier._setting_host
    @property
    def state(self): return self._carrier.state
    @property
    def _setting_pre(self): return int(self._carrier._setting_pre)
    @property
    def _setting_post(self): return int(self._carrier._post)
    @property
    def max_clk_in(self): return self.getMB_SET(self.max_clk)

    # Action methods
    def init(self): self._init(*self.init_args)
    def _init(self, *args): self.nc._init(*args)

    def init_thread(self, carrier):
        return threading.Thread(target=self._init, args=self.init_args)


class _acq4xx(_module):
    """Common class for e.g. acq425 or acq480 but not ao420."""

    _setting_trig_mode = 0
    _setting_trig_mode_translen = 1048576
    @property
    def _soft_out(self):
        return (self._setting_soft_out or
                0 < self.nc._trig_mode(self._setting_trig_mode))

    @property
    def _es_enable(self): return self.nc.es_enable
    @property
    def off(self): return self.nc.off
    @property
    def slo(self): return self.nc.slo
    @property
    def off_slo(self): return self.nc.off_slo


def _module_class(cls):
    """Set up module - decorator."""
    for ch in range(1, cls._num_channels+1):
        class channelx(cls._channel_class):
            on = True
            idx = ch
        setattr(cls, "channel%02d" % ch, channelx)
    return cls


@_module_class
class _acq425(_acq4xx):
    """D-tAcq ACQ425ELF 16 channel transient recorder.

    http://www.d-tacq.com/modproducts.shtml
    """

    _is_ai = True
    _nc_class = _acq425_nc
    _num_channels = 16
    _max_clk = 2000000
    _default_clock = 1000000

    class _channel_class(object):
        gain = 1

    def _setting_gain(self, i): return self.getchannel(i, 'gain')
    def getMB_SET(self, clock): return 50000000
    """ Action methods """
    @property
    def init_args(self):
        return (
            [self._setting_gain(ch)
             for ch in range(1, self._num_channels+1)],
            self._carrier._setting_shot,
        )

    def init_master(self, pre, soft, clkdiv):
        self.nc._init_master(pre, soft, clkdiv)

    def store(self): pass
    def store_master(self): pass


@_module_class
class _acq480(_acq4xx):
    """D-tAcq ACQ480 8 channel transient recorder.

    http://www.d-tacq.com/modproducts.shtml
    """

    _is_ai = True
    _nc_class = _acq480_nc
    _num_channels = 8

    class _channel_class:
        gain = 100
        invert = False
        hpf = 0
        lfns = 0
        t50r = False

    @property
    def max_clk(self): return self.nc._max_clk_dram(self.parent.num_modules)
    @property
    def _clkdiv_adc(self): return self.nc._clkdiv_adc(self._setting_fir)
    @property
    def _clkdiv_fpga(self): return self.nc._clkdiv_fpga
    _setting_fir = 0
    def getMB_SET(self, clock): return self.nc._clkdiv(self._setting_fir)*clock

    @property
    def init_args(self):
        return (
            [self.getchannel(ch, 'gain')
             for ch in range(1, self._num_channels+1)],
            [self.getchannel(ch, 'invert')
             for ch in range(1, self._num_channels+1)],
            [self.getchannel(ch, 'hpf')
             for ch in range(1, self._num_channels+1)],
            [self.getchannel(ch, 'lfns')
             for ch in range(1, self._num_channels+1)],
            [self.getchannel(ch, 't50r')
             for ch in range(1, self._num_channels+1)],
            self._carrier._setting_shot
        )

    def init_master(self, pre, soft, clkdiv):
        self.nc._init_master(
                pre, soft,
                self._setting_trig_mode,
                self._setting_trig_mode_translen,
                self._setting_fir)
        fir = self.nc.ACQ480.FIR.DECIM
        fpga = self.nc.ACQ480.FPGA.DECIM
        self.clkdiv_fir = fir
        self.clkdiv_fpga = fpga
        # check clock
        clk = self._carrier._setting_clock
        if clk*fpga < 10000000:
            raise Exception('Bus clock must be at least 10MHz')
        clk_adc = clk*fpga*fir
        if clk_adc > 80000000:
            raise Exception("ADC clock cannot exceed 80MHz: " +
                            "is %g = %g * %g * %g" % (clk_adc, clk, fpga, fir))

    def store(self): pass
    def store_master(self): pass

    # Channel settings
    def _setting_invert(self, ch): return int(self.getchannel(ch, 'invert'))
    def _setting_gain(self, ch): return int(self.getchannel(ch, 'gain'))
    def _setting_hpf(self, ch): return int(self.getchannel(ch, 'hpf'))
    def _setting_lfns(self, ch): return int(self.getchannel(ch, 'lfns'))
    def _setting_t50r(self, ch): return int(self.getchannel(ch, 't50r'))

    def _setting_setInvert(self, ch):
        self.nc.ACQ480.INVERT[ch] = self._setting_invert(ch)

    def _setting_setGain(self, ch):
        self.nc.ACQ480.GAIN[ch] = self._setting_gain(ch)

    def _setting_setHPF(self, ch):
        self.nc.ACQ480.HPF[ch] = self._setting_hpf(ch)

    def _setLFNS(self, ch):
        self.nc.ACQ480.LFNS[ch] = self._setting_lfns(ch)


@_module_class
class _ao420(_module):
    _is_ai = False
    _max_clk = 1000000
    _nc_class = _ao420_nc
    _num_channels = 4

    class _channel_class:
        expr = "0"   # in
        offset = None  # out
        gain = None  # out
        range = None  # out

    def getMB_SET(self, clock): return 50000000
    def _channel(self, i): return self.getchannel(i)

    @property
    def init_args(self):
        return (
            [self._setting_expr(ch)
             for ch in range(1, self._num_channels+1)],
            self._carrier._setting_shot,
        )

    def init_master(self, pre, soft, clkdiv):
        self.nc._init_master(soft, clkdiv)

    def _init(self, *args):
        ans = self.nc._init(*args)
        for ch in range(self._num_channels):
            off, gain, rang, clip = ans[ch]
            self.setchannel(ch+1, numpy.int16(off), 'offset')
            self.setchannel(ch+1, numpy.int16(gain), 'gain')
            if clip:
                rang = '10V+'
            elif rang:
                rang = '10V'
            else:
                rang = '5V'
            self.setchannel(ch+1, rang, 'range')

    def _setting_expr(self, ch): return self.getchannel(ch, 'expr')


# ------------------------------
#  4 assemblies: carrier_module
# ------------------------------
def _assemble(module, num_modules=1):
    def _(cls):
        cls._assemble(cls, module, num_modules)
        return cls
    return _


@_assemble(_acq425)
class acq1001_acq425(_acq1001):
    """ACQ1001 carrier with a ACQ425 module."""


@_assemble(_acq480)
class acq1001_acq480(_acq1001):
    """ACQ1001 carrier with a ACQ480 module."""


@_assemble(_ao420)
class acq1001_ao420(_acq1001):
    """ACQ1001 carrier with a AO420 module."""


@_assemble(_acq425, 1)
class acq2106_acq425x1(_acq2106):
    """ACQ2106 carrier with one ACQ425 module."""


@_assemble(_acq425, 2)
class acq2106_acq425x2(_acq2106):
    """ACQ2106 carrier with two ACQ425 modules."""


@_assemble(_acq425, 3)
class acq2106_acq425x3(_acq2106):
    """ACQ2106 carrier with three ACQ425 modules."""


@_assemble(_acq425, 4)
class acq2106_acq425x4(_acq2106):
    """ACQ2106 carrier with four ACQ425 modules."""


@_assemble(_acq425, 5)
class acq2106_acq425x5(_acq2106):
    """ACQ2106 carrier with five ACQ425 modules."""


@_assemble(_acq425, 6)
class acq2106_acq425x6(_acq2106):
    """ACQ2106 carrier with six ACQ425 modules."""


@_assemble(_acq480, 1)
class acq2106_acq480x1(_acq2106):
    """ACQ2106 carrier with one ACQ480 module."""


@_assemble(_acq480, 2)
class acq2106_acq480x2(_acq2106):
    """ACQ2106 carrier with two ACQ480 modules."""


@_assemble(_acq480, 3)
class acq2106_acq480x3(_acq2106):
    """ACQ2106 carrier with two ACQ480 modules."""


@_assemble(_acq480, 4)
class acq2106_acq480x4(_acq2106):
    """ACQ2106 carrier with four ACQ480 modules."""


@_assemble(_acq480, 5)
class acq2106_acq480x5(_acq2106):
    """ACQ2106 carrier with five ACQ480 modules."""


@_assemble(_acq480, 6)
class acq2106_acq480x6(_acq2106):
    """ACQ2106 carrier with six ACQ480 modules."""


def test(mode=STREAM_MODE.OFF, w_ao=True, w_ai=True):
    """Test some assembleys."""
    ai, ao = None, None
    try:
        # from LocalDevices.acq4xx import acq1001_ao420, acq2106_mgtx2_acq480x2
        post = (1 << 15)*10
        if w_ai:
            ai = acq2106_acq480x4('192.168.44.255')
            # ai = acq2106_acq425x2('192.168.44.255')
            ai._setting_stream = mode
            ai.init_mgt(A=[1])
        dprint("test begin")
        if w_ao:
            sin = "sin(linspace(0,pi*2,%d)),0" % post
            cos = "cos(linspace(0,pi*2,%d)),0" % post
            ao = acq1001_ao420('192.168.44.254')
            ao.getchannel(1).expr = '1*'+sin
            ao.getchannel(2).expr = '3*'+cos
            ao.getchannel(3).expr = '-6*'+sin
            ao.getchannel(4).expr = '-12*'+cos
            ao.init(post=post, clock=1000000)
            dprint("ao init done")
        if w_ai:
            ai.init(pre=0, post=1000000, clock=1e6)
            ai._master.nc.simulate = 0 if w_ao else 1
            dprint("ai init done")
        if w_ai:
            ai.arm()
            dprint("ai arm done")
        if w_ao:
            ao.arm()
            dprint("ao arm done")
        if w_ai:
            ai.soft_trigger()
        if w_ao:
            ao.soft_trigger()
        dprint("soft_trigger done")
        if w_ai:
            time.sleep(3)
            ai.store()
            dprint("ai store done")
            ai.deinit()
            dprint("ai deinit done")
    except Exception:
        traceback.print_exc()
    return ai, ao


def test_stream_es():
    """Test stream with event markers."""
    if not os.path.exists('/data/2.00'):
        return

    class test_streaming(_streaming):
        nChannels = "set in __init__"
        es_clock_div = "set in __init__"
        has_es = True

        def __init__(self, nChannels, clock, fpga=1):
            self.nChannels = nChannels
            self.clock = clock
            self.es_clock_div = fpga

        def get_dt(self): return 1e9/self.clock

        class Stream(_streaming.Stream):
            nSamples = 0x40000
            nChannels = "set in __init__"
            clock = "set in __init__"

            def __init__(self, dev, port, share):
                super(test_streaming.Stream, self).__init__(dev, port, share)
                self.nChannels = self.dev.nChannels
                self.clock = self.dev.clock

            def buffers(self):
                for i in range(0, 24, 4):
                    filepath = "/data/2.%02d" % i
                    yield numpy.memmap(
                            filepath, dtype='int16', mode='r',
                            shape=(self.nSamples, self.nChannels))
    S = test_streaming(32, 2e6, 10)
    Ss = S.Stream(S, 0, {})
    t = time.time()
    Ss.run()
    print(time.time()-t)
    if plot_channel >= 0:
        pp.show(block=True)


try:
    import MDSplus
except Exception:
    if run_test:
        ai, ao = test()
    else:
        test_stream_es()
else:

    # ---------------------------
    #  6 Private MDSplus Devices
    # ---------------------------
    class _STREAMING(_streaming):
        class Stream(_streaming.Stream):
            def __init__(self, dev, port, share):
                super(_STREAMING.Stream, self).__init__(dev, port, share)
                self.dev = self.dev.copy()

            def run(self):
                def gen_chan_nodes():
                    for i, idx in enumerate(self.chanlist):
                        raw = self.dev.getchannel(idx+1)
                        if raw.on:
                            yield i, raw
                self.dev.tree.open()
                self._chans = tuple(gen_chan_nodes())
                self._i1_max = self.dev.trigger_post
                super(_STREAMING.Stream, self).run()

            def store(self, i0, block):
                i1 = self.dev.get_i1(i0, block.shape[0])
                if self._i1_max < 0 or i1 < self._i1_max:
                    dim, dm0, dm1 = self.dev.get_dim_set(i0, i1)
                    for i, raw in self._chans:
                        raw.makeSegment(dm0, dm1, dim, block[:, i])
                else:
                    upto, i1 = self._i1_max - i0, self._i1_max
                    dim, dm0, dm1 = self.dev.get_dim_set(i0, i1)
                    for i, raw in self._chans:
                        raw.makeSegment(dm0, dm1, dim, block[:upto, i])
                    self.dev.dprint(
                        1, "Stream.store reached requested samples.")
                    self.abort()

        def store_scale(self):
            off_slo = self.off_slo
            for idx in range(self._num_channels):
                ch = idx+1
                node = self.getchannel(ch)
                if node.on:
                    node.setSegmentScale(
                            self.get_scale(off_slo[idx][1], off_slo[idx][0]))

    class _STREAMING_ETH(_STREAMING, _streaming_eth):
        class Stream_eth(_STREAMING.Stream, _streaming_eth.Stream_eth):
            def __init__(self, dev, port, share):
                super(_STREAMING_ETH.Stream_eth, self).__init__(
                    dev, port, share)

        def streaming_eth_store(self):
            try:
                self.stop_stream_eth()
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception:
                pass
            self.store_eth_sites()
            if self._streams is None:
                raise MDSplus.DevINV_SETUP
            streams = list(self._streams)
            triggered = any(stream.triggered for stream in streams)
            if triggered:
                self.store_eth_scale()
            else:
                for stream in streams:
                    stream.stop()
            for stream in streams:
                stream.join()
            self._streams = None
            self.deinit_stream_eth()
            if not triggered:
                raise MDSplus.DevNOT_TRIGGERED

    @MDSplus.with_mdsrecords
    class _STREAMING_MGT482(_streaming_mgt482, _STREAMING_ETH):
        parts = [
            {'path': ':STREAM.MGT_A',
             'type': 'numeric',
             'options': ('no_write_shot', 'write_once'),
             'help': "Host's device id for lane A",
             'filter': int,
             'default': 0,
             },
            {'path': ':STREAM.MGT_A:SITES',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'help': "Sites streamed via lane A",
             'filter': [int],
             'default': [],
             },
            {'path': ':STREAM.MGT_B',
             'type': 'numeric',
             'options': ('no_write_shot', 'write_once'),
             'help': "Host's device id for lane B",
             'filter': int,
             'default': 1,
             },
            {'path': ':STREAM.MGT_B:SITES',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'help': "Sites streamed via lane B",
             'filter': [int],
             'default': [],
             },
        ]

        def get_devid(self, i):
            if i == 0:
                return self._stream_mgt_a
            return self._stream_mgt_b

        def get_sites(self, i):
            if i == 0:
                return self._stream_mgt_a_sites
            return self._stream_mgt_b_sites

        class Stream_mgt(_STREAMING.Stream, _streaming_mgt482.Stream_mgt):
            def __init__(self, dev, port, share):
                super(_STREAMING_MGT482.Stream_mgt, self).__init__(
                    dev, port, share)

    class _MDS_EXPRESSIONS(object):
        def get_dt(self):
            return MDSplus.DIVIDE(MDSplus.Int64(1e9), self.clock)

        def get_dim(self, i0=None, i1=None, trg=None):
            if trg is None:
                trg = self.trigger
            dt = self.get_dt()
            return MDSplus.Dimension(
                    MDSplus.Window(i0, i1, trg),
                    MDSplus.Range(None, None, dt))

        @staticmethod
        def update_dim(dim, i0, i1):
            dim[0][0], dim[0][1] = i0, i1

        def get_dmx(self, i0=None, trg=None):
            if trg is None:
                trg = self.trigger
            dt = self.get_dt()
            return MDSplus.ADD(MDSplus.MULTIPLY(i0, dt), trg)

        @staticmethod
        def update_dmx(dmx, i0):
            dmx[0][1] = i0

        @staticmethod
        def get_i1(i0, length): return i0+length-1

        def get_dim_set(self, i0=None, i1=None, trg=None):
            return (
                self.get_dim(i0, i1, trg),
                self.get_dmx(i0, trg),
                self.get_dmx(i1, trg),
            )

        @staticmethod
        def update_dim_set(dim, dm0, dm1, i0, i1):
            dim[0][0], dim[0][1] = i0, i1
            dm0[0][0], dm1[0][1] = i0, i1

        @staticmethod
        def get_scale(slope, offset):
            return MDSplus.ADD(MDSplus.MULTIPLY(
                MDSplus.dVALUE(), slope), offset)

    class _CARRIER(MDSplus.Device, _carrier, _MDS_EXPRESSIONS):
        """Class that can set the various knobs (PVs) of the D-TACQ module.

        PVs are set from the user tree-knobs.
        """

        parts = [
            {'path': ':ACTIONSERVER',
             'type': 'TEXT',
             'options': ('no_write_shot', 'write_once'),
             'filter': str,
             },
            {'path': ':ACTIONSERVER:INIT',
             'type': 'ACTION',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           'Dispatch(head.actionserver, "INIT", 51),'
                           'Method(None, "init", head))')
             },
            {'path': ':ACTIONSERVER:ARM',
             'type': 'ACTION',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           'Dispatch(head.actionserver, "ARM", 51),'
                           'Method(None, "arm", head))')
             },
            {'path': ':ACTIONSERVER:SOFT_TRIGGER',
             'type': 'ACTION',
             'options': ('no_write_shot', 'write_once', 'disabled'),
             'valueExpr': ('Action('
                           'Dispatch(head.actionserver, "PULSE", 1),'
                           'Method(None, "soft_trigger", head))')
             },
            {'path': ':ACTIONSERVER:REBOOT',
             'type': 'TASK',
             'options': ('write_once',),
             'valueExpr': 'Method(None, "reboot", head)',
             },
            {'path': ':HOST',
             'type': 'text',
             'value': 'localhost',
             'options': ('no_write_shot',),  # hostname or ip address
             'filer': str,
             },
            {'path': ':STREAM',
             'type': 'numeric',
             'value': 0,
             'options': ('no_write_shot',),
             'help': 'Stream Mode: OFF, ETH, MGT',
             'enum': {0: "OFF", 1: "ETH", 2: "MGT"},
             },
            {'path': ':TRIGGER',
             'type': 'numeric',
             'valueExpr': 'Int64(0)',
             'options': ('no_write_shot',),
             },
            {'path': ':TRIGGER:EDGE',
             'type': 'text',
             'value': 'rising',
             'options': ('no_write_shot',),
             'filer': str,
             },
            {'path': ':CLOCK',
             'type': 'numeric',
             'valueExpr': 'head._default_clock',
             'options': ('no_write_shot',),
             'filer': float,
             },
            {'path': ':CLOCK:SRC',
             'type': 'numeric',
             'valueExpr': 'head.clock_src_zclk',
             'options': ('no_write_shot',),
             'help': "reference to ZCLK or set FPCLK in Hz",
             'filer': float,
             },
            {'path': ':CLOCK:SRC:ZCLK',
             'type': 'numeric',
             'valueExpr': 'Int32(33.333e6).setHelp("INT33M")',
             'options': ('no_write_shot',),
             'help': 'INT33M',
             },
            {'path': ':COMMANDS',
             'type': 'text',
             'options': ('no_write_model', 'write_once'),
             'filer': json.loads,
             },
        ]

        @staticmethod
        def _assemble(cls, module, num_modules=1):
            _carrier._assemble(cls, module, num_modules)
            if module._is_ai:
                cls.parts = cls.parts + [
                    {'path': ':ACTIONSERVER:STORE',
                     'type': 'ACTION',
                     'options': ('no_write_shot', 'write_once'),
                     'valueExpr': ('Action('
                                   'Dispatch(head.actionserver, "STORE", 51),'
                                   'Method(None, "store", head))'),
                     },
                    {'path': ':ACTIONSERVER:DEINIT',
                     'type': 'ACTION',
                     'options': ('no_write_shot', 'write_once'),
                     'valueExpr': ('Action('
                                   'Dispatch(head.actionserver, "DEINIT", 51),'
                                   'Method(None, "deinit", head))'),
                     },
                    {'path': ':TRIGGER:PRE',
                     'type': 'numeric',
                     'value': 0,
                     'options': ('no_write_shot',),
                     },
                    {'path': ':TRIGGER:POST',
                     'type': 'numeric',
                     'value': 100000,
                     'options': ('no_write_shot',),
                     },
                ]
                @MDSplus.mdsrecord(filter=int)
                def _setting_pre(self):
                    return self.__getattr__('trigger_pre')
                cls._setting_pre = _setting_pre
                @MDSplus.mdsrecord(filter=int)
                def _setting_post(self):
                    return self.__getattr__('trigger_post')
                cls._setting_post = _setting_post
            else:  # is AO420
                cls.parts = list(_CARRIER.parts)
                cls._setting_pre = 0
                cls._setting_post = 0
            for i in range(num_modules):
                prefix = ':MODULE%d' % (i+1)
                cls.parts.extend([
                    {'path': prefix,
                     'type': 'text',
                     'value': module.__name__[1:],
                     'options': ('write_once',),
                     },
                ])
                module._addModuleKnobs(cls, prefix, i)
                if i == 0:
                    module._addMasterKnobs(cls, prefix)

        @property
        def is_test(self): return self.actionserver_soft_trigger.on
        @property
        def is_ext_clk(self):
            src = self.clock_src.record
            return not (
                isinstance(src, MDSplus.TreeNode) and
                src.nid == self.clock_src_zclk.nid)

        def getmodule(self, site):
            return self.module_class(
                self.part_dict["module%d" % site] + self.head.nid,
                self.tree, self, site=site)

        @_cached_property
        def _setting_host(self):
            try:
                host = MDSplus.version.tostr(self.host.data())
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception as ex:
                raise MDSplus.DevNO_NAME_SPECIFIED(str(ex))
            if not host:
                raise MDSplus.DevNO_NAME_SPECIFIED
            return host

        @property
        def _setting_shot(self): return self.tree.shot
        @MDSplus.mdsrecord(filter=int)
        def _setting_stream(self): return self.__getattr__('stream')
        @MDSplus.mdsrecord(filter=int)
        def _setting_clock_src(self): return self.__getattr__('clock_src')
        @MDSplus.mdsrecord(filter=int)
        def _setting_clock(self): return self.__getattr__('clock')
        @MDSplus.mdsrecord(filter=int)
        def _setting_trigger(self): return self.__getattr__('trigger')

        @MDSplus.cached_property
        def subset(self):
            num = self._num_channels
            while not self.getchannel(num).on:
                num -= 1
            start = 1
            while not self.getchannel(start).on:
                start += 1
            num += 1-start
            return start, num

        def soft_trigger(self):
            """Send out a trigger on the carrier's internal trigger line (d1).

            ACTION METHOD
            """
            try:
                self.nc.soft_trigger()
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))

        def reboot(self):
            try:
                _carrier.reboot(self)
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))

        def store_commands(self, dic):
            self.commands = json.dumps(dic)

        def init(self):
            """Initialize all device settings.

            ACTION METHOD
            """
            _dtacq_nc.add_cache(self._setting_host)
            try:
                _carrier._init(self)
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception:
                traceback.print_exc()
                raise
            finally:
                dic = _dtacq_nc.remove_cache(self._setting_host)
                self.store_commands(self.filter_commands(dic))

        def _arm_acq(self, timeout=50):
            """Arm the device for acq modules.

            ACTION METHOD
            """
            try:
                del(self.subset)
                super(_CARRIER, self)._arm_acq(timeout)
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception:
                traceback.print_exc()
                raise MDSplus.MDSplusERROR(
                    'not armed after %d seconds' % (timeout,))

        def _arm_ao(self):
            """Arm the device for ao modules.

            ACTION METHOD
            """
            try:
                super(_CARRIER, self)._arm_ao()
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))

        def _store_acq(self, abort=False):
            try:
                super(_CARRIER, self)._store_acq()
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))
            except (SystemExit, KeyboardInterrupt):
                raise
            except Exception:
                traceback.print_exc()
                raise MDSplus.DevNOT_TRIGGERED

        def _get_dim_slice(self, i0, start, end, pre, mcdf=1):
            """Calculate the time-vector."""
            first = int(i0-pre)
            last = first + end-start-1
            dim, dm0, dm1 = self.get_dim_set(first, last)
            return slice(start, end), start, dim, dm0, dm1

        def _store_channels_from_queue(self, dims_slice, queue):
            """Store data in queue with time to tree with linked segments.

            dims_slice: tuple
                range of the data vector.
            queue: Queue
                dict of ch:raw data.
            """
            chunksize = 100000
            off_slo = self.off_slo
            for i in range(queue.maxsize):
                ch, value, on = queue.get()
                if value is None or not on:
                    continue
                node = self.getchannel(ch)
                node.setSegmentScale(
                        self.get_scale(off_slo[ch-1][1], off_slo[ch-1][0]))
                for slc, start, dim, dm0, dm1 in dims_slice:
                    val = value[slc]
                    dlen = val.shape[0]
                    for seg, is0 in enumerate(range(0, dlen, chunksize)):
                        is1 = min(is0+chunksize, dlen)-1
                        i0, i1 = start+is0, start+is1
                        self.update_dim_set(dim, dm0, dm1, i0, i1)
                        if debug > 1:
                            dprint(
                                "segment (%7.1fms,%7.1fms)", dm0/1e6, dm1/1e6)
                        node.makeSegment(dm0, dm1, dim, val[is0:is1+1])

        def _deinit_acq(self):
            """Abort and go to idle.

            ACTION METHOD
            """
            try:
                super(_CARRIER, self)._deinit_acq()
            except socket.error as e:
                raise MDSplus.DevOFFLINE(str(e))

    class _ACQ1001(_CARRIER, _STREAMING_ETH, _acq1001):
        pass

    class _ACQ2106(_CARRIER, _STREAMING_MGT482, _acq2106):
        parts = _CARRIER.parts + _STREAMING_MGT482.parts

        def init(self):
            """Initialize all device settings.

            ACTION METHOD
            """
            self.init_mgt(self._stream_mgt_a_sites,
                          self._stream_mgt_b_sites,
                          self._stream_mgt_a,
                          self._stream_mgt_b)
            super(_ACQ2106, self).init()

    class _MODULE(MDSplus.TreeNode, _module):
        """MDSplus mdule superclass covering Gen.4 D-tAcq module device types.

        PVs are set from the user tree-knobs.
        """

        def __init__(self, nid, *a, **kw):
            """Mimic _module.__init__."""
            if isinstance(nid, _MODULE):
                return
            self.site = kw.get('site', None)
            if self.site is None:
                raise Exception("No site specified for acq4xx._MODULE class")
            if isinstance(nid, _MODULE):
                return
            super(_MODULE, self).__init__(nid, *a, site=self.site)
            self._init_done = True

        @classmethod
        def _addMasterKnobs(cls, carrier, prefix):
            carrier.parts.extend([
                {'path': '%s:TRIG_MODE' % (prefix,),
                 'type': 'numeric',
                 'options': ('no_write_shot',),
                 'enum': {0: 'OFF', 2: 'RGM', 3: 'RTM'},
                 },
                {'path': '%s:TRIG_MODE:TRANSLEN' % (prefix,),
                 'type': 'numeric',
                 'value': 10000,
                 'options': ('no_write_shot',),
                 'help': 'Samples acquired per trigger for RTM',
                 },
            ])

        @classmethod
        def _addModuleKnobs(cls, carrier, prefix, idx):
            carrier.parts.extend([
                {'path': '%s:CHECK' % (prefix,),
                 'type': 'TASK',
                 'options': ('write_once',),
                 'valueExpr': 'Method(None,"check",head)',
                 },
            ])

        def __getattr__(self, name):
            """Redirect Device.part_name to head."""
            partname = "module%d_%s" % (self.site, name)
            if partname in self.head.part_dict:
                return self.__class__(
                        self.head.part_dict[partname]+self.head.nid,
                        self.tree, self.head, site=self.site)
            return self.__getattribute__(name)

        def __setattr__(self, name, value):
            """Redirect Device.part_name to head."""
            if self._init_done:
                partname = "module%d_%s" % (self.site, name)
                if partname in self.head.part_dict:
                    self.head.__setattr__(partname, value)
                    return
            super(_MODULE, self).__setattr__(name, value)

        def getchannel(self, ch, *args):
            """Mimic _module.getchannel."""
            return self.__getattr__('_'.join([
                'channel%02d' % (ch+self.head._channel_offset[self.site-1],)
                ]+list(args)))

        def setchannel(self, ch, val, *args):
            """Mimic _module.setchannel."""
            return self.__setattr__('_'.join([
                'channel%02d' % (ch+self.head._channel_offset[self.site-1],)
                ]+list(args)), val)

        def get_dim(self, ch):
            # unused TODO: use in store as it supports channels.range
            """Create the time dimension for channel ch."""
            from MDSplus import MINUS, SUBTRACT, Window, Dimension
            pre = self._carrier.trigger_pre
            post = self._carrier.trigger_post
            trig = self._carrier.trigger
            win = Window(MINUS(pre), SUBTRACT(post, 1), trig)
            rang = self.getchannel(ch, 'range')
            dim = Dimension(win, rang)
            return dim

        def check(self, *chs):
            """Check if all values are valid.

            TASK_METHOD
            """
            if not chs:
                chs = range(1, self._num_channels+1)
            if self.tree.shot != -1:
                raise MDSplus.TreeNOWRITESHOT  # only check model trees
            if self.site == 1 and not self.on:
                raise MDSplus.DevINV_SETUP  # module 1 is master module
            pre, post = self._pre, self._post
            for ch in chs:
                if not self.getchannel(ch).on:
                    continue
                rang = self._range(ch)
                if not isinstance(rang, MDSplus.Range) or rang.delta < 1:
                    raise MDSplus.DevRANGE_MISMATCH
                if -pre > rang.begin:
                    raise MDSplus.DevBAD_STARTIDX
                if post < rang.ending:
                    raise MDSplus.DevBAD_ENDIDX
            return chs

    class _ACQ425(_MODULE, _acq425):
        @classmethod
        def _addMasterKnobs(cls, carrier, prefix):
            _MODULE._addMasterKnobs(carrier, prefix)
            carrier.parts.extend([
                {'path': '%s:CLKDIV' % (prefix,),
                 'type': 'numeric',
                 'options': ('no_write_model',),
                 },
            ])

        @classmethod
        def _addModuleKnobs(cls, carrier, prefix, idx):
            _MODULE._addModuleKnobs(carrier, prefix, idx)
            start = carrier._channel_offset[idx]+1
            for i in range(start, start+cls._num_channels):
                path = '%s:CHANNEL%02d' % (prefix, i)
                carrier.parts.extend([
                    {'path': path,
                     'type': 'SIGNAL',
                     'options': ('no_write_model', 'write_once',)
                     },
                    {'path': '%s:RANGE' % (path,),
                     'type': 'AXIS',
                     'valueExpr': 'Range(None, None, 1)',
                     'options': ('no_write_shot'),
                     },
                    {'path': '%s:GAIN' % (path,),
                     'type': 'NUMERIC',
                     'value': 10,
                     'help': '0..12 [dB]',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:OFFSET' % (path,),
                     'type': 'NUMERIC',
                     'value': 0.,
                     'help': '[-1., 1.]',
                     'options': ('no_write_shot',),
                     },
                ])

        def _setting_gain(self, i):
            return float(self.getchannel(i, 'gain').record.data())

        # Action methods
        def check(self, *chs):
            chs = super(_ACQ425, self).check(*chs)
            for ch in chs:
                if self.getchannel(ch).on:
                    try:
                        self._offset(ch)
                    except (SystemExit, KeyboardInterrupt):
                        raise
                    except Exception:
                        raise MDSplus.DevBAD_OFFSET
                    try:
                        self._setGain(ch)
                        if not self._gain(ch) == self.nc.range(ch):
                            raise
                    except (SystemExit, KeyboardInterrupt):
                        raise
                    except Exception:
                        print(ch, self._gain(ch), self.nc.range(ch))
                        self.nc.GAIN[ch] = 10
                        raise MDSplus.DevBAD_GAIN

    class _ACQ425(_ACQ425, _acq425):
        pass

    class _ACQ480(_MODULE, _acq480):
        @_cached_property
        def _clkdiv_fpga(self):
            try:
                return int(self.clkdiv_fpga.record.data())
            except MDSplus.TreeNODATA:
                return self.nc._clkdiv_fpga

        @classmethod
        def _addMasterKnobs(cls, carrier, prefix):
            _MODULE._addMasterKnobs(carrier, prefix)
            carrier.parts.extend([
                {'path': '%s:CLKDIV' % (prefix,),
                 'type': 'numeric',
                 'valueExpr': 'MULTIPLY(node.FIR,node.FPGA)',
                 'options': ('write_once', 'no_write_shot'),
                 },
                {'path': '%s:CLKDIV:FIR' % (prefix,),
                 'type': 'numeric',
                 'options': ('write_once', 'no_write_model'),
                 },
                {'path': '%s:CLKDIV:FPGA' % (prefix,),
                 'type': 'numeric',
                 'options': ('write_once', 'no_write_model'),
                 },
                {'path': '%s:FIR' % (prefix,),
                 'type': 'numeric',
                 'value': 0,
                 'help': 'DECIM=[1,2,2,4,4,4,4,2,4,8,1] for FIR=[0,..,10]',
                 'options': ('no_write_shot',),
                 },
            ])

        @classmethod
        def _addModuleKnobs(cls, carrier, prefix, idx):
            start = carrier._channel_offset[idx]+1
            for i in range(start, start+cls._num_channels):
                path = '%s:CHANNEL%02d' % (prefix, i)
                carrier.parts.extend([
                    {'path': path,
                     'type': 'signal',
                     'options': ('no_write_model', 'write_once',),
                     },
                    {'path': '%s:INVERT' % (path,),
                     'type': 'numeric',
                     'value': False,
                     'help': 'bool',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:RANGE' % (path,),
                     'type': 'axis',
                     'valueExpr': 'Range(None, None, 1)',
                     'options': ('no_write_shot'),
                     },
                    {'path': '%s:GAIN' % (path,),
                     'type': 'numeric',
                     'value': 0,
                     'help': '0..12 [dB]',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:LFNS' % (path,),
                     'type': 'numeric',
                     'value': False,
                     'help': 'bool',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:HPF' % (path,),
                     'type': 'numeric',
                     'value': False,
                     'help': 'bool',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:T50R' % (path,),
                     'type': 'numeric',
                     'value': False,
                     'help': 'bool',
                     'options': ('no_write_shot',),
                     },
                ])

        @MDSplus.mdsrecord(filter=int, default=0)
        def _setting_trig_mode(self): return self.__getattr__('trig_mode')
        @MDSplus.mdsrecord(filter=int)
        def _setting_trig_mode_translen(self):
            return self.__getattr__('trig_mode_translen')

        @MDSplus.mdsrecord(filter=int)
        def _setting_fir(self): return self.__getattr__('fir')

        @property
        def init_args(self):
            return (
                [self._setting_gain(c)
                 for c in range(1, self._num_channels+1)],
                [self._setting_invert(c)
                 for c in range(1, self._num_channels+1)],
                [self._setting_hpf(c)
                 for c in range(1, self._num_channels+1)],
                [self._setting_lfns(c)
                    for c in range(1, self._num_channels+1)],
                [self._setting_t50r(c)
                 for c in range(1, self._num_channels+1)],
                self.tree.shot,
            )

        def init_master(self, pre, soft, clkdiv):
            self.nc._init_master(pre, soft,
                                 self._setting_trig_mode,
                                 self._setting_trig_mode_translen,
                                 self._setting_fir)
            fir = self.nc.ACQ480.FIR.DECIM
            fpga = self.nc.ACQ480.FPGA.DECIM
            self.clkdiv_fir = fir
            self.clkdiv_fpga = fpga
            # check clock
            clk = self._carrier._setting_clock
            if clk*fpga < 10000000:
                print('Bus clock must be at least 10MHz')
                raise MDSplus.DevINV_SETUP
            if clk*fpga*fir > 80000000:
                print('ADC clock cannot exceed 80MHz')
                raise MDSplus.DevINV_SETUP

        # Channel settings
        def _setting_invert(self, ch):
            return int(self.getchannel(ch, 'invert').record.data())

        def _setting_gain(self, ch):
            return int(self.getchannel(ch, 'gain').record.data())

        def _setting_hpf(self, ch):
            return int(self.getchannel(ch, 'hpf').record.data())

        def _setting_lfns(self, ch):
            return int(self.getchannel(ch, 'lfns').record.data())

        def _setting_t50r(self, ch):
            return int(self.getchannel(ch, 't50r').record.data())

    class _AO420(_MODULE, _ao420):
        """MDSplus module for AO420.

        PVs are set from the user tree-knobs.
        """

        @classmethod
        def _addMasterKnobs(cls, carrier, prefix):
            _MODULE._addMasterKnobs(carrier, prefix)
            carrier.parts.extend([
                {'path': '%s:CLKDIV' % (prefix,),
                 'type': 'numeric',
                 'options': ('no_write_model',),
                 },
            ])

        @classmethod
        def _addModuleKnobs(cls, carrier, prefix, idx):
            start = carrier._channel_offset[idx]+1
            for i in range(start, start+cls._num_channels):
                path = '%s:CHANNEL%02d' % (prefix, i)
                carrier.parts.extend([
                    {'path': path,
                     'type': 'numeric',
                     'valueExpr': 'EXT_FUNCTION(None, "numpy", node.EXPR)',
                     'options': ('no_write_model', 'write_once',
                                 'no_write_shot',),
                     },
                    {'path': '%s:EXPR' % (path,),
                     'type': 'text',
                     'value': 'sin(linspace(0, 2*pi, 100000))*5',
                     'options': ('no_write_shot',),
                     },
                    {'path': '%s:RANGE' % (path,),
                     'type': 'text',
                     'options': ('no_write_model', 'write_once'),
                     },
                    {'path': '%s:GAIN' % (path,),
                     'type': 'numeric',
                     'options': ('no_write_model', 'write_once'),
                     },
                    {'path': '%s:OFFSET' % (path,),
                     'type': 'numeric',
                     'options': ('no_write_model', 'write_once'),
                     },
                ])

        def _setting_expr(self, ch):
            return str(self.getchannel(ch, 'expr').record.data())

    # -------------------------
    #  7 Public MDSplus Devices
    # -------------------------

    # ACQ1001 carrier
    @_assemble(_ACQ425)
    class ACQ1001_ACQ425(_ACQ1001):
        """ACQ1001 carrier with a ACQ425."""

    @_assemble(_ACQ480)
    class ACQ1001_ACQ480(_ACQ1001):
        """ACQ1001 carrier with a ACQ480."""

    @_assemble(_AO420)
    class ACQ1001_AO420(_ACQ1001):
        """ACQ1001 carrier with a AO420."""

    # set bank_mask A,AB,ABC (default all, i.e. ABCD)
    #  only for ACQ425 for now
    class _ACQ425_4CH(_ACQ425):
        _num_channels = 4

    @_assemble(_ACQ425_4CH)
    class ACQ1001_ACQ425_4CH(_ACQ1001):
        """ACQ1001 carrier with a ACQ425 and 4 active channels."""

    class _ACQ425_8CH(_ACQ425):
        _num_channels = 8

    @_assemble(_ACQ425_8CH)
    class ACQ1001_ACQ425_8CH(_ACQ1001):
        """ACQ1001 carrier with a ACQ425 and 8 active channels."""

    class _ACQ425_12CH(_ACQ425):
        _num_channels = 12

    @_assemble(_ACQ425_12CH)
    class ACQ1001_ACQ425_12CH(_ACQ1001):
        """ACQ1001 carrier with a ACQ425 and 12 active channels."""

    # /mnt/fpga.d/ACQ1001_TOP_08_ff_64B-4CH.bit.gz
    # TODO: how to deal with es_enable CH 5&6 ?
    class _ACQ480_4CH(_ACQ480):
        _num_channels = 4

    @_assemble(_ACQ480_4CH)
    class ACQ1001_ACQ480_4CH(_ACQ1001):
        """ACQ1001 carrier with a ACQ480 and 4 active channels."""

    # ACQ2106 carrier
    @_assemble(_ACQ425, 1)
    class ACQ2106_ACQ425x1(_ACQ2106):
        """ACQ2106 carrier with one ACQ425 modules."""

    @_assemble(_ACQ425, 2)
    class ACQ2106_ACQ425x2(_ACQ2106):
        """ACQ2106 carrier with two ACQ425 modules."""

    @_assemble(_ACQ425, 3)
    class ACQ2106_ACQ425x3(_ACQ2106):
        """ACQ2106 carrier with three ACQ425 modules."""

    @_assemble(_ACQ425, 4)
    class ACQ2106_ACQ425x4(_ACQ2106):
        """ACQ2106 carrier with four ACQ425 modules."""

    @_assemble(_ACQ425, 5)
    class ACQ2106_ACQ425x5(_ACQ2106):
        """ACQ2106 carrier with five ACQ425 modules."""

    @_assemble(_ACQ425, 6)
    class ACQ2106_ACQ425x6(_ACQ2106):
        """ACQ2106 carrier with six ACQ425 modules."""

    @_assemble(_ACQ480, 1)
    class ACQ2106_ACQ480x1(_ACQ2106):
        """ACQ2106 carrier with one ACQ425 modules."""

    @_assemble(_ACQ480, 2)
    class ACQ2106_ACQ480x2(_ACQ2106):
        """ACQ2106 carrier with two ACQ425 modules."""

    @_assemble(_ACQ480, 3)
    class ACQ2106_ACQ480x3(_ACQ2106):
        """ACQ2106 carrier with three ACQ425 modules."""

    @_assemble(_ACQ480, 4)
    class ACQ2106_ACQ480x4(_ACQ2106):
        """ACQ2106 carrier with four ACQ425 modules."""

        _max_clk = 14000000

    @_assemble(_ACQ480, 5)
    class ACQ2106_ACQ480x5(_ACQ2106):
        """ACQ2106 carrier with five ACQ425 modules."""

        _max_clk = 11000000

    @_assemble(_ACQ480, 6)
    class ACQ2106_ACQ480x6(_ACQ2106):
        """ACQ2106 carrier with six ACQ425 modules."""

        _max_clk = 9000000

    # ---------------
    # 8 test drivers
    # ---------------
    from unittest import TestCase, TestSuite, TextTestRunner
    T = [None]

    def _out(msg, reset=False):
        if reset:
            T[0] = time.time()
            t = T[0]
        else:
            t = T[0]
            T[0] = time.time()
        print('%7.3f: %s' % (T[0]-t, msg))

    class _Tests(TestCase):
        simulate = True
        acq2106_480_fpgadecim = 10
        acq2106_425_host = '192.168.44.255'
        acq2106_480_host = '192.168.44.255'  # 'acq2106_065'
        acq1001_420_host = '192.168.44.255'  # 'acq1001_291'
        acq1001_425_host = '192.168.44.255'
        acq1001_480_host = '192.168.44.255'  # 'acq1001_316'
        shot = 1000
        @classmethod
        def getShotNumber(cls):
            return cls.shot
            import datetime
            d = datetime.datetime.utcnow()
            return d.month*100000+d.day*1000+d.hour*10

        @classmethod
        def setUpClass(cls):
            sys.path.insert(0, os.path.dirname(__file__))
            import acq4xx as a
            a.debug = 7
            import gc
            gc.collect(2)
            cls.shot = cls.getShotNumber()
            if MDSplus.getenv('test_path', None) is None:
                MDSplus.setenv('test_path', '/tmp')
            with MDSplus.Tree('test', -1, 'new') as t:
                A = a.ACQ2106_ACQ480x1.Add(t, 'ACQ480x1')
                A.host = cls.acq2106_480_host
                # A.clock_src = 10000000
                A.clock = 2000000
                A.trigger_pre = 0
                A.trigger_post = 100000
                A.module1_fir = 3
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ2106_ACQ480x4.Add(t, 'ACQ480x4_M2')
                A.stream = STREAM_MODE.MGT
                A.stream_mgt_a = 2
                A.stream_mgt_b = 3
                A.stream_mgt_a_sites = MDSplus.Int32([1, 2])
                A.stream_mgt_b_sites = MDSplus.Int32([3, 4])
                A.host = cls.acq2106_480_host
                # A.clock_src = 10000000
                A.clock = 2000000
                A.trigger_pre = 0
                A.trigger_post = 500000
                A.module1_fir = 3
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ2106_ACQ425x2.Add(t, 'ACQ425x2')
                A.host = cls.acq2106_425_host
                # A.clock_src = 10000000
                A.clock = 100000
                A.stream = STREAM_MODE.ETH
                A.trigger_pre = 0
                A.trigger_post = 100000
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ2106_ACQ425x2.Add(t, 'ACQ425x2_M2')
                A.stream = STREAM_MODE.MGT
                A.stream_mgt_a_sites = MDSplus.Int32([1])
                A.stream_mgt_b_sites = MDSplus.Int32([2])
                A.host = cls.acq2106_425_host
                # A.clock_src = 10000000
                A.clock = 1000000
                A.trigger_pre = 0
                A.trigger_post = 500000
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ1001_ACQ425.Add(t, 'ACQ425')
                A.host = cls.acq1001_425_host
                # A.clock_src = 10000000
                A.clock = 1000000
                A.trigger_pre = 0
                A.trigger_post = 100000
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ1001_ACQ480.Add(t, 'ACQ480')
                A.host = cls.acq1001_480_host
                # A.clock_src = 10000000
                A.clock = 2000000
                A.trigger_pre = 0
                A.trigger_post = 100000
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                A = a.ACQ1001_AO420.Add(t, 'AO420')
                A.host = cls.acq1001_420_host
                # A.clock_src = 10000000
                A.clock = 1000000
                sin = "sin(linspace(0,pi*2,%d)),0" % 100000
                cos = "cos(linspace(0,pi*2,%d)),0" % 100000
                A.getchannel(1, 'expr').record = '1+1*'+sin
                A.getchannel(2, 'expr').record = '4*'+cos
                A.getchannel(3, 'expr').record = '-7*'+sin
                A.getchannel(4, 'expr').record = '-11*'+cos
                A.ACTIONSERVER.SOFT_TRIGGER.on = True
                t.write()
            t.cleanDatafile()

        @classmethod
        def tearDownClass(cls): pass

        @staticmethod
        def makeshot(t, shot, dev):
            _out('creating "%s" shot %d' % (t.tree, shot))
            MDSplus.Tree('test', -1, 'readonly').createPulse(shot)
            t = MDSplus.Tree('test', shot)
            A = t.getNode(dev)
            A.simulate = _Tests.simulate
            A.debug = 7
            _out('init A')
            A.init()
            _out('arm A')
            A.arm()
            try:
                _out('wait 2sec ')
                time.sleep(2)
                _out('TRIGGER! ')
                A.soft_trigger()
                t = int(A._setting_post/A._setting_clock+1)*2
                _out('wait %dsec' % t)
                time.sleep(t)
                if dev.startswith('ACQ'):
                    _out('store')
                    A.store()
            finally:
                if dev.startswith('ACQ'):
                    A.deinit()
            _out('done')
            if dev.startswith('ACQ'):
                sig = A.getchannel(2).record
                if A.debug > 0:
                    print(sig)
                if len(sig) != A.trigger_post + A.trigger_pre:
                    if A.debug == 0:
                        print(sig)
                    raise Exception("Unexpected number of samples recorded.")

        def test420Normal(self):
            _out('start test420Normal', 1)
            t = MDSplus.Tree('test')

            self.makeshot(t, self.shot+8, 'AO420')
            t = MDSplus.Tree('test', self.shot+8)
            rang = ('5V', '5V', '10V', '10V+')
            gain = (6553, 26214, 22937, 32767)
            offs = (6553, 0, 0, 0)
            for i in range(4):
                self.assertEqual(
                    str(t.AO420.getchannel(i+1, 'range').data()), rang[i])
                self.assertEqual(
                    int(t.AO420.getchannel(i+1, 'gain').data()), gain[i])
                self.assertEqual(
                    int(t.AO420.getchannel(i+1, 'offset').data()), offs[i])
            print(t.AO420.COMMANDS.record)

        def test425Normal(self):
            _out('start test425Normal', 1)
            t = MDSplus.Tree('test')
            self.makeshot(t, self.shot+5, 'ACQ425')

        def test425X2Normal(self):
            _out('start test425X2Normal', 1)
            t = MDSplus.Tree('test')
            self.makeshot(t, self.shot+6, 'ACQ425X2')

        def test425X2Stream(self):
            _out('start test425X2Stream', 1)
            t = MDSplus.Tree('test')
            self.makeshot(t, self.shot+7, 'ACQ425X2_M2')

        def test480Normal(self):
            _out('start test480Normal', 1)
            t = MDSplus.Tree('test')
            self.makeshot(t, self.shot+5, 'ACQ480')

        def test480X1Normal(self):
            _out('start test480X1Normal', 1)
            t = MDSplus.Tree('test')
            t.ACQ480X1.module1_trig_mode = TRIG_MODE.RUN
            t.ACQ480X1.stream = STREAM_MODE.OFF
            self.makeshot(t, self.shot+1, 'ACQ480X1')

        def test480X1RGM(self):
            _out('start test480X4RGM', 1)
            t = MDSplus.Tree('test')
            t.ACQ480X1.module1_trig_mode = TRIG_MODE.RGM
            self.makeshot(t, self.shot+2, 'ACQ480X1')

        def test480X1RTM(self):
            _out('start test480X1RTM', 1)
            t = MDSplus.Tree('test')
            t.ACQ480X1.module1_trig_mode = TRIG_MODE.RTM
            translen = t.ACQ480X1.trigger_post >> 3
            t.ACQ480X1.module1_trig_mode_translen = translen
            self.makeshot(t, self.shot+3, 'ACQ480X1')

        def test480X1ETH(self):
            _out('start test480X1ETH', 1)
            t = MDSplus.Tree('test')
            t.ACQ480X1.module1_trig_mode = TRIG_MODE.RUN
            t.ACQ480X1.stream = STREAM_MODE.ETH
            t.ACQ480X1.getchannel(1).on = False
            self.makeshot(t, self.shot+1, 'ACQ480X1')

        def runTest(self):
            for test in self.getTests():
                self.__getattribute__(test)()

        @staticmethod
        def get480Tests():
            return ['test480Normal']

        @staticmethod
        def get480x1Tests():
            return ['test480X1Normal', 'test480X1ETH']

        @staticmethod
        def get425Tests():
            return ['test425Normal', 'test425X2Normal', 'test425X2ETH']

        @staticmethod
        def get420Tests():
            return ['test420Normal']

    def _suite(tests):
        return TestSuite(map(_Tests, tests))

    def _runTests(tests):
        TextTestRunner(verbosity=2).run(_suite(tests))

    def _run480():
        _runTests(_Tests.get480Tests())

    def _run480x1():
        _runTests(_Tests.get480x1Tests())

    def _run425():
        _runTests(_Tests.get425Tests())

    def _run420():
        _runTests(_Tests.get420Tests())

    def _runmgtdram(blocks=10, uut='localhost'):
        blocks = int(blocks)
        m = _mgtdram_nc(uut)
        print("INIT  PHASE RUN")
        m._init(blocks, log=sys.stdout)
        print("INIT  PHASE DONE")
        print("STORE PHASE RUN")
        m._store(blocks, chans=32)
        print("STORE PHASE DONE")

    if __name__ == '__main__':
        # test generation
        MDSplus.setenv('acq4xx_path', '/tmp')
        with MDSplus.Tree('acq4xx', -1, 'new') as t:
            import acq4xx
            acq4xx.ACQ1001 = t.addNode('ACQ1001')
            acq4xx.ACQ1001_ACQ425.Add(t, "ACQ1001:ACQ425")
            acq4xx.ACQ1001_ACQ425_12CH.Add(t, "ACQ1001:ACQ425_12CH")
            acq4xx.ACQ1001_ACQ425_8CH.Add(t, "ACQ1001:ACQ425_8CH")
            acq4xx.ACQ1001_ACQ425_4CH.Add(t, "ACQ1001:ACQ425_4CH")
            acq4xx.ACQ1001_ACQ480.Add(t, "ACQ1001:ACQ480")
            acq4xx.ACQ1001_ACQ480_4CH.Add(t, "ACQ1001:ACQ480_4CH")
            acq4xx.ACQ1001_AO420.Add(t, "ACQ1001:AO420")
            acq4xx.ACQ2106 = t.addNode('ACQ2106')
            acq4xx.ACQ2106_ACQ425x1.Add(t, "ACQ2106:ACQ425x1")
            acq4xx.ACQ2106_ACQ425x2.Add(t, "ACQ2106:ACQ425x2")
            acq4xx.ACQ2106_ACQ425x3.Add(t, "ACQ2106:ACQ425x3")
            acq4xx.ACQ2106_ACQ425x4.Add(t, "ACQ2106:ACQ425x4")
            acq4xx.ACQ2106_ACQ425x5.Add(t, "ACQ2106:ACQ425x5")
            acq4xx.ACQ2106_ACQ425x6.Add(t, "ACQ2106:ACQ425x6")
            acq4xx.ACQ2106_ACQ480x1.Add(t, "ACQ2106:ACQ480x1")
            acq4xx.ACQ2106_ACQ480x2.Add(t, "ACQ2106:ACQ480x2")
            acq4xx.ACQ2106_ACQ480x3.Add(t, "ACQ2106:ACQ480x3")
            acq4xx.ACQ2106_ACQ480x4.Add(t, "ACQ2106:ACQ480x4")
            acq4xx.ACQ2106_ACQ480x5.Add(t, "ACQ2106:ACQ480x5")
            acq4xx.ACQ2106_ACQ480x6.Add(t, "ACQ2106:ACQ480x6")
            t.write()

        if run_test:
            _Tests.simulate = True
            t = _Tests()
            t.setUpClass()
            t.test425X2Normal()
            t.test425X2Stream()
            # t.test480X4Stream()
            # t.test480Normal()
            t = MDSplus.Tree('test', 1007)
            rec = t.ACQ425X2_M2.getchannel(1).record
            pp.plot(rec.dim_of()[:10000], rec.data()[:10000])
            pp.show()
        else:
            if len(sys.argv) <= 1:
                print('%s test' % sys.argv[0])
                print('%s test420' % sys.argv[0])
                print('%s test425' % sys.argv[0])
                print('%s test480' % sys.argv[0])
                print('%s test480x1' % sys.argv[0])
                print('%s testmgtdram <blocks> [uut]' % sys.argv[0])
                test_stream_es()
            else:
                print(sys.argv[1:])
                if sys.argv[1] == 'test':
                    if len(sys.argv) > 2:
                        _runTests(sys.argv[2:])
                    else:
                        ai, ao = test()
                elif sys.argv[1] == 'test480x1':
                    _run480x1()
                elif sys.argv[1] == 'test480':
                    _run480()
                elif sys.argv[1] == 'test425':
                    _run425()
                elif sys.argv[1] == 'test420':
                    _run420()
                elif sys.argv[1] == 'testmgtdram':
                    _runmgtdram(*sys.argv[2:])
