#!/usr/bin/env python
# based on v7proto-134.pdf
import os
import sys
import time
import base64
import json
import socket
import select
import threading
import traceback

import numpy

if sys.version_info < (3, ):
    import Queue as queue

    def b(s): return s

    def s(b): return b
    string = str, unicode  # analysis:ignore
else:
    import queue

    def b(s): return s if isinstance(s, bytes) else s.encode()

    def s(b): return b if isinstance(b, str) else b.decode("UTF-8")
    string = str, bytes

debug = 0
_dlock = threading.Lock()


def dprint(msg, *var):
    with _dlock:
        if var:
            print(msg % var)
        else:
            print(msg)


class cached_property(property):
    def __init__(self, func):
        self.func = func
        self.__doc__ = func.__doc__

    def __get__(self, instance, cls=None):
        if instance is None:
            return self
        if self.func.__name__ in instance.__dict__:
            return instance.__dict__[self.func.__name__]
        val = instance.__dict__[self.func.__name__] = self.func(instance)
        return val

    def __delete__(self, inst):
        if self.func.__name__ in inst.__dict__:
            del(inst.__dict__[self.func.__name__])


class IP(object):
    def __init__(self, i3, i2=None, i1=None, i0=None):
        if i2 is None:
            if isinstance(i3, int):
                self.ip = i3
                return
            elif isinstance(i3, string):
                if not isinstance(i3, bytes):
                    i3 = i3.encode("UTF-8")
                try:
                    i3 = tuple(int(i) for i in i3.split(b'.', 3))
                except ValueError:
                    i3 = socket.getaddrinfo(i3, 0, socket.AF_INET)[0][4][0]
                    i3 = tuple(int(i) for i in i3.split(b'.', 3))
            i3, i2, i1, i0 = i3
        self.ip = (i3 << 24) + (i2 << 16) + (i1 << 8) + i0

    def __str__(self):
        return ('%d.%d.%d.%d' %
                ((self.ip >> 24) & 0xff,
                 (self.ip >> 16) & 0xff,
                 (self.ip >> 8) & 0xff,
                 (self.ip) & 0xff,
                 ))

    def __int__(self):
        return self.ip


class PhantomExc(Exception):
    pass


class PhantomExcConnect(PhantomExc):
    pass


class PhantomExcTimeout(PhantomExc):
    pass


class PhantomExcNotTriggered(PhantomExc):
    pass


