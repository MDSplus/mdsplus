#!/usr/bin/env python
"""RedPitaya SCPI interface."""

import socket
import sys
import time
import numpy

if sys.version_info < (3,):
    _str = str
    _bytes = bytes
else:
    _str = bytes.decode
    _bytes = str.encode

if sys.version_info < (3, 4):
    def _wraps(wrapped):
        def wrap(wrapper):
            wrapper.__wrapped__ = wrapped
            wrapper.__name__ = wrapped.__name__
            wrapper.__doc__ = wrapped.__doc__
            return wrapper
        return wrap
else:
    from functools import wraps as _wraps


def _const(lst):
    return type(lst.__name__, (), dict(
        (n, _bytes(n))
        for n in lst()
    ))


@_const
def DIR():
    """Directions - dir."""
    return 'IN', 'OUT'


@_const
def GPIO():
    """Digital inputs and outputs - gpio."""
    return (
        'DIO%d_%s' % (i, p)
        for p in 'PN'
        for i in range(8)
    )


@_const
def LED():
    """Digital led outputs - led."""
    return ('LED%d' % i for i in range(8))


class DPIN(GPIO, LED):
    """Digital inputs and outputs - dpin."""


@_const
def AIN():
    """Analog inputs - ain."""
    return ('AIN%d' for i in range(4))


@_const
def AOUT():
    """Analog outputs - aout."""
    return ('AOUT%d' for i in range(4))


class APIN(AIN, AOUT):
    """Analog inputs and outputs - apin."""


@_const
def FUNC():
    """Define function types for GenWaveform - func."""
    return (
        'SINE', 'SQUARE', 'TRIANGLE',
        'SAWU', 'SAWD',
        'PWM', 'ARBITRARY',
        'DC', 'DC_NEG',
    )


@_const
def GENMODE():
    """Define generator modes for GenMode - genmode."""
    return 'BURST', 'CONTINUOUS'


@_const
def TRIGGER():
    """Define trigger sources for GenTriggerSource - trigger."""
    return 'EXT_PE', 'EXT_NE', 'INT', 'GATED'


@_const
def SOURCE():
    """Define trigger sources for AcqSetTriggerSrc - source."""
    return (
        'DISABLED', 'NOW',
        'CH1_PE', 'CH1_NE', 'CH2_PE', 'CH2_NE',
        'EXT_PE', 'EXT_NE', 'AWG_PE', 'AWG_NE',
    )


@_const
def UNITS():
    """Define units for AcqScpiDataUnits - units."""
    return 'RAW', 'VOLTS'


@_const
def FORMAT():
    """Define formats for AcqScpiDataFormat - format."""
    return 'BIN', 'ASCII'


def _request_any(FUN, recv=None, units=None):
    @_wraps(FUN)
    def method(self, *a, **kw):
        timeout = kw.pop('timeout', None)
        cmd = FUN(self, *a, **kw)
        if isinstance(cmd, tuple):
            if cmd[2] is None:
                cmd = cmd[0] + b'?'
                _recv = recv
            else:
                cmd = b'%s %s' % (cmd[0], cmd[1] % cmd[2:])
                _recv = None
        else:
            _recv = recv
        self.flush()
        if _recv:
            if units:
                self.AcqScpiDataUnits(units)
                self.AcqScpiDataFormat(FORMAT.BIN)
            self.send(cmd)
            return getattr(self, recv)(timeout=timeout)
        elif self.debug & 2:
            self.send(cmd + b';*STB?')
            ans = self.recv_str(timeout=timeout)
            if 'ERR!' in ans:
                raise Exception(_str(cmd), ans)
        else:
            self.send(cmd)
    return method


def _request(FUN):
    return _request_any(FUN)


def _request_raw(FUN):
    return _request_any(FUN, 'recv_raw', UNITS.RAW)


def _request_volts(FUN):
    return _request_any(FUN, 'recv_volts', UNITS.VOLTS)


def _request_str(FUN):
    return _request_any(FUN, 'recv_str')


