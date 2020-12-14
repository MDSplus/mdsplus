#!/usr/bin/env python
"""Device driver for Raptor Cygnet4k."""

__version__ = (2020, 7, 16)

import threading
import ctypes
import time
import sys
import signal
import numpy
import subprocess


def _chr(*x):
    return bytes(bytearray(x))


if sys.version_info < (3,):
    import Queue as queue
    _str = str
    _bytes = bytes
    _ord = ord
else:
    _str = bytes.decode
    _bytes = str.encode
    _ord = int
    import queue

code = br"""
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int pxd_capturedBuffer(int);
extern int pxd_buffersFieldCount(int, int);
extern int pxd_readushort(int, int, int, int, int, int, char*, int, char[]);

#if defined(_MSC_VER)
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#define TRUE !0
#define FALSE !!0
#define MAX_DEV 32

typedef struct{
    void *next;
    int ticks;
} header_t;
#define BUFFER_LINK(n) struct { header_t h; char buffer[n]; }

typedef BUFFER_LINK() buffer_link;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static buffer_link *heads[MAX_DEV];
static buffer_link *tails[MAX_DEV];

#define CASE1(i) case 1<<i: return i
#define CASE2(i) CASE1(i);CASE1(i+1)
#define CASE4(i) CASE2(i);CASE2(i+2)
#define CASE8(i) CASE4(i);CASE4(i+4)
#define CASE16(i) CASE8(i);CASE8(i+8)
#define CASE32(i) CASE8(i);CASE8(i+16)
static inline int get_dev(int um){
    switch ((unsigned)um){
        CASE32(0);
        default:
            fprintf(stderr, "Invalid unit mask 0x%X.\n", um);
            return 0;
    }
}

static inline int check_capture(const int um, int *buf, int *last){
    const int nbuf = pxd_capturedBuffer(um);
    if (nbuf == *buf)
        return FALSE;
    *buf = nbuf;
    const int curr = pxd_buffersFieldCount(um, nbuf);
    if (curr - *last <= 0){
        fprintf(stderr, "Buffer changed but curr=%d < last=%d.\n",
                curr, *last);
        return FALSE;
    }
    *last = curr;
    return TRUE;
}

EXPORT int cygnet4k_fetch(
        const int um,
        const int roi0, const int roi1, const int roi2, const int roi3,
        const int *stop){
    const int dev = get_dev(um);
    buffer_link **head = &heads[dev];
    buffer_link **tail = &tails[dev];
    const int pixels = roi2*roi3;
        const ssize_t size = pixels*2;
    int buf = pxd_capturedBuffer(um);
    int idx = pxd_buffersFieldCount(um, buf);
    while (!*stop){
        if (check_capture(um, &buf, &idx)){
            buffer_link *new = (buffer_link*)malloc(sizeof(BUFFER_LINK(size)));
            new->h.next = NULL;
            new->h.ticks = pxd_buffersSysTicks(um, buf);
            const pixels_out = pxd_readushort(
                um, buf, roi0, roi1, roi0+roi2, roi1+roi3, new->buffer,
                pixels, "Grey");
            if (pixels_out != pixels){
                fprintf(stderr, "Error reading buffer %d: %d != %d.\n",
                        buf, pixels_out, pixels);
                free(new);
                return 1;
            }
            else{
#if defined(DEBUG) && DEBUG & 8
                fprintf(stderr, "frame %d read from %d at time %d.\n",
                        idx, buf, new->h.ticks);
#endif
                pthread_mutex_lock(&lock);
                if (!*tail){
                    *tail = new;
                    pthread_cond_signal(&cond);
                }
                else if (*head)
                    (*head)->h.next = new;
                *head = new;
                pthread_mutex_unlock(&lock);
            }
        }
        else
            usleep(4000);
    }
    return 0;
}

EXPORT int cygnet4k_next(const int um, void** buf, int *ticks){
    const int dev = get_dev(um);
    buffer_link **tail = &tails[dev];
    buffer_link **head = &heads[dev];
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    int ok;
    pthread_mutex_lock(&lock);
    if (!*tail)
        pthread_cond_timedwait(&cond, &lock, &ts);
#if defined(DEBUG) && DEBUG & 16
        buffer_link *t = *tail;
        while (t){
            fprintf(stderr, "0x%lx->", *(unsigned long int*)&t);
            t = t->h.next;
        }
        fprintf(stderr, "NULL.\n");
#endif
    if ((ok = !!*tail)){
        if (*head == *tail)
            *head = NULL;
        *buf = (*tail)->buffer;
        *ticks = (*tail)->h.ticks;
        *tail = (*tail)->h.next;
    }
    pthread_mutex_unlock(&lock);
    return ok;
}

EXPORT void cygnet4k_free(void* ptr)
{
    if (ptr)
        free(ptr-sizeof(header_t));
}
"""


def compile_helper(debug=0):
    """Compile helper lib for faster fetcher."""
    cmd = ['gcc', '-x', 'c', '-fPIC', '--shared', '-lpthread', '-lxclib',
           '-o', '/tmp/libcygnet4k.so', '-']
    if debug:
        cmd[1:1] = ['-g', '-O0', '-DDEBUG=%d' % debug]
    else:
        cmd[1:1] = ['-O3']

    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, bufsize=-1)
    process.stdin.write(code)
    process.stdin.close()
    process.wait()


class CygnetExc(Exception):
    """General Cygnet4K exception."""

    def __init__(self, msg=None):
        super(CygnetExc, self).__init__(msg if msg else self.__doc__)


class CygnetExcConnect(CygnetExc):
    """Could not open camera. Camera may not be connected."""


class CygnetExcValue(CygnetExc):
    """Value invalid or out of range."""