class phantom(object):
    """
    Core n-etwork c-onnection to the phantom appliance.
    This class provides the methods for sending and receiving information/data
    through a network socket. All communication with the D-TACQ devices will
    require this communication layer
    """

    i1e9 = int(1e9)
    i1e6 = int(1e6)
    i1e3 = int(1e3)
    _host = None
    flags_list = (
        ('PRE', 'P'),
        ('ABL', 'O'),
        ('RDY', 'R'),
        ('STR', 'S'),
        ('WRT', 'W'),
        ('TRG', 'T'),
        ('DEF', 'D'),
        ('ACT', 'A'),
    )

    @classmethod
    def ph2flags(cls, ph):
        return ''.join(
            f if flag in ph else f.lower()
            for flag, f in cls.flags_list
        )

    @staticmethod
    def find_cameras(bc='<broadcast>', src="", timeout=3):
        """phantom.find_cameras("192.168.44.255")"""
        def gen(server):
            while True:
                try:
                    type, addr = server.recvfrom(256)
                except Exception:
                    break
                else:
                    yield addr[0], s(type)
        server = socket.socket(
            socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        server.settimeout(timeout)
        server.bind((src, 0))
        server.sendto(b"phantom?", (bc, 7380))
        return dict(gen(server))

    @classmethod
    def trigtime2ns(cls, trigtime):
        if 'trigtime' in trigtime:
            trigtime = trigtime.trigtime
        us = trigtime.secs * cls.i1e6
        us += trigtime.frac
        return us * cls.i1e3

    class Unit(dict):
        " use keys like attributes - add a '_' prefix in case of conflicts "
        @staticmethod
        def conv(value):
            if isinstance(value, (list, tuple)):
                if not isinstance(value, phantom.Res):
                    value = ' '.join(map(str, value))
            elif isinstance(value, dict):
                if not isinstance(value, phantom.Unit):
                    value = phantom.Unit(**value)
            elif isinstance(value, string):
                value = '"%s"' % s(value)
            return value

        def __init__(self, gen=[], **kw):
            self.update(kw)
            for k, v in gen:
                if debug & 4:
                    dprint("unit: %s : %r", k, v)
                if k in self:
                    if isinstance(self[k], list):
                        self[k].append(v)
                    else:
                        self[k] = [self[k], v]
                else:
                    self[k] = v

        def __str__(self):
            return "{%s}" % (", ".join(
                "%s:%s" % (k, self.conv(v))
                for k, v in self.items()))

        def __bytes__(self): return b(str(self))

        def __getattr__(self, name): return self[name.lstrip('_')]

        def __dir__(self): return self.keys()

    class Res(tuple):
        def __str__(self): return '%dx%d' % self

    class Parser:
        buflen = 256

        def __init__(self, input):
            if isinstance(input, socket.socket):
                self.sock = input
                self.sock.settimeout(5)
                self.buf = bytearray(self.buflen)
                self.peaked = 0
                self.sock.settimeout(.1)
            else:
                self.sock = None
                self.buf = input
                self.peaked = len(input)
            self.view = memoryview(self.buf)
            self.pos = 0
            if self.peak(3) == b'ERR':
                self.input(5).reset()
                raise PhantomExc(s(self.peak(self.buflen)))

        def _peak(self, n=1):
            if n > self.peaked and self.sock:
                if select.select([self.sock], [], [], .1)[0]:
                    self.peaked += self.sock.recv_into(
                        self.view[self.pos+self.peaked:], n-self.peaked)

        def peak(self, n=1):
            self._peak(n)
            return bytes(self.view[self.pos:self.pos+min(n, self.peaked)])

        def get(self, off=0): return s(bytes(self.view[off:self.pos]).strip())

        def input(self, n=1, can_reset=False):
            self._peak(n)
            self.peaked -= n
            if self.peaked < 0:
                raise EOFError
            self.pos += n
            if self.peaked == 0 and can_reset and self.sock is not None:
                self.pos = 0
            return self

        def reset(self):
            if self.pos:
                if self.sock is None:
                    self.view = self.view[self.pos:]
                else:
                    for i in range(self.peaked):
                        self.buf[i] = self.buf[self.pos+i]
            self.pos = 0
            return self

        def is_end(self):
            ans = self.peak(2)
            if len(ans) < 2:
                print(ans)
                return True
            if ans == b"\r\n":
                return True
            return False

        def parse(self):
            key = None
            res = False
            while True:
                if self.is_end():
                    break
                while self.peak() in b' \\\t\r\n,:':
                    self.input(can_reset=True)
                self.reset()
                if key is None:
                    if self.peak() == b'}':
                        self.input()
                        break
                    while self.peak() not in b':}':
                        self.input()
                    if self.peak() == b'}':
                        value = tuple(self.get().split(" "))
                        self.input()
                        yield 0, value
                        break
                    key = self.get()
                    if debug & 2:
                        dprint("parser: got key: %s", key)
                    self.input()
                else:
                    if self.peak() == b'{':
                        self.input()
                        value = phantom.Unit(self.parse())
                        value = value.get(0, value)
                    elif self.peak() == b'"':
                        self.input()
                        while self.peak() != b'"':
                            self.input()
                        value = self.get(1)
                        self.input()  # skips "
                    else:
                        value = []
                        while True:
                            while self.peak() not in b' },\r\n':
                                self.input()
                            yytext = self.get()
                            if '.' in yytext:
                                value.append(float(yytext))
                            elif yytext[0] == '0':
                                value.append(int(yytext, 16))
                            else:
                                value.append(int(yytext))
                            if self.peak() == b'}':
                                self.input()  # skips }
                                break
                            elif self.peak(3) == b' x ':
                                self.input(3).reset()
                                res = True
                            else:
                                break
                        if len(value) == 1:
                            value = value[0]
                        elif res:
                            value = phantom.Res(value)
                        else:
                            value = tuple(value)
                    if debug & 2:
                        dprint("parser: got val: %r", value)
                    yield (key, value)
                    key = None
                    res = False

        def get_unit(self): return phantom.Unit(self.parse())

    def __init__(self, host): self.__dict__['_host'] = host

    def __str__(self): return "%s(%s)" % (self.__class__.__name__, self._host)

    def __repr__(self): return str(self)

    def __getattr__(self, name): return self.get(name.replace("_", "."))

    def __setattr__(self, name, value): self.set(name.replace("_", "."), value)

    def new_socket(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        try:
            sock.connect((self._host, 7115))
        except socket.timeout:
            raise PhantomExcConnect
        return sock

    def __call__(self, cmd):
        def recvstr(sock):
            sock.settimeout(5)
            ans = sock.recv(1024)
            yield ans
            sock.settimeout(.1)
            try:
                while ans:
                    ans = sock.recv(1024)
                    yield ans
            except socket.timeout:
                pass
            finally:
                sock.settimeout(5)
        if select.select([self.sock], [], [], 0)[0]:
            raise PhantomExc("Error: RecvBuffer not empty")
        sock = self.sock
        if debug & 8:
            dprint('cmd: %s' % s(cmd))
        sock.sendall(b(cmd))  # sends the command
        sock.sendall(b'\n')
        try:
            res = b''.join(recvstr(sock))
        except socket.timeout:
            raise PhantomExcTimeout
        if res.startswith(b'ERR:'):
            raise PhantomExc(s(res[5:]))
        return res

    def get(self, *names):  # 5.1
        def getone(name):
            cmd = b("get %s\n" % name)
            if debug & 8:
                dprint("send: %s", cmd)
            self.sock.send(cmd)
            ans = self.Parser(self.sock).get_unit()
            if len(ans) == 1:
                for value in ans.values():
                    return value
            return ans
        if len(names) == 1:
            if isinstance(names[0], str):
                return getone(names[0])
            names = names[0]
        return tuple((getone(name) for name in names))

    def set(self, name, value):  # 5.2
        self("set %s %s" % (name, self.Unit.conv(value)))

    def record_cine(self, cine=None):  # 5.3
        """Start recording in specified cine or next available"""
        self("rec %d" % cine if cine else "rec")
        if cine is not None:
            self("rtorec %d" % cine)

    def delete_cine(self, cine): self("del %d" % cine)  # 5.4

    def soft_trigger(self): self("trig")  # 5.5

    def get_cstats(self):  # 5.6
        # return self.get('c%d.state' % i for i in range(maxcine))
        return self.Parser(self('cstats')).get_unit()

    def start_data(self, port=0):  # 5.7
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.settimeout(5)
        server.bind(("", port))
        try:
            server.listen(1)
            port = server.getsockname()[1]
            self("startdata %d" % port)
            sock, client = server.accept()
            if debug & 1:
                dprint("%s: data connection from %s:%d", self, *client)
            sock.settimeout(10)
            return sock
        finally:
            server.close()

    def img(self, cine, start, count=1, fmt='8'):  # 5.8
        ans = self('img {cine:%d, start:%d, cnt:%d, fmt:%s}'
                   % (cine, start, count, fmt))
        return self.Parser(ans[5:]).get_unit()

    def time(self, cine, start, count=1):  # 5.10
        """comes with a delay, should be requested async"""
        ans = self('time {cine:%d, start:%d, cnt:%d}'
                   % (cine, start, count))
        return self.Parser(ans[5:]).get_unit()

    def ifconfig(self):  # 5.11
        return s(self('ifconfig')[1:-3].replace(b'\\', b'').strip())

    def route(self):  # 5.12
        return s(self('route')[1:-3].replace(b'\\', b'').strip())

    def alloc(self, mm): self('alloc {%s}' % (','.join(map(str, mm))))  # 5.13

    def white_balance(self, nframes=1): self('wbal %d' % nframes)  # 5.14

    def black_reference(self, nframes=8): self('bref %d' % nframes)  # 5.15
    # def update_white_reference(self): self('wupdate')  # 5.16
    # def update_black_reference(self): self('bupdate')  # 5.17
    # def get_fstat(self): return self.Parser(self('fstat')).get_unit()  # 5.18
    # def erase_flash(self): self('ferase')  # 5.19
    # def save_flash(self, cine, format, first, last): self('fsave')  # 5.20
    # def save_cflash(self, cine, format, first, last): self('cfsave')  # 5.21

    def trigger_real_time_output(self): self('rtotrig')  # 5.22

    def tail(self): return s(self('tail').rstrip())  # 5.23

    def set_acqmode(self, new_bpp=12): self('acqmode %d' % new_bpp)  # 5.24
    # def vplay  # 5.25

    def clean(self): self('clean')  # 5.26

    def wrset(self, addr, data):  # 5.27
        length, data = len(data), base64.encodebytes(b(data))
        data = b"begin-base64%s====" % data.rstrip()
        param = self.Unit(addr=5000, len=length, data={})
        self(b'wrset %s' % b(str(param)).replace(b'{}', data))

    def rdset(self, addr, len):  # 5.28
        ans = self(b'rdset %s' % self.Unit(addr=addr, len=len))
        return base64.decodebytes(b''.join(ans.split(b'\\\r\n')[1:-1]))

    def _save_settings(self): self('isave')  # 5.29

    def _load_settings(self): self('iload')  # 5.30

    def save_user_settings(self, slot): self('usave %d' % slot)  # 5.31

    def load_user_settings(self, slot): self('uload %d' % slot)  # 5.32

    def erase_user_settings(self, slot): self('uerase %d' % slot)  # 5.33

    def console(self, level=5):  # 5.34
        """Level    Description
            -2      Exit debug mode
            -1      Error messages.
            0       Informational messages about major events.
            1..5    Other messages (verbosity increases with level)
        """
        sock = self.new_socket()
        sock.send('console %d\n' % level)
        return sock

    # def set_fstop(self, aperture): self('fstop %f' % aperture)  # 5.35
    # def get_fstop(self): return self('fstop')  # 5.35
    # def set_focus(self, move): self('focus %d' % move)  # 5.36
    # def get_focus(self): return self('focus')  # 5.36
    # def lens_cmd(self, cmd): self('lens "%s"' % cmd)  # 5.37

    @cached_property
    def sock(self): return self.new_socket()

    def get_info(self): return self.get('info')  # 4.2

    def get_cam(self): return self.get('cam')  # 4.4

    def get_auto(self): return self.get('auto')  # 4.5

    def get_defc(self): return self.get('defc')  # 4.6

    def get_cinfo(self, cine=1): return self.get('c%d' % cine)  # 4.7

    def get_cstate(self, cine=1): return self.get('c%d.state' % cine)

    def get_irig(self): return self.get('irig')  # 4.9

    def get_meta(self): return self.get('meta')  # 4.10

    def get_hw(self): return self.get('hw')  # 4.11

    def get_video(self): return self.get('video')  # 4.12

    def get_eth(self): return self.get('eth')  # 4.13

    def get_mag(self): return self.get('mag')  # 4.14

    def get_cf(self): return self.get('cf')  # 4.15

    def get_usets(self): return self.get('usets')  # 4.16

    def get_1394(self): return self.get('1394')

    def cstats(self):
        cs = self.get_cstats()
        del(cs['c0'])
        maxc = len(cs)
        cs = tuple(
            self.ph2flags(cs['c%d' % c])
            for c in range(1, maxc)
        )
        return cs

    def sync_time(self, secs=None):
        self.hw_rtctime_tz = 0
        if secs is None:
            secs = time.time() + 1
            time.sleep(secs % 1)
        ack = self.hw_rtctime_secs = int(secs)
        self.hw_rtctime_secs = ack + 1  # wont accept changes otherwise
        return self.irig_sec

    def partition(self, multicine=True):  # 5.15
        cinemem = self.info_cinemem
        if multicine:
            frames = self.defc_ptframes
            res = self.defc_res
            frmem = res[0] * res[1] * 2
            cmem = frmem * frames / (1 << 20)
            num = max(min(int(cinemem/cmem), self.info_maxcines-1), 1)
            mem = cinemem / num
            self.alloc([mem]*num)
            return num
        else:
            self.alloc([cinemem])
            return 1

    def init(self, num_frames, frame_rate, exposure, width=256, height=128,
             ext_clk=False):
        self.defc = self.Unit(
            res=phantom.Res((width, height)),
            rate=frame_rate,
            exp=exposure,
            ptframes=num_frames,
            shoff=0,  # enable shutter
            edrexp=0,  # disable enhanced dynamic range
            hqenable=0,  # disable HQ acquisition mode
            autoexp=self.Unit(enable=0),  # disable autoexposure
            autotrig=0,  # disable autotrigger
            ramp="",  # disable profiling ramp specification
            bcount=0,  # disable burst mode
            bperiod=exposure,  # obuse as set value for exposure
            profile=self.Unit(atrig=0),  # disable profile autotrigger
        )
        self.cam = self.Unit(
            syncimg=1 if ext_clk else 2,  # 0:free running, 1:fsync, 2:irig
            frdelay=0,  # delay in us
        )
        self.set_acqmode(12)  # set to 12 bpp

    def arm(self, multicine=True, bref_frames=32):
        num_cine = self.partition(multicine)
        self.meta_name = "00"  # reset name counter
        self.auto = self.Unit(
            videoplay=0,
            flashsave=0,
            filesave=0,
            bref=0,
            acqrestart=1 if num_cine > 1 else 0,
            trigger=self.Unit(
                x=-1,
                y=-1,
                w=0,
                h=0,
                speed=0,  # dt in us
                area=0,
                threshold=0,
                mode=0,  # 0:disabled, 1:speed
            )
        )
        self.black_reference(bref_frames)
        self.record_cine(1)
        return num_cine

    def is_ready(self, cine=1): return 'ABL' in self.get_cstate(cine)

    def is_active(self, cine=1): return 'ACT' in self.get_cstate(cine)

    def is_armed(self, cine=1): return 'WTR' in self.get_cstate(cine)

    def is_triggered(self, cine=1): return 'TRG' in self.get_cstate(cine)

    def is_stored(self, cine=1): return 'STR' in self.get_cstate(cine)

    def is_invalid(self, cine=1): return 'INV' in self.get_cstate(cine)

    def wait_armed(self, cine=1, timeout=3):
        to = time.time()+timeout
        while not self.is_armed(cine):
            if time.time() > to:
                raise PhantomExcTimeout("timeout: not armed")
            time.sleep(.1)

    def wait_triggered(self, cine=1, timeout=10):
        to = time.time()+timeout
        while not self.is_triggered(cine):
            if time.time() > to:
                raise PhantomExcNotTriggered("timeout: not triggered")
            time.sleep(.1)

    def wait_stored(self, cine=1, timeout=10):
        to = time.time()+timeout
        while not self.is_stored(cine):
            if time.time() > to:
                raise PhantomExcTimeout("timeout: not stored")
            time.sleep(.1)

    def read_images(self, start=0, count=-1, maxchunk=128, debug=0,
                    cine=1, use8bit=False, trigtime=0, port=0):
        def recv(sock, size):
            buf = bytearray(size)
            view = memoryview(buf)
            while size > 0:
                nbytes = sock.recv_into(view, size)
                if not nbytes:
                    break
                view = view[nbytes:]
                size -= nbytes
            return buf[:-size] if size > 0 else buf
        cinfo = self.get_cinfo(cine)
        firstfr = cinfo.firstfr
        lastfr = cinfo.lastfr + 1
        if count < 0:
            count = lastfr - firstfr
        if debug & 1:
            dprint("downloading %d frames.", count)
        trigtime = self.trigtime2ns(cinfo) - trigtime
        if start < firstfr:
            count -= firstfr - start
            start = firstfr
        end = start + count
        if end > lastfr:
            count -= end - lastfr
            end = lastfr
        if debug & 1:
            dprint("cine images: %d : %d - read %d : %d",
                   firstfr, lastfr, start, end)
        if use8bit:
            fmt, fnb, fnp = '8', 1, '<u1'
        else:
            fmt, fnb, fnp = '16', 2, '<u2'
        res = cinfo.res
        imgsize = res[0] * res[1] * fnb
        maxchunk = min((1 << 22) // imgsize, maxchunk)  # limit chunks to 4MB
        stream = self.start_data(port)
        while count > 0:
            chunk = min(count, maxchunk)
            self.img(cine, end-count, chunk, fmt)
            buf = recv(stream, imgsize*chunk)
            shape = (chunk, res[1], res[0])
            yield trigtime, numpy.frombuffer(buf, fnp).reshape(shape)
            count -= chunk
            if debug & 1:
                dprint("remaining frames: %6d", count)


try:
    import MDSplus
except ImportError:
    MDSplus = None
else:

    @MDSplus.with_mdsrecords  # analysis:ignore
    class PHANTOM(MDSplus.Device):  # analysis:ignore
        class Stream(threading.Thread):
            class Writer(threading.Thread):
                def __init__(self, strm, queue):
                    super(PHANTOM.Stream.Writer, self).__init__(
                        name="%s.Writer" % strm.name)
                    self.strm = strm
                    self.queue = queue
                    self.fsttrig = None
                    self.frames = strm.dev.frames
                    self.frames_max = strm.dev.frames_max
                    self.frame_rate = strm.dev.frame_rate
                    self.trigger = strm.dev.trigger

                def run(self):
                    def time_rel(ns=1e9):
                        return MDSplus.DIVIDE(MDSplus.Int64(ns), self.frame_rate)
                    try:
                        cur_frame = 0
                        rng = MDSplus.Range(None, None, time_rel())
                        while True:
                            try:
                                queued = self.queue.get(True, 1)
                            except queue.Empty:
                                if self.strm.on:
                                    continue
                                break  # measurement done
                            if queued is None:
                                break
                            trigger, frames = queued
                            if self.fsttrig is None:
                                self.fsttrig = trigger
                            trigger = MDSplus.Int64(trigger-self.fsttrig)
                            trg = MDSplus.ADD(self.trigger, trigger)
                            first_frame = cur_frame
                            for ic in range(frames.shape[0]):
                                dim = MDSplus.ADD(
                                    trg, time_rel([cur_frame*1e9]))
                                limit = MDSplus.ADD(
                                    trg, time_rel(cur_frame*1e9))
                                data = frames[ic:ic+1]
                                self.frames.makeSegment(
                                    limit, limit, dim, data)
                                cur_frame += 1
                            last_frame = cur_frame - 1
                            win = MDSplus.Window(first_frame, last_frame, trg)
                            start = MDSplus.ADD(trg, time_rel(first_frame*1e9))
                            end = MDSplus.ADD(trg, time_rel(last_frame*1e9))
                            dim = MDSplus.Dimension(win, rng)
                            frames = frames.reshape(
                                (frames.shape[0], -1)).max(1)
                            self.frames_max.makeSegment(
                                start, end, dim, frames)
                            self.queue.task_done()
                    except Exception as e:
                        self.exception = e
                        traceback.print_exc()

            @staticmethod
            def update(node, value):
                value.validation = node.record.data()
                node.no_write_shot = False
                node.no_write_model = True
                node.record = value
                node.write_once = True

            def __init__(self, dev):
                super(PHANTOM.Stream, self).__init__(name="%s.Stream" % dev)
                self.dev = dev.copy()
                self.lib = phantom(dev.lib._host)
                self.stopped = threading.Event()

            def stop(self): self.stopped.set()

            @property
            def on(self): return not self.stopped.is_set()

            def run(self):
                try:
                    self.dev.tree.open()
                    while not self.lib.is_stored(1):
                        if not self.on:
                            return
                        time.sleep(.1)
                    cinfo = self.lib.get_cinfo()
                    self.dev.store_cinfo(cinfo)
                    rate = float(cinfo['rate'])
                    self.update(self.dev.frame_rate,
                                (MDSplus.Float64(rate).setUnits('Hz')))
                    self.update(self.dev.exposure,
                                (MDSplus.Int32(cinfo['exp']).setUnits('ns')))
                    num_cine = len(self.lib.cstats())
                    Q = queue.Queue(30)
                    writer = self.Writer(self, Q)
                    writer.start()
                    try:
                        for c in range(1, num_cine+1):
                            while not self.lib.is_triggered(c):
                                if not self.on:
                                    return  # end of measurement
                                time.sleep(.1)
                            self.lib.wait_stored(c)
                            for trig, imgs in self.lib.read_images(
                                    0, cine=c, debug=self.dev.debug,
                                    port=self.dev._host_port):
                                while writer.is_alive():
                                    try:
                                        Q.put((trig, imgs), True, 1)
                                    except queue.Full:
                                        continue
                                    else:
                                        break
                                else:
                                    break
                    finally:
                        Q.put(None)  # indicate last frame to end thread
                        writer.join()
                        if hasattr(writer, "exception"):
                            self.exception = writer.exception
                except Exception as e:
                    self.exception = e
                    traceback.print_exc()

        parts = [  # Prepare init action collection structure
            dict(
                path=':ACTIONSERVER',
                type='TEXT',
                options=('no_write_shot', 'write_once'),
            ),
            # Init-phase action: configure Phantom camera
            dict(
                path=':ACTIONSERVER:INIT',
                type='ACTION',
                options=('no_write_shot', 'write_once'),
                valueExpr=('Action('
                           'Dispatch(head.ACTIONSERVER, "INIT", 50),'
                           'Method(None, "init", head))'),
            ),
            dict(
                path=':ACTIONSERVER:ARM',
                type='ACTION',
                options=('no_write_shot', 'write_once'),
                valueExpr=('Action('
                           'Dispatch(head.ACTIONSERVER, "ARM", 50),'
                           'Method(None, "arm", head))'),
            ),
            dict(
                path=':ACTIONSERVER:SOFT_TRIGGER',
                type='ACTION',
                options=('no_write_shot', 'write_once', 'disabled'),
                valueExpr=('Action('
                           'Dispatch(head.ACTIONSERVER, "PULSE", 1),'
                           'Method(None, "soft_trigger", head))'),
            ),
            # Store-phase action: save video to local tree
            dict(
                path=':ACTIONSERVER:STORE',
                type='ACTION',
                options=('no_write_shot', 'write_once'),
                valueExpr=('Action('
                           'Dispatch(head.ACTIONSERVER, "STORE", 50),'
                           'Method(None, "store", head))'),
            ),
            dict(
                path=':ACTIONSERVER:DEINIT',
                type='ACTION',
                options=('no_write_shot', 'write_once'),
                valueExpr=('Action('
                           'Dispatch(head.ACTIONSERVER, "DEINIT", 50),'
                           'Method(None, "deinit", head))'),
            ),
            # Create rest of nodes
            dict(
                path=':FRAMES',
                type='SIGNAL',
                options=('no_write_model', 'write_once', 'compress_segments'),
            ),
            dict(
                path=':FRAMES:MAX',
                type='SIGNAL',
                options=('no_write_model', 'write_once', 'compress_segments'),
            ),
            dict(
                path=':FRAMES:META',
                type='TEXT',
                options=('no_write_model', 'write_once'),
                filter=json.loads,
            ),
            dict(
                path=':HOST',
                type='TEXT',
                options=('no_write_shot', ),
                filter=str,
                default=None,
            ),
            dict(
                path=':HOST:PORT',
                type='NUMERIC',
                options=('no_write_shot', ),
                filter=int,
                default=0,
            ),
            dict(
                path=':TRIGGER',
                type='NUMERIC',
                valueExpr='Int64(0).setUnits("ns")',
                options=('no_write_shot', ),
                filter=int,
            ),
            dict(
                path=':RESOLUTION',
                type='NUMERIC',
                valueExpr='Uint16Array([256, 128])',
                options=('no_write_shot', ),
                help='frame dimension in [W, H]',
                filter=[int],
            ),
            dict(
                path=':NUM_FRAMES',
                type='NUMERIC',
                value=1000,
                options=('no_write_shot', ),
                filter=int,
            ),
            dict(
                path=':FRAME_RATE',
                type='NUMERIC',
                valueExpr='Int32(10000).setUnits("Hz")',
                options=('no_write_shot', ),
                filter=int,
            ),
            dict(
                path=':EXPOSURE',
                type='NUMERIC',
                valueExpr='Int32(100000).setUnits("ns")',
                options=('no_write_shot', ),
                help='Exposure time in ns caps automatically based on frame_rate',
                filter=int,
            ),
        ]

        def store_cinfo(self, cinfo):
            """Store cinfo in tree. as method so it can be redirected."""
            self.frames_meta = json.dumps(cinfo)

        @property
        def lib(self):
            """Cache phantom interface. Clears on deinit."""
            pers = self.persistent
            if pers is None:
                pers = self.persistent = dict(
                    lib=phantom(self._host),
                )
            return pers['lib']

        def init(self):
            """Run the INIT action."""
            if self.debug:
                dprint("started init")
            w, h = self._resolution
            try:
                self.lib.init(
                    self._num_frames, self._frame_rate, self._exposure, w, h)
            except PhantomExcConnect:
                raise MDSplus.DevOFFLINE
            except PhantomExcTimeout:
                raise MDSplus.DevCOMM_ERROR
            except PhantomExc:
                raise MDSplus.DevERROR_DOING_INIT

        def arm(self):
            """Run the ARM action."""
            if self.debug:
                print("started arm")
            pers = self.persistent
            if pers is not None:
                stream = pers.get('stream', None)
                if stream is not None:
                    raise MDSplus.DevUNKNOWN_STATE
            try:
                self.lib.arm()
                pers['stream'] = stream = self.Stream(self)
                stream.start()
            except PhantomExcConnect:
                raise MDSplus.DevOFFLINE
            except PhantomExcTimeout:
                raise MDSplus.DevCOMM_ERROR
            except PhantomExc:
                raise MDSplus.DevINV_SETUP

        def soft_trigger(self):
            """Run the SOFT_TRIGGER action."""
            if self.debug:
                dprint("started soft_trigger")
            try:
                self.lib.soft_trigger()
            except PhantomExcConnect:
                raise MDSplus.DevOFFLINE
            except PhantomExcTimeout:
                raise MDSplus.DevCOMM_ERROR
            except PhantomExc:
                raise MDSplus.DevTRIGGER_FAILED

        def store(self):
            """Run the STORE action."""
            pers = self.persistent
            if pers is None:
                raise MDSplus.DevINV_SETUP
            try:
                stream = pers.get('stream', None)
                if stream is not None:
                    try:
                        stream.stop()
                        while stream.is_alive():
                            stream.join(1)
                        if hasattr(stream, "exception"):
                            raise stream.exception
                    finally:
                        del(pers['stream'])
            except PhantomExcConnect:
                raise MDSplus.DevOFFLINE
            except PhantomExcNotTriggered:
                raise MDSplus.DevNOT_TRIGGERED
            except PhantomExcTimeout:
                raise MDSplus.DevTRIGGERED_NOT_STORED
            except PhantomExc:
                raise MDSplus.DevCOMM_ERROR

        def deinit(self):
            """Run the DEINIT action."""
            pers = self.persistent
            if pers is not None:
                try:
                    try:
                        self.store()
                    finally:
                        lib = pers['lib']
                        del(lib.sock)
                finally:
                    self.persistent = None


def test_single_cine():
    """Test basic functions and single cine capture."""
    # print(phantom.find_cameras("192.168.44.255"))
    p = phantom("192.168.44.255")
    p.init(100, 1000, 100000, 128, 80)
    rate = p.defc_rate
    assert rate == 1000, rate
    p.defc = dict(rate=5000)
    rate = p.defc_rate
    assert rate == 5000, rate
    p.defc_rate = 1000
    # single CINE
    p.arm(False)
    p.wait_armed()
    p.soft_trigger()
    p.wait_triggered()
    for t, img in p.read_images(0, 4, 2):
        pass
    try:
        from matplotlib.pyplot import imshow
    except ImportError:
        print("Test: No matplotlib so no imshow")
    else:
        imshow(img[0][::-1], cmap='gray', vmax=1 << 12)


def test_multi_cine():
    """Test multi cine capture."""
    p = phantom("192.168.44.255")
    p.init(1000, 100000, 100000, 1280, 800)
    num_cine = p.arm()
    trg = p.sync_time() * p.i1e9
    print("%18d, %10d, %2d: %r" % (trg, 0, -1, p.cstats()))
    for i in range(2):
        for c in range(1, num_cine+1):
            p.wait_armed(c)
            p.soft_trigger()
            p.wait_stored(c)
            tp = trg
            trg = p.trigtime2ns(p.get('c%d.trigtime' % c))
            print("%18d, %10d, %2d: %r" %
                  (trg, trg - tp, int(p.get("c%d.meta.name" % c)), p.cstats()))


if MDSplus is not None:
    def testmds(expt='test', shot=1):
        """Test MDSplus device with multi cine, i.e. multiple trigger."""
        import gc
        gc.collect(2)
        MDSplus.setenv('default_tree_path', os.getenv("TMP", "/tmp"))
        # MDSplus.setenv("MDS_PYDEVICE_PATH", os.path.dirname(__file__))
        with MDSplus.Tree(expt, -1, 'NEW') as tree:
            dev = PHANTOM.Add(tree, 'PHANTOM')
            dev.host.record = "192.168.44.255"
            tree.write()
        tree.open()
        try:
            tree.createPulse(shot)
            old = MDSplus.Device.debug
            MDSplus.Device.debug = 7
            tree.shot = shot
            tree.open()
            try:
                dev.init()
                dev.arm()
                dev.lib.wait_armed()
                for i in range(5, 0, -1):
                    dev.soft_trigger()
                    if i > 0:
                        time.sleep(1)
                dev.store()
            finally:
                dev.deinit()
        finally:
            tree.close()
            MDSplus.Device.debug = old
            dprint("testmds done")

if __name__ == "__main__":
    test_single_cine()
    test_multi_cine()
    if MDSplus is not None:
        testmds(expt='test', shot=1)