def _request_int(FUN):
    return _request_any(FUN, 'recv_int')


def _request_bool(FUN):
    return _request_any(FUN, 'recv_bool')


def _request_float(FUN):
    return _request_any(FUN, 'recv_float')


class rp_scpi(object):
    """SCPI class used to access Red Pitaya over an IP network.

    Requires:
    ---------
        systemctl start redpitaya_scpi
        systemctl enable redpitaya_scpi
    """

    app_name = 'scopegenpro'
    start_app_url = 'http://%s:80/bazaar?start=' + app_name
    stop_app_url = 'http://%s:80/bazaar?stop=' + app_name
    debug = 0
    _term = b'\r\n'

    _sock = None
    @property
    def sock(self):
        """Connect to RedPitaya and return socket - cached."""
        if self._sock is None:
            ip, port = socket.getaddrinfo(*self._server)[0][-1]
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            sock.connect((ip, port))
            sock.settimeout(self._timeout)
            self._sock = sock
        return self._sock

    def dprint(self, level, msg, *param):
        """Print debug messages based on debug level."""
        if level & self.debug:
            if param:
                msg = msg % param
            sys.stdout.write("%.3f: " % time.time())
            sys.stdout.write(_str(msg))
            sys.stdout.write('\n')
            sys.stdout.flush()

    def __init__(self, host, timeout=10, port=5000):
        """Initialize object and open IP connection."""
        self._server = host, port
        self._timeout = timeout

    def __del__(self):
        """Cleanup."""
        self.close()

    def close(self):
        """Close socket."""
        if self._sock is not None:
            self._sock.close()
            self._sock = None

    def flush(self):
        """Flush socket input buffer."""
        sock = self.sock
        sock.settimeout(0)
        try:
            while sock.recv(0x1000):
                pass
        except IOError:
            pass
        finally:
            sock.settimeout(self._timeout)

    def _recv_filtered(self, bytes_to_read, stop=False):
        sock = self.sock
        term = self._term
        buf = bytearray(bytes_to_read)
        view = memoryview(buf)
        bytes_read = 0
        while view:
            read = sock.recv_into(view, 1)
            if read == 0:
                raise socket.error("Connection closed.")
            if stop:
                if view[0] == term[1]:
                    return bytes(buf[:bytes_read]), False, bytes_read == 0
            stop = view[0] == term[0]
            view = view[1:]
            bytes_read += 1
        return bytes(buf), True, stop

    def _recv_chunks(self, timeout, chunksize=0x1000):
        sock = self.sock
        if timeout is not None:
            sock.settimeout(timeout)
        prev = None
        try:
            chunk, more, stop = self._recv_filtered(chunksize)
        except Exception:
            if timeout is not None:
                sock.settimeout(self._timeout)
            raise
        sock.settimeout(1)
        try:
            if more:
                prev = chunk
                chunk, more, stop = self._recv_filtered(chunksize, stop)
                while more:
                    yield prev
                    prev = chunk
                    chunk, more, stop = self._recv_filtered(chunksize, stop)
                else:
                    if chunk or not stop:
                        yield prev
                        prev = chunk
                    yield prev[:-1]
            else:
                yield chunk[:-1]
        finally:
            sock.settimeout(self._timeout)

    def recv(self, timeout=None):
        """Receive text string and return it after removing the delimiter."""
        msg = b''.join(self._recv_chunks(timeout))
        self.dprint(4, b'< %s', msg)
        return msg

    def recv_str(self, timeout=None):
        """Receive text string and return as str."""
        return _str(self.recv(timeout=timeout))

    def recv_int(self, timeout=None):
        """Receive text string and return as int."""
        return int(self.recv(timeout=timeout))

    def recv_bool(self, timeout=None):
        """Receive text string and return as int."""
        return bool(int(self.recv(timeout=timeout)))

    def recv_float(self, timeout=None):
        """Receive text string and return as float."""
        return float(self.recv(timeout=timeout))

    def recv_bytes(self, timeout=None):
        """Recieve binary data from scpi server."""
        sock = self.sock
        if timeout is not None:
            sock.settimeout(timeout)
        try:
            msg = sock.recv(1)
            if not (msg == b'#'):
                msg += sock.recv(0x1000)
                raise socket.error("Unexpected response '%s'." % _str(msg))
        except Exception:
            if timeout is not None:
                sock.settimeout(self._timeout)
            raise
        sock.settimeout(1)
        numOfNumBytes = int(sock.recv(1))
        if numOfNumBytes == 0:
            return b''
        bytes_to_read = int(sock.recv(numOfNumBytes))
        buf = bytearray(bytes_to_read)
        view = memoryview(buf)
        while bytes_to_read:
            read = sock.recv_into(view, bytes_to_read)
            if not read:
                raise socket.error("Connection closed.")
            view = view[read:]
            bytes_to_read -= read
        return buf

    def recv_raw(self, timeout=None):
        """Recieve binary data as int16 array."""
        return numpy.frombuffer(self.recv_bytes(timeout), 'int16').byteswap()

    def recv_volts(self, timeout=None):
        """Recieve binary data as float32 array."""
        return numpy.frombuffer(self.recv_bytes(timeout), 'float32').byteswap()

    def send(self, msg):
        """Send text string ending and append delimiter."""
        self.dprint(4, b'> %s', msg)
        sock = self.sock
        sock.send(msg)
        sock.send(self._term)

    def __call__(self, msg, timeout=None):
        """Send/receive text string."""
        self.send(msg)
        if b'?' in msg:
            return self.recv(timeout=timeout)
        if timeout is not None:
            self.wait(timeout=timeout)

    # IEEE Mandated Commands

    @_request
    def CLS(self):
        """Clear Status Command."""
        return b'*CLS'

    @property
    @_request_int
    def ESE(self):
        """Read standard Event Status Enable Query."""
        return b'*ESE?'

    @ESE.setter
    @_request
    def ESE(self, value):
        """Write standard Event Status Enable Command."""
        return b'*ESE %d' % (value,)

    @property
    @_request_int
    def ESR(self):
        """Read standard Event Status Register Query."""
        return b'*ESR?'

    @property
    @_request_str
    def IDN(self):
        """Identification Query."""
        return b'*IDN?'

    @_request_bool
    def wait(self):
        """Operation Complete Query.

        *OPC? - *OPC? Operation Complete Query
        The Operation Complete (OPC) query returns the ASCII character 1 in the
        standard Event Status Register when all pending operations have
        finished. This query stops any new commands from being processed until
        the current processing is complete.
        This command blocks the communication until all operations are complete
        (i.e. the timeout setting should be longer than the longest sweep).
        """
        return b'*OPC?'

    @_request
    def OPC(self):
        """Operation Complete Command.

        *OPC - *OPC Operation Complete Command
        The Operation Complete (OPC) command sets bit 0 in the standard
        Event Status Register when all pending operations have finished.
        """
        return b'*OPC'

    @_request
    def RST(self):
        """Reset the device.

        *RST affected - *RST Reset Command
        Resets most signal generator functions to factory- defined conditions.
        Each command shows the [*RST] default value if the setting is affected.
        """
        return b'*RST'

    @property
    @_request_int
    def STB(self):
        """Read STatus Byte."""
        return b'*STB?'

    @property
    @_request_int
    def SRE(self):
        """Read Service Request Enable register.

        *SRE? - *SRE? Service Request Enable Query
        Range 0 - 63 & 128 - 191   (64 - 127 is equiv. to 0 to 63)
        """
        return b'*SRE?'

    @SRE.setter
    @_request
    def SRE(self, data):
        """Write the Service Request Enable register.

        *SRE <data> - *SRE Service Request Enable Command
        Bit 6 (value 64) is ignored and cannot be set by this command.
        Range 0 - 255
        The setting enabled by this command is not affected by signal generator
        preset or *RST.
        However, cycling the signal generator power will reset it.
        """
        return b'*SRE %d' % (data,)

    @_request
    def TRG(self):
        """Trigger device.

        *TRG
        The Trigger (TRG) command triggers the device if LAN is the selected
        trigger source, otherwise, *TRG is ignored.
        """
        return b'*TRG'

    @_request
    def WAI(self):
        """Wait for Commands to complete before continue.

        *WAI - *WAI Wait-to-Continue Command
        The Wait-to-Continue (WAI) command causes the signal generator to
        wait until all pending commands are completed, before executing any
        other commands.
        """
        return b'*WAI'

    @_request
    def DpinSetDirection(self, dir, gpio):
        """Set direction of digital pins to output or input."""
        return b'DIG:PIN:DIR %s,%s' % (dir, gpio)

    @_request_str
    def DpinGetDirection(self, gpio):
        """Get direction of digital pins to output or input."""
        return b'DIG:PIN:DIR? %s' % (gpio,)

    @_request
    def DpinSetState(self, pin, state):
        """Set state of digital outputs to True (HIGH) or False (LOW)."""
        return b'DIG:PIN %s,%d' % (pin, 1 if state else 0)

    @_request_bool
    def DpinGetState(self, pin):
        """Set state of digital outputs to True (HIGH) or False (LOW)."""
        return b'DIG:PIN? %s' % (pin,)

    @_request
    def ApinSetValue(self, pin, value_in_volt):
        """Set analog voltage on slow analog outputs.

        Voltage range of slow analog outputs is: 0 - 1.8 V
        """
        return b'ANALOG:PIN %s,%.3f' % (pin, value_in_volt)

    @_request_float
    def ApinGetValue(self, pin):
        """Read analog voltage from slow analog inputs.

        Voltage range of slow analog inputs is: 0 - 3.3 V
        """
        return b'ANALOG:PIN? %s' % (pin,)

    @_request_bool
    def GenOutEnabled(self, ch, on=None):
        """Check fast analog outputs."""
        if on is not None:
            on = b'ON' if on else b'OFF'
        return b'OUTPUT%d:STATE' % ch, b'%s', on

    def GenOutEnable(self, ch):
        """Enable fast analog outputs."""
        return self.GenOutEnabled(ch, True)

    def GenOutDisable(self, ch):
        """Disable fast analog outputs."""
        return self.GenOutEnabled(ch, False)

    @_request_int
    def GenFreq(self, ch, freq=None):
        """Set fixed sample rate for source."""
        return b'SOUR%d:FREQ:FIX' % (ch,), b'%d', freq

    @_request_str
    def GenWaveform(self, ch, func=None):
        """Set source to given funtion."""
        return b'SOUR%d:FUNC' % (ch,), b'%s', func

    @_request_float
    def GenAmp(self, ch, amplitude=None):
        """Set amplitude voltage of fast analog outputs.

        amplitude : 0 .. 1 [V]
        amplitude + abs(offset) value must be less than maximum output range 1V
        """
        return b'SOUR%d:VOLT' % (ch,), b'%.4f', amplitude

    @_request_float
    def GenOffset(self, ch, offset=None):
        """Set offset voltage of fast analog outputs.

        offset : -1 .. 1 [V]
        amplitude + abs(offset) value must be less than maximum output range 1V
        """
        return b'SOUR%d:VOLT:OFFS' % (ch,), b'%.4f', offset

    @_request_float
    def GenPhase(self, ch, phase=None):
        """Set phase of fast analog outputs.

        phase : -360 .. 360 [deg]
        """
        if phase is not None:
            phase *= numpy.pi/180
        return b'SOUR%d:PHAS' % (ch,), b'%g', phase

    @_request_float
    def GenDutyCycle(self, ch, par=None):
        """Set duty cycle of PWM waveform.

        par : 0 .. 1
        """
        return b'SOUR%d:DCYC' % (ch,), b'%.4g', par

    @_request_volts
    def GenArbWaveform(self, ch, array=None):
        """Load voltage into arbitrary wave generator."""
        if array is not None:
            array = b','.join(b'%.4f' % (v,) for v in array)
        return b'SOUR%d:TRAC:DATA:DATA' % (ch,), b'%s', array

    @_request_str
    def GenMode(self, ch, genmode=None):
        """Set burst mode."""
        return b'SOUR%d:BURS:STAT' % (ch,), b'%s', genmode

    @_request_int
    def GenBurstCount(self, ch, count=None):
        """Set N number of periods in one burst.

        count : 0 - 50000 where 0 is INF
        """
        return b'SOUR%d:BURS:NCYC' % (ch,), b'%d', count

    @_request_int
    def GenBurstRepetitions(self, ch, count=None):
        """Set R number of repeated bursts.

        count : 1 - 50000
        """
        return b'SOUR%d:BURS:NOR' % (ch,), b'%d', count

    @_request_int
    def GenBurstPeriod(self, ch, time=None):
        """Set P total time of one burst in micro seconds.

        time : 1 - 500 000 000 [us]
        This includes the signal and delay.
        """
        return b'SOUR%d:BURS:INT:PER' % (ch,), b'%d', time

    @_request_str
    def GenTriggerSource(self, ch, trigger=None):
        """Set trigger source for selected signal."""
        return b'SOUR%d:TRIG:SOUR' % (ch,), b'%s', trigger

    @_request
    def GenTrigger(self, ch):
        """Trigger selected source immediately only for Burst mode.

        ch == 3 : both
        """
        return b'SOUR%d:TRIG:IMM' % (ch,)

    @_request
    def GenReset(self):
        """Reset generator to default settings."""
        return b'GEN:RST'

    @_request_int
    def AcqGetWritePointer(self):
        """Return current position of write pointer."""
        return b'ACQ:WPOS?'

    @_request
    def AcqStart(self):
        """Start acquisition."""
        return b'ACQ:START'

    @_request
    def AcqStop(self):
        """Stop acquisition."""
        return b'ACQ:STOP'

    @_request
    def AcqReset(self):
        """Stop acquisition and sets all parameters to default values."""
        return b'ACQ:RST'

    @_request
    def AcqSetDecimation(self, decimation):
        """Set decimation factor."""
        for d in 1, 8, 64, 1024, 8192, 65536:
            if decimation <= d:
                break
        return b'ACQ:DEC %d' % (d,)

    @_request_int
    def AcqGetDecimation(self):
        """Get decimation factor."""
        return b'ACQ:DEC?'

    @_request
    def AcqSetAveraging(self, on=True):
        """Enable/disable averaging."""
        return b'ACQ:AVG %s' % (b'ON' if on else b'OFF',)

    @_request_str
    def AcqGetAveraging(self):
        """Get averaging status."""
        return b'ACQ:AVG?'

    @_request
    def AcqSetTriggerSrc(self, source=SOURCE.DISABLED):
        """Set trigger source.

        Disable triggering, trigger immediately or set trigger source & edge.
        """
        return b'ACQ:TRIG %s' % (source,)

    @_request_str
    def AcqGetTriggerState(self):
        """Get trigger status. If DISABLED -> TD else WAIT."""
        return b'ACQ:TRIG:STAT?'

    @_request
    def AcqSetTriggerDelay(self, samples):
        """Set trigger delay in samples."""
        return b'ACQ:TRIG:DLY %d' % (samples,)

    @_request_int
    def AcqGetTriggerDelay(self):
        """Get trigger delay in samples."""
        return b'ACQ:TRIG:DLY?'

    @_request
    def AcqSetTriggerDelayNs(self, time):
        """Set trigger delay in ns."""
        return b'ACQ:TRIG:DLY:NS %d' % (time,)

    @_request_int
    def AcqGetTriggerDelayNs(self):
        """Get trigger delay in ns."""
        return b'ACQ:TRIG:DLY:NS?'

    @_request
    def AcqSetGain(self, ch, hv=False):
        """Set gain settings to HIGH or LOW.

        high : False for LOW, True for HIGH
        This gain is referring to jumper settings on Red Pitaya fast analog
        inputs.
        """
        return b'ACQ:SOUR%d:GAIN %s' % (ch, b'HV' if hv else b'LV')

    @_request_str
    def AcqGetGain(self, ch):
        """Set gain settings to HIGH or LOW.

        high : False for LOW, True for HIGH
        This gain is referring to jumper settings on Red Pitaya fast analog
        inputs.
        """
        return b'ACQ:SOUR%d:GAIN?' % (ch,)

    @_request
    def AcqSetAC_DC(self, ch, ac=False):
        """Set the AC / DC modes for input. (Only SIGNALlab 250-12)."""
        return b'ACQ:SOUR%d:COUP %s' % (ch, b'AC' if ac else b'DC')

    @_request_str
    def AcqGetAC_DC(self, ch, ac=False):
        """Get the AC / DC modes for input. (Only SIGNALlab 250-12)."""
        return b'ACQ:SOUR%d:COUP?' % (ch,)

    @_request
    def AcqSetTriggerLevel(self, level):
        """Set trigger level in mV."""
        return b'ACQ:TRIG:LEV %d' % (level,)

    @_request_int
    def AcqGetTriggerLevel(self):
        """Get trigger level in mV."""
        return b'ACQ:TRIG:LEV?'

    @_request
    def AcqSetExtTriggerLevel(self, level):
        """Set trigger external level in V. (Only SIGNALlab 250-12)."""
        return b'ACQ:EXT:TRIG:LEV %d' % (level,)

    @_request_float
    def AcqGetExtTriggerLevel(self):
        """Get trigger external level in V. (Only SIGNALlab 250-12)."""
        return b'ACQ:EXT:TRIG:LEV?'

    @_request_int
    def AcqGetWritePointerAtTrig(self):
        """Return position where trigger event appeared."""
        return b'ACQ:TPOS?'

    @_request
    def AcqScpiDataUnits(self, units=UNITS.RAW):
        """Select units in which acquired data will be returned."""
        return b'ACQ:DATA:UNITS %s' % (units)

    @_request
    def AcqScpiDataFormat(self, format=FORMAT.ASCII):
        """Select the format acquired data will be returned."""
        return b'ACQ:DATA:FORMAT %s' % (format)

    def _AcqGetDataPos(self, ch, start, m):
        return b'ACQ:SOUR%d:DATA:STA:N? %d,%d' % (ch, start, m)

    @_request_raw
    def AcqGetDataPosRaw(self, ch, start, m):
        """Read m raw samples from start to stop position.

        start : 0 - 16384
        m : 1 - 116384
        """
        return self._AcqGetDataPos(ch, start, m)

    @_request_volts
    def AcqGetDataPosV(self, ch, start, m):
        """Read m samples in volts from start to stop position.

        start : 0 - 16384
        m : 1 - 116384
        """
        return self._AcqGetDataPos(ch, start, m)

    def _AcqGetData(self, ch):
        return b'ACQ:SOUR%d:DATA?' (ch,)

    @_request_raw
    def AcqGetDataRaw(self, ch, start, m):
        """Read full buf."""
        return self._AcqGetData(ch)

    @_request_volts
    def AcqGetDataV(self, ch, start, m):
        """Read full buf."""
        return self._AcqGetDataPos(ch,)

    def _AcqGetOldestData(self, ch, m):
        return b'ACQ:SOUR%d:DATA:OLD:N? %d' % (ch, m)

    @_request_raw
    def AcqGetOldestDataRaw(self, ch, m):
        """Read m samples after trigger delay, starting from oldest sample.

        m : 1 - 116384
        The oldes sample is the first sample after trigger delay.
        Trigger delay by default is set to zero (in samples or in seconds).
        If trigger delay is set to zero it will read m samples starting from
        trigger.
        """
        return self._AcqGetOldestData(ch, m)

    @_request_volts
    def AcqGetOldestDataV(self, ch, m):
        """Read m samples after trigger delay, starting from oldest sample.

        m : 1 - 116384
        The oldes sample is the first sample after trigger delay.
        Trigger delay by default is set to zero (in samples or in seconds).
        If trigger delay is set to zero it will read m samples starting from
        trigger.
        """
        return self._AcqGetOldestData(ch, m)

    def _AcqGetLatestData(self, ch, m):
        return b'ACQ:SOUR%d:DATA:LAT:N? %d' % (ch, m)

    @_request_raw
    def AcqGetLatestDataRaw(self, ch, m):
        """Read m samples before trigger delay.

        m : 1 - 116384
        Trigger delay by default is set to zero (in samples or in seconds).
        If trigger delay is set to zero it will read m samples before trigger.
        """
        return self._AcqGetLatestData(ch, m)

    @_request_volts
    def AcqGetLatestDataV(self, ch, m):
        """Read m samples before trigger delay.

        m : 1 - 116384
        Trigger delay by default is set to zero (in samples or in seconds).
        If trigger delay is set to zero it will read m samples before trigger.
        """
        return self._AcqGetLatestData(ch, m)

    @_request_int
    def AcqGetBufSize(self):
        """Request number of samples in buffer."""
        return b'ACQ:BUF:SIZE?'