class CygnetExcComm(CygnetExc):
    """Communication Error."""


class CygnetExcExtSerTimeout(CygnetExcComm):
    """Partial command packet received.

    Camera timed out waiting for end of packet.
    Command not processed.
    """


class CygnetExcExtCkSumErr(CygnetExcComm):
    """Check sum check failed.

    Command not processed.
    """


class CygnetExcExtI2CErr(CygnetExcComm):
    """I2C command received but failed internally in the camera."""


class CygnetExcExtUnknownCmd(CygnetExcComm):
    """Data was detected on serial line, command not recognized."""


class CygnetExcExtDoneLow(CygnetExcComm):
    """Command to access the camera EPROM received but processing failed.

    EPROM is busy, i.e. FPGA if trying to boot.
    """


class _register(property):
    @property
    def __doc__(self):
        return '%s = 0x%04x[%d]:\n%s' % (
            self.name, self.addr, self.length, self.doc)

    def __init__(self, addr, length):
        self.addr = addr
        self.length = length


class _register_m(_register):
    ext = False

    def __call__(self, method):
        self.name = method.__name__
        self.doc = method.__doc__
        self.r, self.w = method()
        return self

    def __set__(self, inst, value):
        if self.w is None:
            super(_register_m, self).__set__(inst, value)
        inst._set_value(
            (self.addr | 0x80) if self.ext else self.addr,
            self.length, self.w(value), self.ext)

    def __get__(self, inst, cls):
        if inst is None:
            return self
        return self.r(inst._get_value(self.addr, self.length, self.ext))


class _register_e(_register_m):
    ext = True


class _register_c(_register):
    def __call__(self, method):
        self.name = method.__name__
        self.doc = method.__doc__
        self.r = method()
        return self

    def __get__(self, inst, cls):
        if inst is None:
            return self
        return self.r(inst._get_cvalue(self.addr, self.length))


class cygnet4k(object):
    """Device driver."""

    TRIG_EXT_BOTH = 0b11001000
    TRIG_EXT_FALLING = 0b01001000
    TRIG_INTEGRATE = 0b00001100
    TRIG_INT = 0b00001110
    debug = 0
    _lib = []
    um = 1
    aoi_rect = (0, 0, 2048, 2048)

    @classmethod
    def lib(cls):
        """Return cdll ref of xclib."""
        if not cls._lib:
            cls._lib.append(cygnet4k._xclib())
        return cls._lib[0]

    class _xclib(ctypes.CDLL):
        @staticmethod
        def unitmap(unit):
            return 1 << (unit-1)
        # allow other applications to share use of imaging boards
        # previously opened for use by the first application
        DRIVERPARMS = '-XU 1 -DM {um}'

        def __init__(self):
            if sys.platform.startswith('win'):
                form = "%s.dll"
            else:
                form = "lib%s.so"
            super(cygnet4k._xclib, self).__init__(form % 'xclib')
            self.pxd_mesgErrorCode.restype = ctypes.c_char_p
            try:
                self.helper = ctypes.CDLL('libcygnet4k.so')
            except OSError:
                try:
                    compile_helper(cygnet4k.debug)
                    self.helper = ctypes.CDLL('/tmp/libcygnet4k.so')
                except Exception as e:
                    self.helper = None
                    print("No helper %r" % (e,))

        def print_error_msg(self, status):
            msg = self.pxd_mesgErrorCode(status)
            sys.stderr.write('ERROR: %s\n' % msg)
            self.pxd_mesgFault(0xFF)

        def close(self):
            self._chk(self.pxd_PIXCIclose(), msg="close")

        def connect(self, um, formatfile="", format="DEFAULT"):
            driverparams = self.DRIVERPARMS.format(um=um)
            self._chk(self.pxd_PIXCIopen(
                ctypes.c_char_p(_bytes(driverparams)),
                ctypes.c_char_p(_bytes(format)),
                ctypes.c_char_p(_bytes(formatfile))
            ), CygnetExcConnect, "open")

        def _chk(self, status, exc=CygnetExc, msg=None):
            if status >= 0:
                return status
            self.print_error_msg(status)
            raise exc if msg is None else exc(msg)

        def go_live(self, um):
            self._chk(self.pxd_goLivePair(
                um, ctypes.c_long(1), ctypes.c_long(2)))

        def gone_live(self, um):
            return self.pxd_goneLive(um, 0)

        def go_unlive(self, um):
            self._chk(self.pxd_goUnLive(um))

        @classmethod
        def _chk_ser(cls, ack):
            if isinstance(ack, (bytes,)):
                ack = ord(ack[0])
            if ack == 0x50:
                return
            if ack == 0x51:
                raise CygnetExcExtSerTimeout
            if ack == 0x52:
                raise CygnetExcExtCkSumErr
            if ack == 0x53:
                raise CygnetExcExtI2CErr
            if ack == 0x54:
                raise CygnetExcExtUnknownCmd
            if ack == 0x55:
                raise CygnetExcExtDoneLow
            else:
                raise CygnetExcComm(ack)

        def init_serial(self, um):
            try:
                self._chk(self.pxd_serialConfigure(
                    um, 0, ctypes.c_double(115200.), 8, 0, 1, 0, 0, 0
                ), CygnetExcComm, "init_serial")
            except Exception:
                sys.stderr.write("ERROR: CONFIGURING SERIAL CAMERALINK PORT\n")
                raise CygnetExcComm

        def chk_sum(self, msg):
            check = 0
            for c in msg:
                check ^= _ord(c)
            return check

        def send_msg(self, um, msg):
            msg += b'\x50'
            chk = self.chk_sum(msg)
            msg += _chr(chk)
            if cygnet4k.debug & 2:
                print('serial write: %s' % (
                    ' '.join(['0x%02x' % _ord(c) for c in msg]),))
            self._chk(self.pxd_serialWrite(
                um, 0, ctypes.c_char_p(msg), len(msg)
            ), CygnetExcComm, "send_msg")
            return chk

        def flush_serial(self, um):
            buf = ctypes.create_string_buffer(1)
            while self.pxd_serialRead(um, 0, buf, 1) > 0:
                pass

        def recv_msg(self, um, expected, ack=True, chk=None):
            ackchk = (1 if ack else 0) + (0 if chk is None else 1)
            bytes_to_read = expected + ackchk  # ETX and optional check sum
            buf = ctypes.create_string_buffer(bytes_to_read)
            timeout = time.time()+.5
            out = b''
            wait = 0.01
            while timeout > time.time():
                read = self._chk(self.pxd_serialRead(
                    um, 0, buf, bytes_to_read
                ), CygnetExcComm, "recv_msg")
                if read == 0:
                    time.sleep(wait)
                    wait += 0.01
                    continue
                out += buf.raw[:read]
                bytes_to_read -= read
                if bytes_to_read <= 0:
                    if chk is not None:
                        rchk = _ord(out[-1])
                        if rchk != chk:
                            print("CHK MISMATCH: 0x%02x => 0x%02x" % (
                                chk, rchk))
                    if ack:
                        self._chk_ser(out[expected])
                    break
            else:
                if cygnet4k.debug & 2:
                    print('serial read: %s' % (
                        ' '.join([
                            '0x%02x' % ord(c)
                            for c in out[:expected+ackchk-bytes_to_read]
                        ]),
                    ))
                raise CygnetExcComm(
                    "timeout with %d remaining bytes ack, chk = %s" % (
                     bytes_to_read, (ack, chk)))
            if cygnet4k.debug & 2:
                print('serial read: %s' % (
                    ' '.join([
                        '0x%02x' % _ord(c)
                        for c in out[:expected+ackchk]
                    ]),
                ))
            return out[:expected]

        def get_image_res(self, um):
            return self.pxd_imageXdims(um), self.pxd_imageYdims(um)

        @property
        def sys_tick_units(self):
            ticku = (ctypes.c_uint32*2)()
            self._chk(self.pxd_infoSysTicksUnits(
                ticku
            ), CygnetExcComm, "sys_tick_units")
            return (1E-6 * ticku[0]) / ticku[1]

        def check_capture(self, um, buf=None, last=None):
            nbuf = self.pxd_capturedBuffer(um)
            if nbuf == buf:
                return last, buf, False
            curr = self.pxd_buffersFieldCount(um, nbuf)
            if curr - last <= 0:  # OK if looping
                return curr, nbuf, True
            return last, nbuf, False

        def fetch_frame(self, um, roi):
            tick_slope = self.sys_tick_units
            tick_off = None
            if cygnet4k.debug:
                print("number of boards:   %d" % (self.pxd_infoUnits(),))
                print("buffer memory size: %.1f MB" % (
                    self.pxd_infoMemsize(um)/1048576.,))
                print("frame buffers:      %d" % (self.pxd_imageZdims(um),))
                print("image resolution:   %d x %d" % tuple(roi[2:]))
                print("max. image res.:    %d x %d" % self.get_image_res(um))
                print("colors:             %d" % (self.pxd_imageCdims(um),))
                print("bits per pixel:     %d" % (self.pxd_imageBdims(um),))
                print("msec per tick:      %.1f" % (tick_slope * 1E3,))
            pixelin = roi[2]*roi[3]
            size = pixelin*2
            shape = roi[2:]
            if cygnet4k.debug & 8:
                tstart = time.time()
                to = 0
            if self.helper:
                class BUFFER_P(ctypes.c_void_p):
                    def __del__(s):
                        self.helper.cygnet4k_free(s)
                ticks = ctypes.c_int(0)
                stop = ctypes.c_int(0)
                fetcher = threading.Thread(
                    target=self.helper.cygnet4k_fetch, args=(
                        um, roi[0], roi[1], roi[2], roi[3],
                        ctypes.byref(stop)))
                fetcher.start()
                try:
                    buf = BUFFER_P(0)
                    while True:
                        if not self.helper.cygnet4k_next(
                                um, ctypes.byref(buf), ctypes.byref(ticks)):
                            if fetcher.is_alive():
                                yield None
                                if cygnet4k.debug & 8:
                                    to += 1
                                continue
                            return
                        if tick_off is None:
                            tick_off = ticks.value
                            dim = 0
                        else:
                            dim = (ticks.value - tick_off) * tick_slope
                        buffer = (ctypes.c_char*size).from_address(buf.value)
                        buffer.__buffer_p = buf
                        data = numpy.frombuffer(
                            buffer, numpy.uint16).reshape(shape)
                        if cygnet4k.debug & 8:
                            tstart, _tstart = time.time(), tstart
                            print(
                                "FRAME READ AT TIME %.3f in %f %d" % (
                                    dim, tstart - _tstart, to))
                            to = 0
                        yield dim, data
                        buffer = BUFFER_P(0)
                finally:
                    stop.value = 1
            else:
                buf = self.pxd_capturedBuffer(um)
                curr = self.pxd_buffersFieldCount(um, buf)
                while True:
                    buffer = (ctypes.c_char*size)()
                    curr, buf, new = self.check_capture(um, buf, curr)
                    if not new:
                        time.sleep(0.01)
                        curr, buf, new = self.check_capture(um, buf, curr)
                    if not new:
                        yield None
                        if cygnet4k.debug & 8:
                            to += 1
                        continue
                    ticks = self.pxd_buffersSysTicks(um, buf)
                    if tick_off is None:
                        tick_off = ticks
                    dim = (ticks - tick_off) * tick_slope
                    pixels = self._chk(self.pxd_readushort(
                        um, buf, roi[0], roi[1], roi[0]+roi[2], roi[1]+roi[3],
                        buffer, pixelin, ctypes.c_char_p(b"Grey")
                    ), CygnetExcComm, "readushort")
                    if pixels <= 0:
                        print("ERROR: pixels = %d" % pixels)
                    data = numpy.frombuffer(
                        buffer, numpy.uint16).reshape(shape)
                    if cygnet4k.debug & 8:
                        tstart, _tstart = time.time(), tstart
                        print(
                            "FRAME %5d READ FROM %d AT TIME %.3f in %f %d" % (
                                curr, buf, dim, tstart - _tstart, to))
                        to = 0
                    yield dim, data

    _init_serial_done = False

    def _serial_io(self, msg, bytes_to_read=0):
        self.connect()
        with self._lock:
            if not self._init_serial_done:
                self.lib().init_serial(self.um)
                self._init_serial_done = True
                time.sleep(0.02)
            if bytes_to_read is not None:
                self.lib().flush_serial(self.um)
            chk = self.lib().send_msg(self.um, msg)
            if bytes_to_read is None:
                return  # reset micro
            return self.lib().recv_msg(
                self.um, bytes_to_read, self._serial_ack,
                chk if self._serial_chk else None)

    @property
    def system_state(self):
        """Get system state byte."""
        byte = ord(self._serial_io(b'\x49', 1))
        self._serial_ack = bool(byte & (1 << 4))
        self._serial_chk = bool(byte & (1 << 6))
        return byte

    def get_system_state(self):
        """Get system state as dict."""
        byte = self.system_state
        return {
            'chksum': bool(byte & (1 << 6)),
            'ack': bool(byte & (1 << 4)),
            'FPGAboot': bool(byte & (1 << 2)),
            'FPGArun': bool(byte & (1 << 1)),
            'FPGAcom': bool(byte & (1 << 0)),
        }
    _serial_ack = _serial_chk = False

    @system_state.setter
    def system_state(self, byte):
        old = self._serial_ack, self._serial_chk
        self._serial_ack = bool(byte & (1 << 4))
        self._serial_chk = bool(byte & (1 << 6))
        try:
            self._serial_io(b'\x4F' + _chr(byte))
        except Exception:
            self._serial_ack, self._serial_chk = old

    def set_system_state(self, chksum, ack, FPGAhold, FPGAcom):
        """Set system_state with chksum, ack, FPGAhold, and FPGAcomms.

        chksum   Bit 6 = 1 to enable check sum mode
        ack      Bit 4 = 1 to enable command ack
        FPGAhold Bit 1 = 0 to Hold FPGA in RESET; do not boot
        FPGAcom  Bit 0 = 1 to enable comms to FPGA EPROM
        """
        byte = 0
        if chksum:
            byte |= (1 << 6)
        if ack:
            byte |= (1 << 4)
        if FPGAhold:
            byte |= (1 << 1)
        if FPGAcom:
            byte |= (1 << 0)
        self.system_state = byte

    def reset(self):
        """Will trap Micro causing watchdog and reset of firmware."""
        # The camera will give no response to this command
        self._serial_io('\x55\x99\x66\x11', None)
        while True:
            try:
                self.system_state = 0x11
                if self.system_state == 0x11:
                    break
            except CygnetExc:
                time.sleep(.2)
        while True:
            self.system_state = 0x12
            time.sleep(.2)
            if self.system_state == 0x16:
                break

    def _set_byte(self, addr, byte):
        if self.debug & 4:
            print("set: 0x%02x:= 0x%02x" % (addr, byte))
        self._serial_io(b'\x53\xE0\x02' + _chr(addr, byte))

    def _set_value(self, addr, length, value, use_ext_reg=False):
        if self.debug & 4:
            print(("set: 0x%%02x:= 0x%%0%dx" % (length*2,)) % (addr, value))
        s, d = (0, 8) if use_ext_reg else (length*8-8, -8)
        for i in range(length):
            byte = (value >> s) & 0xFF
            if use_ext_reg:  # write to extended register
                self._set_byte(0xF3, addr+i)
                self._set_byte(0xF4, byte)
            else:
                self._set_byte(addr+i, byte)
            s += d

    def _get_byte(self, addr):
        self._serial_io(b'\x53\xE0\x01' + _chr(addr))
        byte = ord(self._serial_io(b'\x53\xE1\x01', 1))
        if self.debug & 4:
            print("get: 0x%02x = 0x%02x" % (addr, byte))
        return byte

    def _get_value(self, addr, length=1, use_ext_reg=False):
        value = 0
        _addr = addr+length-1 if use_ext_reg else addr
        for i in range(length):
            if use_ext_reg:
                self._set_value(_addr, 1, 0x00, True)
                byte = self._get_byte(0x73)
            else:
                byte = self._get_byte(_addr)
            value = (value << 8) | byte
            _addr += -1 if use_ext_reg else 1
        if self.debug & 4:
            print(("get: 0x%%02x = 0x%%0%dx" % (length*2,)) % (addr, value))
        return value

    def _get_cvalue(self, addr, length):
        addrc = ((addr >> i) & 0xff for i in range(17, -1, -8))
        lenc = length + (addr & 1)
        self.system_state |= 1
        try:
            self._serial_io(b'\x53\xAE\x05\x01' + _chr(*addrc) + b'\x00')
            return self._serial_io(b'\x53\xAF' + _chr(lenc), lenc)[(addr & 1):]
        finally:
            self.system_state &= -2

    def set_trig_mode(self, rising, ext, abort, cont, fixed, snap):
        """Set trigger mode with rising, ext, abort, cont, fixed, and snap."""
        byte = 0
        if rising:
            byte |= (1 << 7)
        if ext:
            byte |= (1 << 6)
        if abort:
            byte |= (1 << 3)
        if cont:
            byte |= (1 << 2)
        if fixed:
            byte |= (1 << 1)
        if snap:
            byte |= (1 << 0)
        self.trig_mode = byte

    def get_trig_mode(self):
        """Get trigger mode as dict."""
        byte = self.trig_mode
        return {
            'rising': bool(byte & (1 << 7)),
            'ext': bool(byte & (1 << 6)),
            'abort': bool(byte & (1 << 3)),
            'cont': bool(byte & (1 << 2)),
            'fixed': bool(byte & (1 << 1)),
            'snap': bool(byte & (1 << 0)),
        }

    @property
    def micro_Version(self):
        """Read micro controller version."""
        return tuple(map(ord, self._serial_io(b'\x56', 2)[0:2]))

    @_register_m(0x00, 1)
    def fpga_ctrl_reg():
        """Bit 0 = 1 if TEC is enabled."""
        return int, int

    @_register_m(0x70, 2)
    def pcb_temp():
        """16-bit value; temp[oC] * 16."""
        def r(x):
            return (x & 0xFFF if x & 0x800 else x & 0xFFF-0x1000) >> 4
        return r, None

    @_register_m(0xD4, 1)
    def trig_mode():
        """Trigger Mode.

        rising Bit 7 = 1 to enable rising edge, = 0 falling edge Ext trigger
            Default = 1
        ext    Bit 6 = 1 to enable external trigger
            Default = 0
        abort  Bit 3 = 1 to abort current exposure, self-clearing bit
            Default = 0
        cont   Bit 2 = 1 to start continuous seq., 0 to stop
            Default = 1
        fixed  Bit 1 = 1 to enable fixed frame rate, 0 for continuous ITR
            Default = 0
        snap   Bit 0 = 1 to take snapshot, self-clearing bit
            default = 0
        """
        return int, int

    @_register_m(0xD5, 2)
    def digital_gain():
        """16-bit value; gain * 512."""
        def r(x):
            return x/512.

        def w(x):
            return int(x*512)
        return r, w

    @_register_m(0xD7, 2)
    def roi_x_size():
        """ROI X size as 12-bit value."""
        def r(x):
            return x & 0xFFF

        def w(x):
            return min(x, 0xFFF)
        return r, w

    @_register_m(0xD9, 2)
    def roi_x_offset():
        """ROI X offset as 12-bit value."""
        def r(x):
            return x & 0xFFF

        def w(x):
            return min(x, 0xFFF)
        return r, w

    @_register_m(0xDB, 1)
    def binning():
        """1x1, 2x2, 4x4 = 0x00, 0x11, 0x22."""
        def r(x):
            return 1 << (x & 7)

        def w(x):
            return 0x11 * (x >> 1)
        return r, w

    @_register_m(0xDD, 4)
    def frame_rate():
        """32-bit value[Hz]; 1 count = 1*60 MHz period = 16.66ns."""
        def r(x):
            return 6E7/x

        def w(x):
            return int(6E7/x)
        return r, w

    @_register_m(0xED, 5)
    def exposure():
        """40-bit value[ms]; 1 count = 1*60 MHz period = 16.66ns."""
        def r(x):
            return x/6e4

        def w(x):
            return int(x*6e4)
        return r, w

    @_register_m(0x7E, 1)
    def fpga_major():
        """Return fpga major version."""
        return int, None

    @_register_m(0x7F, 1)
    def fpga_minor():
        """Return fpga major version."""
        return int, None

    @_register_e(0x01, 2)
    def roi_y_size():
        """ROI Y size as 12-bit value."""
        def r(x):
            return x & 0xFFF

        def w(x):
            return min(x, 0xFFF)
        return r, w

    @_register_e(0x03, 2)
    def roi_y_offset():
        """ROI Y offset as 12-bit value."""
        def r(x):
            return x & 0xFFF

        def w(x):
            return min(x, 0xFFF)
        return r, w

    @_register_e(0x7E, 2)
    def cmos_temp_raw():
        """16-bit value, raw."""
        return int, None

    @_register_c(0x04, 2)
    def serial():
        """Return serial number."""
        def r(x):
            return tuple(ord(i) for i in x)
        return r

    @_register_c(0x06, 3)
    def build_date():
        """Return build date (DD,MM,YY)."""
        def r(x):
            return tuple(ord(i) for i in x)
        return r

    @_register_c(0x09, 5)
    def build_code():
        """Return build code."""
        def r(x):
            return x if isinstance(x, str) else x.decode()
        return r

    @_register_c(0x0E, 4)
    def adc_cal():
        """Return CMOS temperature calibration, (offset, slope)."""
        def r(x):
            return (ord(x[0]) | ord(x[1]) << 8, ord(x[2]) | ord(x[3]) << 8)
        return r
    _adc_cal = None

    @property
    def cmos_temp(self):
        """Read CMOS temperature."""
        raw = self.cmos_temp_raw
        try:
            if self._adc_cal is None:
                self._adc_cal = self.adc_cal
            return (40. / (self._adc_cal[1] - self._adc_cal[0])) * (
                raw - self._adc_cal[0])
        except Exception:
            return raw

    @property
    def roi_rect(self):
        """Return region of interrest."""
        return (
            self.roi_x_offset, self.roi_y_offset,
            self.roi_x_size, self.roi_y_size,
        )

    @roi_rect.setter  # does not work properly
    def roi_rect(self, rect):
        self.roi_x_size, self.roi_y_size = rect[2:]
        self.roi_x_offset, self.roi_y_offset = rect[:2]
        self.abort()

    def __init__(self, dev_id=1):
        if dev_id <= 0:
            raise Exception('Invalid DEVICE_ID, must be a positive integer.')
        self._lock = threading.Lock()
        self.dev_id = dev_id

    _dev_open = 0
    @property
    def dev_open(self):
        """Return True if device is open."""
        return (self._dev_open & self.um) != 0

    @dev_open.setter
    def dev_open(self, val):
        if val:
            cygnet4k._dev_open |= self.um
        else:
            cygnet4k._dev_open &= 0xffffffff-self.um

    def connect(self, config_file=""):
        """Open camera connection with opt. configuration file."""
        if config_file:
            self.close()  # as config file might have changed we re-open
        if not self.dev_open:
            # use config file if defined else ""
            self.lib().connect(self.um, config_file)
            self.dev_open = True
            self._serial_ack = self._serial_chk = False
            self.system_state

    def close(self):
        """Close Camera if currently opened."""
        if self.dev_open:
            try:
                self.lib().close()
            except CygnetExc:
                pass
            self.dev_open = False

    def snapshot(self):
        """Set the snapshot flag."""
        self.trig_mode |= 1 << 0

    def abort(self):
        """Set the abort flog."""
        self.trig_mode |= 1 << 3

    def init(self, exposure=None, frame_rate=None, trig_mode=None,
             config_file="", aoi_rect=None, binning=None):
        """Initialize camera and set controllables."""
        self.connect(config_file)  # use config file if defined else ""
        if aoi_rect is not None:
            self.aoi_rect = aoi_rect
        if exposure is not None:
            self.exposure = exposure
        if frame_rate is not None:
            self.frame_rate = frame_rate
        if trig_mode is not None:
            self.trig_mode = trig_mode
        if binning is not None:
            self.binning = binning
        if cygnet4k.debug:
            print('binning:    %dx%d' % tuple([self.binning]*2))
            print('AOI rect:   %s' % (self.aoi_rect,))
            print('ROI rect:   %s' % (self.aoi_rect,))
            print('exposure:   %g ms' % (self.exposure,))
            print('int. clock: %g Hz' % (self.frame_rate,))
            print('trig_mode:  %s' % bin(self.trig_mode,))

    """ STORE """
    class _Reader(threading.Thread):
        def __init__(self, device, writer, nframes=-1, aoi=None):
            threading.Thread.__init__(self, name="Cygnet4k.Reader")
            self.daemon = True
            self.device = device
            self.writer = writer
            self.nframes = nframes
            self._stopped = threading.Event()
            self.ready = threading.Event()
            self.cframes = 0
            self.aoi = aoi

        @property
        def is_triggered(self):
            return self.cframes > 0

        def stop(self):
            self._stopped.set()

        def run(self):
            if self.device.lib().helper is not None:
                self.writer.start()
            try:
                if self.nframes == 0:
                    return
                bin = self.device.binning
                if self.aoi is None:
                    aoi = [d//bin for d in self.device.aoi_rect]
                else:
                    aoi = self.aoi
                frames = self.device.lib().fetch_frame(
                    self.device.um, aoi)
                for frame in frames:
                    if frame is None:
                        if self._stopped.is_set():
                            return
                        continue
                    self.ready.set()
                    self.writer.put(frame, False)
                    self.cframes += 1
                    if (self.nframes > 0 and self.cframes >= self.nframes):
                        return
            finally:
                if self.device.lib().helper is None:
                    self.writer.start()
                self.writer.put(None)

    class _Writer(threading.Thread):
        """Stream frames to MDSplus tree."""

        def __init__(self, nframes, storemethod, *storeargs, **storekwargs):
            threading.Thread.__init__(self, name="Cygnet4k.Writer")
            self.nframes = nframes
            self.storemethod = storemethod
            self.storeargs = storeargs
            self.storekwargs = storekwargs
            self.queue = queue.Queue(-1 if nframes < 0 else nframes+1)
            self._stopped = threading.Event()

        def stop(self):
            self._stopped.set()

        def run(self):
            try:
                tstart = time.time() if cygnet4k.debug else None
                cframes = 0
                while (self.nframes < 0 or cframes < self.nframes):
                    if self._stopped.is_set():
                        return  # stopped
                    frameset = self.queue.get()
                    if frameset is None:
                        return  # last frame
                    self.storemethod(
                        frameset[0], frameset[1],
                        *self.storeargs, **self.storekwargs)
                    cframes += 1
            finally:
                if tstart is not None:
                    print("STORE FINISHED AFTER %.1f sec" % (
                        time.time()-tstart))

        def put(self, *a, **k):
            self.queue.put(*a, **k)

        def get(self, *a, **k):
            self.queue.get(*a, **k)

    def get_temps(self):
        """Try to read temperatures, return None on failure, per temp."""
        try:
            cmos = self.cmos_temp
        except Exception:
            cmos = None
        try:
            pcb = self.pcb_temp
        except Exception:
            pcb = None
        return cmos, pcb

    _streams = {}
    @property
    def capture_is_active(self):
        """Return True if capture is active."""
        return self.um in self._streams

    def start_capture_stream(
            self, nframes, aoi, storemethod, *store_a, **store_kw):
        """Start capture stream thread and return temperatures."""
        def int_action(*a):
            self.stop_capture_stream(0)
        self.connect()  # connect if not yet connected
        if self.lib().gone_live(self.um):
            self.stop_capture_stream(-1)
        cmos, pcb = self.get_temps()
        self.lib().go_live(self.um)
        writer = self._Writer(nframes, storemethod, *store_a, **store_kw)
        reader = self._Reader(self, writer, nframes, aoi)
        self._streams[self.um] = (reader, writer)
        timeout = time.time()+3
        while not self.lib().gone_live(self.um):
            if timeout > time.time():
                raise CygnetExc("timeout on going live")
            time.sleep(.1)
        reader.start()
        signal.signal(signal.SIGINT, int_action)
        if reader.is_alive():
            reader.ready.wait(1)
        if cygnet4k.debug:
            print("Video capture started.")
        return cmos, pcb

    def stop_capture_stream(self, timeout=1):
        """Stop capture stream thread and return temperatures."""
        if self.capture_is_active:
            reader, writer = self._streams[self.um]
            if timeout > 0 and not reader.nframes < 0:
                timeout += time.time()
                while reader.is_alive():
                    if time.time() > timeout:
                        break
                    reader.join(.1)
        self.connect()
        self.lib().go_unlive(self.um)
        if not self.capture_is_active:
            return (None, None) if timeout < 0 else self.get_temps()
        if timeout < 0:
            writer.stop()
        reader.stop()
        reader.join()
        if cygnet4k.debug:
            print("Video capture stopped.")
        signal.signal(signal.SIGINT, signal.SIG_DFL)
        cmos, pcb = (None, None) if timeout < 0 else self.get_temps()
        writer.join()
        del(self._streams[self.um])
        return cmos, pcb


try:
    import MDSplus
except Exception:
    pass
else:
    @MDSplus.with_mdsrecords
    class CYGNET4K(MDSplus.Device):
        """Cygnet 4K sCMOS Camera."""

        parts = [
            {'path': ':ACTIONSERVER',
             'type': 'text',
             'options': ('no_write_shot', 'write_once'),
             },
            {'path': ':ACTIONSERVER:INIT',
             'type': 'action',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           "Dispatch(head.actionserver,'INIT',20,None),"
                           "Method(None,'init',head))"),
             },
            {'path': ':ACTIONSERVER:START',
             'type': 'action',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           "Dispatch(head.actionserver,'INIT',50,None),"
                           "Method(None,'start',head))"),
             },
            {'path': ':ACTIONSERVER:STOP',
             'type': 'action',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           "Dispatch(head.actionserver,'STORE',50,None),"
                           "Method(None,'stop',head,1))"),
             },
            {'path': ':ACTIONSERVER:DEINIT',
             'type': 'action',
             'options': ('no_write_shot', 'write_once'),
             'valueExpr': ('Action('
                           "Dispatch(head.actionserver,'DEINIT',50,None),"
                           "Method(None,'deinit',head))"),
             },
            {'path': ':COMMENT',
             'type': 'text',
             },
            {'path': ':CONF_FILE',
             'options': ('no_write_shot',),
             'type': 'text',
             'filter': str,
             'default': ""
             },
            {'path': ':DEVICE_ID',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': int,
             'value': 1,
             },
            {'path': ':NUM_FRAMES',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': int,
             'value': 100,
             },
            {'path': ':BINNING',
             'type': 'text',
             'options': ('no_write_shot',),
             'filter': lambda x: int(str(x)[0]),
             'value': '1x1',
             },
            {'path': ':ROI_RECT',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': [int],
             'valueExpr': 'Int32([0, 0, 2048, 2048])',
             },
            {'path': ':EXPOSURE',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': float,
             'valueExpr': "Float32(4).setUnits('ms')",
             },
            {'path': ':FRAME_RATE',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': float,
             'valueExpr': "Float32(27.333).setUnits('Hz')",
             },
            {'path': ':TRIGGER_TIME',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': float,
             'valueExpr': "Float32(0).setUnits('s')",
             },
            {'path': ':TRIG_MODE',
             'type': 'numeric',
             'options': ('no_write_shot',),
             'filter': int,
             'valueExpr': 'node.INT',
             },
            {'path': ':TRIG_MODE:EXT_BOTH',
             'type': 'numeric',
             'options': ('no_write_shot', 'write_once'),
             'value': cygnet4k.TRIG_EXT_BOTH,
             },
            {'path': ':TRIG_MODE:EXT_FALLING',
             'type': 'numeric',
             'options': ('no_write_shot', 'write_once'),
             'value': cygnet4k.TRIG_EXT_FALLING,
             },
            {'path': ':TRIG_MODE:INT',
             'type': 'numeric',
             'options': ('no_write_shot', 'write_once'),
             'value': cygnet4k.TRIG_INT,
             },
            {'path': ':TEMP_CMOS',
             'type': 'numeric',
             'options': ('no_write_model', 'write_once'),
             },
            {'path': ':TEMP_PCB',
             'type': 'numeric',
             'options': ('no_write_model', 'write_once'),
             },
            {'path': ':FRAMES',
             'type': 'signal',
             'options': ('no_write_model', 'write_once'),
             },
            {'path': ':FRAMES:PERCENTILE',
             'type': 'signal',
             'options': ('no_write_model', 'write_once'),
             },
        ]

        _cygnet4k = None
        @property
        def cygnet4k(self):
            """Initialize camera."""
            cygnet4k.debug = self.debug
            if self._cygnet4k is None:
                self._cygnet4k = cygnet4k(self._device_id)
            return self._cygnet4k

        def init(self):
            """Initialize camera."""
            try:
                self.deinit()
                # self.cygnet4k.reset()
                self.cygnet4k.init(
                    self._exposure,
                    self._frame_rate,
                    self._trig_mode,
                    self._conf_file,
                    self._roi_rect,
                    self._binning,
                )
            except CygnetExcConnect:
                raise MDSplus.DevOFFLINE
            except CygnetExcValue:
                raise MDSplus.DevINV_SETUP
            except CygnetExcComm:
                raise MDSplus.DevIO_STUCK

        @staticmethod
        def _stream(dim, data, dev):
            dev.tree.open()
            dev.frames_percentile.makeSegment(
                dim, dim, MDSplus.Float32([dim]),
                MDSplus.Uint16([numpy.percentile(data, 99)]))
            if MDSplus.Device.debug & 1:
                print('storeFrame: %s, %s, %s' % (
                    dev.minpath, dim, data.shape))
            dims = MDSplus.Float32([dim]).setUnits('s')
            data = MDSplus.Uint16([data])
            dev.frames.makeSegment(dim, dim, dims, data)

        def start(self):
            """Start video capture."""
            def validate(node, value):
                node.write_once = node.no_write_shot = False
                try:
                    node.record = node.record.setValidation(value)
                finally:
                    node.write_once = node.no_write_shot = True

            def store_temp(node, new):
                if isinstance(new, (float,)):
                    node.record = MDSplus.Float64([new])
                elif isinstance(new, (int,)):
                    node.record = MDSplus.Uint32([new])
            try:
                validate(
                    self.frame_rate, MDSplus.Float32(self.cygnet4k.frame_rate))
                validate(
                    self.exposure, MDSplus.Float32(self.cygnet4k.exposure))
                validate(
                    self.trig_mode, MDSplus.Uint8(self.cygnet4k.trig_mode))
                validate(
                    self.binning, MDSplus.Int8(self.cygnet4k.binning))
                validate(
                    self.roi_rect, MDSplus.Int16(self.cygnet4k.roi_rect))
                aoi = self._roi_rect
                cmos, pcb = self.cygnet4k.start_capture_stream(
                    self._num_frames, aoi, self._stream, self.copy())
                store_temp(self.temp_cmos, cmos)
                store_temp(self.temp_pcb, pcb)
            except CygnetExcConnect:
                raise MDSplus.DevOFFLINE
            except CygnetExcValue:
                raise MDSplus.DevINV_SETUP
            except CygnetExcComm:
                raise MDSplus.DevIO_STUCK

        def stop(self, timeout=1):
            """Stop video capture."""
            def update(node, new):
                if isinstance(new, (float,)):
                    new = MDSplus.Float32(new)
                elif isinstance(new, (int,)):
                    new = MDSplus.Uint16(new)
                else:
                    return
                rec = node.getRecord(None)
                if rec is None:
                    rec = MDSplus.Array([new, new])
                elif len(rec) != 1:
                    return
                else:
                    rec = MDSplus.Array([rec[0], new])
                node.write_once = node.no_write_shot = False
                try:
                    node.record = rec
                finally:
                    node.write_once = node.no_write_shot = True
            if not self.cygnet4k.capture_is_active:
                return
            if isinstance(timeout, MDSplus.Data):
                timeout = timeout.data().tolist()
            try:
                cmos, pcb = self.cygnet4k.stop_capture_stream(timeout)
                node = self.frames_percentile
                if node.getNumSegments() > 0:
                    triggered = True
                    rec = node.getRecord(None)
                    if rec is not None:
                        node.write_once = node.no_write_shot = False
                        try:
                            node.record = rec
                        finally:
                            node.write_once = node.no_write_shot = True
                else:
                    triggered = False
                if timeout < 0:
                    return
                update(self.temp_cmos, cmos)
                update(self.temp_pcb, pcb)
            except CygnetExcConnect:
                raise MDSplus.DevOFFLINE
            except CygnetExcValue:
                raise MDSplus.DevINV_SETUP
            except CygnetExcComm:
                raise MDSplus.DevIO_STUCK
            if not triggered:
                raise MDSplus.DevNOT_TRIGGERED

        def deinit(self):
            """Abort video capture."""
            self.stop(-1)

    def testmds(cfg="/etc/xcap_cygnet4k.fmt"):
        """Test MDSplus device."""
        import gc
        gc.collect()
        MDSplus.setenv('test_path', '/tmp')
        from devices.cygnet4k import CYGNET4K
        with MDSplus.Tree('test', 1, 'NEW') as t:
            dev = CYGNET4K.Add(t, "CYGNET4K")
            t.write()
            dev.conf_file.no_write_shot = False
            dev.conf_file = cfg
            dev.roi_rect.no_write_shot = False
            dev.roi_rect = MDSplus.Int32((128, 128, 512, 512))
        old = MDSplus.Device.debug
        MDSplus.Device.debug = 0  # max(1, old)
        t.open()
        try:
            dev.init()
            time.sleep(1)
            dev.start()
            time.sleep(1)
            dev.stop()
        finally:
            dev.deinit()
            print(dev.frames_percentile.getRecord(None))
            t.close()
            MDSplus.Device.debug = old


def test(cfg="/etc/xcap_cygnet4k.fmt"):
    """Test driver."""
    c = cygnet4k(1)
    c.debug = 8
    aoi = (128, 128, 512, 512)
    c.init(39, 25, c.TRIG_INT, cfg, aoi, 2)

    def store(dim, data):
        print("STORED: %7.3fs %s; p99 = %d; zeros = %d; max = %d" % (
            dim, data.shape,
            numpy.percentile(data, 99),
            numpy.sum(data == 0),
            numpy.sum(data == 4095)))
    c.start_capture_stream(10, aoi, store)
    try:
        time.sleep(3)
    finally:
        c.stop_capture_stream()


def main(*args):
    """Support pyhton -m devices.cygnet4k test."""
    if args and args[0].startswith('mds'):
        testmds(*args[1:])
    else:
        test(*args[1:])


if __name__ == '__main__':
    main(*sys.argv[1:])