def main(args=None):
    """Run some tests."""
    if args is None:
        args = sys.argv[1:]
    ip = args[0] if args else 'RP-F0432C'
    rp = rp_scpi(ip)
    rp.debug = 0
    B = rp.AcqGetBufSize()
    print(rp.IDN, B)
    rp.RST()
    rp.ESE = rp.SRE = 0xff
    rp.GenReset()
    rp.wait()
    rp.DpinSetState(DPIN.LED0, True)
    rp.GenArbWaveform(1, (0, .5, 1))
    rp.GenArbWaveform(1)
    rp.GenFreq(1, 1000)
    rp.GenMode(1, GENMODE.CONTINUOUS)
    rp.GenOffset(1, 0)
    rp.GenAmp(1, 1)
    rp.GenPhase(1, 30)
    rp.GenDutyCycle(1, .5)
    rp.GenBurstCount(1, 0)
    rp.GenBurstRepetitions(1, 1)
    rp.GenWaveform(1, FUNC.SINE)
    rp.GenTriggerSource(1, TRIGGER.INT)
    rp.GenOutEnable(1)
    rp.wait()
    rp.DpinSetState(DPIN.LED1, True)
    rp.AcqReset()
    rp.AcqSetDecimation(0x20)
    rp.AcqSetTriggerLevel(0)
    rp.AcqSetTriggerDelay(B >> 1)
    rp.AcqSetGain(1, hv=False)
    rp.AcqSetAveraging(False)
    rp.AcqStart()
    time.sleep(1)
    rp.DpinSetState(DPIN.LED2, True)
    rp.AcqSetTriggerSrc(SOURCE.CH1_PE)
    while rp.AcqGetTriggerState() == 'TD':
        pos = rp.AcqGetWritePointerAtTrig()+1
        if pos != 1:
            break
    else:
        pos = rp.AcqGetWritePointerAtTrig()+1
    rp.AcqSetTriggerSrc(SOURCE.DISABLED)
    while rp.AcqGetTriggerState() == 'WAIT':
        end = rp.AcqGetWritePointer()
        if end == pos-1:
            break
    else:
        end = rp.AcqGetWritePointer()
    if end < pos:
        start = end
        end = B
    else:
        start = 0
    print(start, pos, end)
    d = rp.AcqGetDataPosV(1, 0, end)
    from matplotlib.pyplot import plot
    plot(range(start-pos, 0), d[start:pos])
    plot(range(0, end-pos), d[pos:])
    plot(range(end-pos, end-pos+start), d[:start])
    print(rp.STB, rp.ESR, rp.SRE)
    return 0


if __name__ == '__main__':
    sys.exit(main())
