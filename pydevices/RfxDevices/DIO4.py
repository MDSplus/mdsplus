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

from MDSplus import mdsExceptions, Device, Data, Range, makeArray
from numpy import array
from time import sleep

# Hardware API ported from the supplied DIO4HW*.fun routines.
# Fixed-width ctypes follow TDI BYTE/WORD/LONG (8/16/32 bits).
# The vendor header was not supplied: confirm these signatures on the target.
import ctypes as _ct
import math as _math
from contextlib import contextmanager as _contextmanager
from threading import RLock as _RLock

_U8 = _ct.c_uint8
_U16 = _ct.c_uint16
_U32 = _ct.c_uint32
_I32 = _ct.c_int32
_P8 = _ct.POINTER(_U8)
_P16 = _ct.POINTER(_U16)
_P32 = _ct.POINTER(_U32)


class DIO4Error(RuntimeError):
    pass


class _DIO4Hardware(object):
    """Native DIO4 API. One open/close pair per operation; no TDI globals."""
    _lock = _RLock()
    _lib = None
    _signatures = {
        'InitLibrary': [], 'Open': [_I32, _ct.POINTER(_I32)],
        'Close': [_I32], 'Reset': [_I32], 'ResetIOError': [_I32],
        'Cmd_TimingChannelDisarm': [_I32, _U8],
        'Cmd_TimingChannelArm': [_I32, _U8],
        'Cmd_TimingChannelStart': [_I32, _U8],
        'Cmd_TimingChannelTrigger': [_I32, _U8],
        'Cmd_GenerateEvent': [_I32, _U8],
        'Cmd_FlushEventRecorder': [_I32], 'Cmd_ArmEventRecorder': [_I32],
        'Cmd_StopEventRecorder': [_I32], 'EC_ClearAllEventChannels': [_I32],
        'TH_SetTimingHighway': [_I32, _U8, _U8, _U8],
        'CS_SetClockSource': [_I32, _U8, _U8, _U8],
        # TDI passes a LONG output for CheckClockOK, not a BYTE.
        'CS_CheckClockOK': [_I32, _P32],
        'Tst_ReadRegister': [_I32, _U32, _P32],
        'Tst_WriteRegister': [_I32, _U32, _U32],
        'ER_SetEventRecorder': [_I32, _U8, _U8],
        'ER_GetEventCount': [_I32, _P16],
        'ER_GetEvent': [_I32, _P8, _P32],
        'TC_SetTrigger': [_I32, _U8, _U8, _U8, _U8],
        'TC_SetGate': [_I32, _U8, _U8, _U8, _U8, _U8],
        'TC_SetPhaseSettings': [_I32, _U8, _U8, _U8, _P8],
        'TC_SetPhaseTiming': [_I32, _U8, _P32, _U32, _U32],
        'TC_GetPhase1Count': [_I32, _U8, _P32],
        'TC_GetPhase2Count': [_I32, _U8, _P32],
        'IO_SetIOConnectionOutput': [_I32, _U8, _U8, _U8, _U8, _U8, _U8],
        'IO_SetIOConnectionInput': [_I32, _U8, _U8, _U8],
        'EC_GetEventDecoder': [_I32, _U8, _P8, _P8, _P8],
        'EC_SetEventDecoder': [_I32, _U8, _U8, _U8, _U8],
        'EC_GetEventEncoder': [_I32, _U8, _P8, _P8, _P8, _P8],
        'EC_SetEventEncoder': [_I32, _U8, _U8, _U8, _U8, _U8],
    }

    @classmethod
    def library(cls):
        if cls._lib is None:
            lib = _ct.CDLL('libDIO4.so')
            for name, types in cls._signatures.items():
                fn = getattr(lib, 'DIO4_' + name)
                fn.argtypes = types
                fn.restype = _I32
            # The legacy TDI ignores InitLibrary's return value; retain that
            # convention because its return contract is not documented here.
            lib.DIO4_InitLibrary()
            cls._lib = lib
        return cls._lib

    def call(self, name, *args):
        status = getattr(self.library(), 'DIO4_' + name)(*args)
        if status != 0:
            raise DIO4Error('DIO4_%s failed (board %s, status %s)' %
                            (name, self.board, status))

    @_contextmanager
    def opened(self, board):
        with self._lock:
            self.board = int(board)
            handle = _I32(0)
            self.call('Open', self.board, _ct.byref(handle))
            self.h = handle.value
            try:
                yield self
            except BaseException:
                # Preserve the original error if close fails as well.
                try:
                    self.call('Close', self.h)
                except Exception:
                    pass
                raise
            else:
                self.call('Close', self.h)

    @staticmethod
    def channel(c):
        c = int(c)
        if not 0 <= c < 8:
            raise ValueError('Timing channel must be in 0..7')
        return c

    @staticmethod
    def event(code, allow_zero=False):
        code = int(code)
        if not (0 if allow_zero else 1) <= code <= 255:
            raise ValueError('Event code must be in %s..255' %
                             (0 if allow_zero else 1))
        return code

    @staticmethod
    def ticks(value, correction=0, rounded=True):
        value = float(value)
        if not _math.isfinite(value) or value < 0:
            raise ValueError('Time must be finite and nonnegative')
        result = max(0, int(value / 1e-7 + (0.5 if rounded else 0)) - correction)
        if result > 0xffffffff:
            raise ValueError('Time exceeds the 32-bit DIO4 counter range')
        return result

    @staticmethod
    def clock_cycles(frequency, duty):
        frequency, duty = float(frequency), float(duty)
        if not _math.isfinite(frequency) or frequency <= 0:
            raise ValueError('Frequency must be finite and positive')
        if not _math.isfinite(duty) or not 0 <= duty <= 100:
            raise ValueError('Duty cycle must be in 0..100')
        total = int((1. / frequency) / 1e-7 + 0.5)
        if not 2 <= total <= 0xffffffff:
            raise ValueError('Clock period is outside the DIO4 counter range')
        first = max(0, int(total * duty / 100.) - 1)
        return [first, max(0, total - first - 2)]

    def read_register(self, address):
        value = _U32(0)
        self.call('Tst_ReadRegister', self.h, address, _ct.byref(value))
        return value.value

    def check_clock(self):
        for attempt in range(10):
            ok = _U32(0)
            self.call('CS_CheckClockOK', self.h, _ct.byref(ok))
            if ok.value == 1:
                return
            if attempt < 9:
                sleep(1)
        raise DIO4Error('PLL did not lock (board %s)' % self.board)

    def outputs(self, c, term):
        self.call('IO_SetIOConnectionOutput', self.h, 2*c+1, 0, 3,
                  c+1, int(bool(term)), 0)
        self.call('IO_SetIOConnectionInput', self.h, 2*c+2, 0, 0)

    def trigger_settings(self, c, trigger):
        trigger = int(trigger)
        if trigger not in (0, 1, 2, 3):
            raise ValueError('Trigger mode must be in 0..3')
        self.call('TC_SetTrigger', self.h, c+1,
                  (3, 1, 2, 3)[trigger], 1, 2*c+2)
        self.call('TC_SetGate', self.h, c+1, 0, 0, 0, 0)

    def phases(self, c, mode, levels, cycles, delay=0, duration=0):
        self.call('TC_SetPhaseSettings', self.h, c+1, mode, 0,
                  (_U8 * 4)(*levels))
        self.call('TC_SetPhaseTiming', self.h, c+1,
                  (_U32 * 4)(*cycles), delay, duration)

    def decode_events(self, c, trigger, events):
        if int(trigger) != 0:
            return
        if not events:
            raise ValueError('EVENT trigger needs at least one event')
        for event in events:
            event = self.event(event)
            for slot in range(1, 17):
                code, mask, kind = _U8(), _U8(), _U8()
                self.call('EC_GetEventDecoder', self.h, slot,
                          _ct.byref(code), _ct.byref(mask), _ct.byref(kind))
                if code.value == 0 or (code.value == event and kind.value == 0):
                    self.call('EC_SetEventDecoder', self.h, slot, event,
                              mask.value | (1 << c), 0)
                    break
            else:
                raise DIO4Error('No free event decoder slot')

    def Init(self, source, recorder, synch):
        source, recorder = int(source), self.event(recorder, True)
        if source not in (0, 1, 2) or len(synch) > 16:
            raise ValueError('Invalid clock source or too many synchronization events')
        synch = [self.event(code) for code in synch]
        self.call('Cmd_TimingChannelDisarm', self.h, 255)
        self.call('EC_ClearAllEventChannels', self.h)
        self.call('Tst_WriteRegister', self.h, 0x008,
                  self.read_register(0x008) & ~0x800)
        self.call('TH_SetTimingHighway', self.h, 1, int(source != 1), 0)
        sleep(1)
        self.call('CS_SetClockSource', self.h, 3 if source == 1 else 0, 0, 0)
        self.call('Tst_WriteRegister', self.h, 0x3e0,
                  (self.read_register(0x3e0) & 0xffffc000) | 0xc8)
        sleep(1)
        self.check_clock()
        if recorder:
            self.call('ER_SetEventRecorder', self.h, recorder, 0)
            self.call('Cmd_FlushEventRecorder', self.h)
            self.call('Cmd_ArmEventRecorder', self.h)
        for slot in range(1, 17):
            if slot <= len(synch):
                self.call('EC_SetEventDecoder', self.h, slot, synch[slot-1], 255, 1)
            else:
                self.call('EC_SetEventDecoder', self.h, slot, 0, 0, 0)
        return 1

    def Reset(self):
        self.call('Cmd_TimingChannelDisarm', self.h, 255)
        self.call('ResetIOError', self.h)
        self.call('Reset', self.h)
        sleep(1)
        self.call('TH_SetTimingHighway', self.h, 1, 1, 0)
        sleep(1)
        return 1

    def SetClockChan(self, c, frequency, duty, term):
        c = self.channel(c)
        cycles = self.clock_cycles(frequency, duty)
        self.call('TC_SetTrigger', self.h, c+1, 0, 0, 0)
        self.call('TC_SetGate', self.h, c+1, 0, 0, 0, 0)
        self.phases(c, 0, [1, 0, 1, 0], cycles + [0, 0])
        self.outputs(c, term)
        return 1

    def SetPulseChan(self, c, trigger, cyclic, level1, level2,
                     delay, duration, events, term):
        c = self.channel(c)
        self.trigger_settings(c, trigger)
        self.phases(c, int(bool(cyclic)), [level1, level1, level2, level2],
                    [0, 0, 0, 0], self.ticks(delay, 2), self.ticks(duration, 1))
        self.decode_events(c, trigger, events)
        self.outputs(c, term)
        return 1

    def SetGClockChan(self, c, trigger, frequency, delay, duration, events,
                      duty, cyclic, term):
        c = self.channel(c)
        continuous = float(duration) > 420
        cycles = self.clock_cycles(frequency, duty)
        self.trigger_settings(c, trigger)
        self.phases(c, int(bool(cyclic)) | (2 if continuous else 0),
                    [0, 0, 1, 0], [1, 1] + cycles,
                    self.ticks(delay, rounded=False),
                    0 if continuous else self.ticks(duration, rounded=False))
        self.decode_events(c, trigger, events)
        self.outputs(c, term)
        return 1

    def SetDClockChan(self, c, trigger, frequency1, frequency2, delay,
                      duration, events, cyclic, term):
        c = self.channel(c)
        cycles = self.clock_cycles(frequency1, 50) + self.clock_cycles(frequency2, 50)
        self.trigger_settings(c, trigger)
        self.phases(c, int(bool(cyclic)), [1, 0, 1, 0], cycles,
                    self.ticks(delay, 1), self.ticks(duration, 1))
        self.decode_events(c, trigger, events)
        self.outputs(c, term)
        return 1

    def SetExternalClockChan(self, c, frequency1, frequency2, duty, term):
        c = self.channel(c)
        divider = float(frequency1) / 5000.
        if not _math.isfinite(divider) or divider != int(divider) or not 1 <= divider <= 0x3fff:
            raise ValueError('External frequency must be an integer multiple of 5 kHz; divider 1..16383')
        cycles = self.clock_cycles(frequency2, duty)
        self.call('TC_SetTrigger', self.h, c+1, 0, 0, 0)
        self.call('TC_SetGate', self.h, c+1, 0, 0, 0, 0)
        self.phases(c, 0, [1, 0, 1, 0], cycles + [0, 0])
        self.call('CS_SetClockSource', self.h, 1, 2*c+2, 0)
        self.check_clock()
        self.call('Tst_WriteRegister', self.h, 0x008,
                  self.read_register(0x008) | 0x800)
        self.call('Tst_WriteRegister', self.h, 0x3e0,
                  (self.read_register(0x3e0) & 0xffffc000) | int(divider))
        self.outputs(c, term)
        return 1

    def SetEventGenChan(self, channel, event, term, rising):
        channel, event = int(channel), self.event(event)
        if not 1 <= channel <= 16:
            raise ValueError('Physical I/O channel must be in 1..16')
        self.call('IO_SetIOConnectionInput', self.h, channel, 0, int(bool(term)))
        for slot in range(16, 0, -1):
            code, edge, source, chan = _U8(), _U8(), _U8(), _U8()
            self.call('EC_GetEventEncoder', self.h, slot, _ct.byref(code),
                      _ct.byref(edge), _ct.byref(source), _ct.byref(chan))
            if code.value == 0:
                self.call('EC_SetEventEncoder', self.h, slot, event,
                          0 if rising else 1, 1, channel)
                return 1
        raise DIO4Error('No free event encoder slot')

    def SetEvenGenChan(self, c, events, term, rising):
        # Legacy misspelled file was unused and declared the wrong function.
        c = self.channel(c)
        if len(events) != 2 or len(rising) != 2:
            raise ValueError('Paired encoder needs two event and edge values')
        for i in range(2):
            self.SetEventGenChan(2*c+i+1, events[i], term, rising[i])
        return 1

    def StartChan(self, mask, synch):
        mask = int(mask)
        if not 0 <= mask <= 255 or synch not in ('YES', 'NO'):
            raise ValueError('Invalid channel mask or synchronization mode')
        self.call('Cmd_TimingChannelArm', self.h, mask)
        if synch == 'NO':
            self.call('Cmd_TimingChannelStart', self.h, mask)
        return 1

    def Trigger(self, mask):
        mask = int(mask)
        if not 0 <= mask <= 255:
            raise ValueError('Invalid channel mask')
        self.call('Cmd_TimingChannelTrigger', self.h, mask)
        return 1

    def EventTrigger(self, code):
        self.call('Cmd_GenerateEvent', self.h, self.event(code, True))
        return 1

    def DummyEvent(self):
        return self.EventTrigger(0)

    def GetPhaseCount(self, c):
        c = self.channel(c)
        first, second = _U32(), _U32()
        self.call('TC_GetPhase1Count', self.h, c+1, _ct.byref(first))
        self.call('TC_GetPhase2Count', self.h, c+1, _ct.byref(second))
        return [first.value * 1e-7, (first.value + second.value) * 1e-7]

    def GetRecEvents(self):
        self.call('Cmd_StopEventRecorder', self.h)
        count = _U16()
        self.call('ER_GetEventCount', self.h, _ct.byref(count))
        events, times = [], []
        for unused in range(count.value):
            code, ticks = _U8(), _U32()
            self.call('ER_GetEvent', self.h, _ct.byref(code), _ct.byref(ticks))
            events.append(code.value)
            times.append(ticks.value * 1e-7)
        return [events, times]


_OPERATIONS = frozenset(('Init', 'Reset', 'SetClockChan', 'SetPulseChan',
    'SetGClockChan', 'SetDClockChan', 'SetExternalClockChan', 'SetEventGenChan',
    'SetEvenGenChan', 'StartChan', 'Trigger', 'EventTrigger', 'DummyEvent',
    'GetPhaseCount', 'GetRecEvents'))


def _plain(value):
    if callable(getattr(value, 'data', None)):
        value = value.data()
    if hasattr(value, 'tolist'):
        value = value.tolist()
    if isinstance(value, (list, tuple)):
        return [_plain(item) for item in value]
    if isinstance(value, bytes):
        return value.decode('utf-8')
    return value




class DIO4(Device):
    """INCAA DIO4 Timing Module; local hardware access only."""
    # Retain unused :SW_MODE and :IP_ADDR nodes to preserve tree offsets.
    parts = [{'path': ':BOARD_ID', 'type': 'numeric', 'value': 0}, {'path': ':SW_MODE', 'type': 'text', 'value': 'LOCAL'}, {'path': ':IP_ADDR', 'type': 'text'}, {'path': ':COMMENT', 'type': 'text'}, {'path': ':CLOCK_SOURCE', 'type': 'text', 'value': 'INTERNAL'}, {'path': ':REC_START_EV', 'type': 'text', 'state': '0'}, {'path': ':REC_EVENTS', 'type': 'text'}, {'path': ':REC_TIMES', 'type': 'numeric'}, {'path': ':SYNCH', 'type': 'text', 'value': 'NO'}, {'path': ':SYNCH_EVENT', 'type': 'text'}]
    for i in range(8):
        parts.append({'path': '.CHANNEL_%d' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d:FUNCTION' % (i + 1), 'type': 'text', 'value': 'CLOCK'})
        parts.append({'path': '.CHANNEL_%d:TRIG_MODE' % (i + 1), 'type': 'text', 'value': 'SOFTWARE'})
        parts.append({'path': '.CHANNEL_%d:EVENT' % (i + 1), 'type': 'text'})
        parts.append({'path': '.CHANNEL_%d:CYCLIC' % (i + 1), 'type': 'text', 'value': 'NO'})
        parts.append({'path': '.CHANNEL_%d:DELAY' % (i + 1), 'type': 'numeric', 'value': 0.0})
        parts.append({'path': '.CHANNEL_%d:DURATION' % (i + 1), 'type': 'numeric', 'value': 1.0})
        parts.append({'path': '.CHANNEL_%d:FREQ_1' % (i + 1), 'type': 'numeric', 'value': 1000})
        parts.append({'path': '.CHANNEL_%d:FREQ_2' % (i + 1), 'type': 'numeric', 'value': 1000})
        parts.append({'path': '.CHANNEL_%d:INIT_LEV_1' % (i + 1), 'type': 'text', 'value': 'LOW'})
        parts.append({'path': '.CHANNEL_%d:INIT_LEV_2' % (i + 1), 'type': 'text', 'value': 'LOW'})
        parts.append({'path': '.CHANNEL_%d:DUTY_CYCLE' % (i + 1), 'type': 'numeric', 'value': 50})
        parts.append({'path': '.CHANNEL_%d:TRIGGER' % (i + 1), 'type': 'numeric', 'value': 0})
        parts.append({'path': '.CHANNEL_%d:CLOCK' % (i + 1), 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_%d:TRIGGER_1' % (i + 1), 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_%d:TRIGGER_2' % (i + 1), 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_%d:COMMENT' % (i + 1), 'type': 'text'})
        parts.append({'path': '.CHANNEL_%d:OUT_EV1' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV1:NAME' % (i + 1), 'type': 'text'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV1:CODE' % (i + 1), 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV1:TIME' % (i + 1), 'type': 'numeric', 'value': 0})
        parts.append({'path': '.CHANNEL_%d.OUT_EV1:TERM' % (i + 1), 'type': 'text', 'value': 'NO'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV1:EDGE' % (i + 1), 'type': 'text', 'value': 'RISING'})
        parts.append({'path': '.CHANNEL_%d:OUT_EV2' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV2:NAME' % (i + 1), 'type': 'text'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV2:CODE' % (i + 1), 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV2:TIME' % (i + 1), 'type': 'numeric', 'value': 0})
        parts.append({'path': '.CHANNEL_%d.OUT_EV2:TERM' % (i + 1), 'type': 'text', 'value': 'NO'})
        parts.append({'path': '.CHANNEL_%d.OUT_EV2:EDGE' % (i + 1), 'type': 'text', 'value': 'RISING'})
    del i
    parts.append({'path': '.OUT_EV_SW', 'type': 'structure'})
    parts.append({'path': '.OUT_EV_SW:NAME', 'type': 'text'})
    parts.append({'path': '.OUT_EV_SW:CODE', 'type': 'numeric'})
    parts.append({'path': '.OUT_EV_SW:TIME', 'type': 'numeric', 'value': 0})
    parts.append({'path': '.CHANNEL_1:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_2:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_3:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_4:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_5:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_6:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_7:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': '.CHANNEL_8:TERM', 'type': 'text', 'value': 'NO'})
    parts.append({'path': ':INIT_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options': ('no_write_shot',)})
    parts.append({'path': ':STORE_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)})
    parts.append({'path': ':RESET_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('CPCI_SERVER','RESET',50,None),Method(None,'RESET',head))", 'options': ('no_write_shot',)})

    def INIT(self):
        if Device.debug:
            print('INIT')
        try:
            boardId = self.board_id.data()
            if Device.debug:
                print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        clockSourceDict = {'INTERNAL': 0, 'HIGHWAY': 1, 'EXTERNAL': 2}
        try:
            clockSource = clockSourceDict[self.clock_source.data()]
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK_SOURCE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if Device.debug:
            print('CLOCK_SOURCE: ' + self.clock_source.data() + ' - ID: ' + str(clockSource))
        recStartEv = 0
        if getattr(self, 'rec_start_ev').isOn():
            try:
                recStartEv = Data.execute('TimingDecodeEvent($1)', self.rec_start_ev)
            except:
                recStartEv = 0
        print('REC_START_EV: ' + str(recStartEv))
        synchEvents = []
        try:
            synch = self.synch.data()
            print('synch: ' + synch)
            if synch == 'YES':
                synchEvent = self.synch_event.data()
                synchEvSize = Data.execute('size($1)', synchEvent)
                if Device.debug:
                    print('synch event size: ' + str(synchEvSize))
                if synchEvSize == 1:
                    synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent))
                else:
                    for i in range(synchEvSize):
                        synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent[i]))
                    del i
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SYNCH_EVENT')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        status = self._hardware('Init', boardId, clockSource, recStartEv, makeArray(synchEvents))
        if status == 0:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW initialization')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        channelMask = 0
        huge = 2147483647
        huge = float(3.4028234663852886e+38)
        if getattr(self, 'out_ev_sw').isOn():
            if Device.debug:
                print('SW EVENT IS ON')
            try:
                evName = getattr(self, 'out_ev_sw_name').data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            if evCode != 0:
                setattr(self, 'out_ev_sw_code', evCode)
            else:
                try:
                    evCode = int(self.out_ev_sw_code.data())
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                if Device.debug:
                    print('LEGGO evTime')
                evTime = getattr(self, 'out_ev_sw_time').data()
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            if Device.debug:
                print('NAME: ' + evName)
            if Device.debug:
                print('PATH: ' + nodePath)
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if Device.debug:
                print('FATTO')
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        elif Device.debug:
            print('SW EVENT IS OFF')
        for c in range(8):
            if getattr(self, 'channel_%d' % (c + 1)).isOn():
                channelMask = channelMask | 1 << c
                try:
                    function = getattr(self, 'channel_%d_function' % (c + 1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if Device.debug:
                    print('FUNCTION: ' + function)
                if function == 'CLOCK':
                    try:
                        frequency = getattr(self, 'channel_%d_freq_1' % (c + 1)).data()
                        print('FREQ: ' + str(frequency))
                        dutyCycle = getattr(self, 'channel_%d_duty_cycle' % (c + 1)).data()
                        evTermDict = {'NO': 0, 'YES': 1}
                        evTerm = getattr(self, 'channel_%d_term' % (c + 1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid clock parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    status = self._hardware('SetClockChan', boardId, c, frequency, dutyCycle, evTermCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    period = int(1.0 / frequency / 1e-07 + 0.5) * 1e-07
                    getattr(self, 'channel_%d_clock' % (c + 1)).putData(Range(None, None, period))
                elif function == 'PULSE':
                    trigModeDict = {'EVENT': 0, 'RISING EDGE': 1, 'FALLING EDGE': 2, 'SOFTWARE': 3}
                    try:
                        trigMode = getattr(self, 'channel_%d_trig_mode' % (c + 1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self, 'channel_%d_event' % (c + 1)).data()
                                eventSize = Data.execute('size($1)', event)
                                print('event: ' + str(event))
                                print('event size: ' + str(eventSize))
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    if Device.debug:
                                        print('eventTime: ' + str(eventTime))
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self, 'channel_%d_trigger' % (c + 1)).data()
                                    else:
                                        setattr(self, 'channel_%d_trigger' % (c + 1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                if Device.debug:
                                    print('eventCodes: ' + str(eventCodes))
                                getattr(self, 'channel_%d_trigger' % (c + 1)).data()
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'error events')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        cyclicDict = {'NO': 0, 'YES': 1}
                        levelDict = {'LOW': 0, 'HIGH': 1}
                        cyclic = cyclicDict[getattr(self, 'channel_%d_cyclic' % (c + 1)).data()]
                        if Device.debug:
                            print(cyclic)
                        initLev1 = levelDict[getattr(self, 'channel_%d_init_lev_1' % (c + 1)).data()]
                        if Device.debug:
                            print(initLev1)
                        initLev2 = levelDict[getattr(self, 'channel_%d_init_lev_2' % (c + 1)).data()]
                        if Device.debug:
                            print(initLev2)
                        duration = getattr(self, 'channel_%d_duration' % (c + 1)).data()
                        if Device.debug:
                            print(duration)
                        delay = getattr(self, 'channel_%d_delay' % (c + 1)).data()
                        if Device.debug:
                            print(delay)
                        evTermDict = {'NO': 0, 'YES': 1}
                        evTerm = getattr(self, 'channel_%d_term' % (c + 1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Pulse parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    status = self._hardware('SetPulseChan', boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes), evTermCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW pulse setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    try:
                        trig_path = getattr(self, 'channel_%d_trigger' % (c + 1)).getFullPath()
                        delay_path = getattr(self, 'channel_%d_delay' % (c + 1)).getFullPath()
                        duration_path = getattr(self, 'channel_%d_duration' % (c + 1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' + duration_path
                        if Device.debug:
                            print(trigger_1)
                        if Device.debug:
                            print(trigger_2)
                        getattr(self, 'channel_%d_trigger_1' % (c + 1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2' % (c + 1)).putData(Data.compile(trigger_2))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write Pulse parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                elif function == 'GCLOCK':
                    trigModeDict = {'EVENT': 0, 'RISING EDGE': 1, 'FALLING EDGE': 2, 'SOFTWARE': 3}
                    try:
                        trigMode = getattr(self, 'channel_%d_trig_mode' % (c + 1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self, 'channel_%d_event' % (c + 1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                if Device.debug:
                                    print(eventSize)
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self, 'channel_%d_trigger' % (c + 1)).data()
                                    else:
                                        setattr(self, 'channel_%d_trigger' % (c + 1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        if Device.debug:
                                            print(eA[i])
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        frequency = getattr(self, 'channel_%d_freq_1' % (c + 1)).data()
                        duration = getattr(self, 'channel_%d_duration' % (c + 1)).data()
                        if Device.debug:
                            print('duration: ' + str(duration))
                        delay = getattr(self, 'channel_%d_delay' % (c + 1)).data()
                        dutyCycle = getattr(self, 'channel_%d_duty_cycle' % (c + 1)).data()
                        cyclicDict = {'NO': 0, 'YES': 1}
                        cyclic = cyclicDict[getattr(self, 'channel_%d_cyclic' % (c + 1)).data()]
                        evTermDict = {'NO': 0, 'YES': 1}
                        evTerm = getattr(self, 'channel_%d_term' % (c + 1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Gated Clock parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    status = self._hardware('SetGClockChan', boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle, cyclic, evTermCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW GClock setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    try:
                        trig_path = getattr(self, 'channel_%d_trigger' % (c + 1)).getFullPath()
                        delay_path = getattr(self, 'channel_%d_delay' % (c + 1)).getFullPath()
                        duration_path = getattr(self, 'channel_%d_duration' % (c + 1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' + duration_path
                        period = int(1.0 / frequency / 1e-07 + 0.5) * 1e-07
                        getattr(self, 'channel_%d_clock' % (c + 1)).putData(Range(Data.compile(trigger_1), Data.compile(trigger_2), period))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write GCLOCK parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                elif function == 'DCLOCK':
                    trigModeDict = {'EVENT': 0, 'RISING EDGE': 1, 'FALLING EDGE': 2, 'SOFTWARE': 3}
                    try:
                        trigMode = getattr(self, 'channel_%d_trig_mode' % (c + 1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self, 'channel_%d_event' % (c + 1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                print(eventSize)
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self, 'channel_%d_trigger' % (c + 1)).data()
                                    else:
                                        setattr(self, 'channel_%d_trigger' % (c + 1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        print(eA[i])
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        freq1 = getattr(self, 'channel_%d_freq_1' % (c + 1)).data()
                        freq2 = getattr(self, 'channel_%d_freq_2' % (c + 1)).data()
                        duration = getattr(self, 'channel_%d_duration' % (c + 1)).data()
                        delay = getattr(self, 'channel_%d_delay' % (c + 1)).data()
                        evTermDict = {'NO': 0, 'YES': 1}
                        evTerm = getattr(self, 'channel_%d_term' % (c + 1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Dual Speed Clock parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    status = self._hardware('SetDClockChan', boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), int(getattr(self, 'channel_%d_cyclic' % (c + 1)).data() == 'YES'), evTermCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW DClock setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    try:
                        trig_path = getattr(self, 'channel_%d_trigger' % (c + 1)).getFullPath()
                        delay_path = getattr(self, 'channel_%d_delay' % (c + 1)).getFullPath()
                        duration_path = getattr(self, 'channel_%d_duration' % (c + 1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' + duration_path
                        getattr(self, 'channel_%d_trigger_1' % (c + 1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2' % (c + 1)).putData(Data.compile(trigger_2))
                        period1 = int(1.0 / freq1 / 1e-07 + 0.5) * 1e-07
                        period2 = int(1.0 / freq2 / 1e-07 + 0.5) * 1e-07
                        trig1_path = getattr(self, 'channel_%d_trigger_1' % (c + 1)).getFullPath()
                        trig2_path = getattr(self, 'channel_%d_trigger_2' % (c + 1)).getFullPath()
                        setattr(self, 'channel_%d_clock' % (c + 1), Data.compile('BUILD_RANGE([-1E6,' + trig1_path + ',' + trig2_path + '],[' + trig1_path + ',' + trig2_path + ',1E6],[' + str(period1) + ',' + str(period2) + ',' + str(period1) + '])'))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write DCLOCK parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                elif function == 'ENCODER':
                    for e in range(2):
                        if getattr(self, 'channel_%d_out_ev%d' % (c + 1, e + 1)).isOn():
                            try:
                                evName = getattr(self, 'channel_%d_out_ev%d_name' % (c + 1, e + 1)).data()
                            except:
                                evName = ''
                            if evName != '':
                                evCode = Data.execute('TimingDecodeEvent($1)', evName)
                            else:
                                evCode = 0
                            if Device.debug:
                                print('evCode: ' + str(evCode))
                            if evCode != 0:
                                nodePath = getattr(self, 'channel_%d_out_ev%d_code' % (c + 1, e + 1)).getFullPath()
                                if Device.debug:
                                    print(nodePath)
                                setattr(self, 'channel_%d_out_ev%d_code' % (c + 1, e + 1), evCode)
                            else:
                                try:
                                    evCode = getattr(self, 'channel_%d_out_ev%d_code' % (c + 1, e + 1)).data()
                                except:
                                    evCode = 0
                            if evCode == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            try:
                                evTime = getattr(self, 'channel_%d_out_ev%d_time' % (c + 1, e + 1)).data()
                            except:
                                evTime = huge
                            if evTime == huge:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            nodePath = getattr(self, 'channel_%d_out_ev%d_time' % (c + 1, e + 1)).getFullPath()
                            status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
                            if status == -1:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register event time')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            evTermDict = {'NO': 0, 'YES': 1}
                            try:
                                evTerm = getattr(self, 'channel_%d_out_ev%d_term' % (c + 1, e + 1)).data()
                                evTermCode = evTermDict[evTerm]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event termination specification for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            evEdgeDict = {'FALLING': 0, 'RISING': 1}
                            try:
                                evEdge = getattr(self, 'channel_%d_out_ev%d_edge' % (c + 1, e + 1)).data()
                                evEdgeCode = evEdgeDict[evEdge]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event edge specification for channel %d' % (c + 1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            if Device.debug:
                                print('channel: ' + str(c))
                            realChannel = 2 * c + 1 + e
                            if Device.debug:
                                print('real channel: ' + str(realChannel))
                            if Device.debug:
                                print('code: ' + str(evCode))
                            status = self._hardware('SetEventGenChan', boardId, realChannel, evCode, evTermCode, evEdgeCode)
                            if status == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW set event setup')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        elif Device.debug:
                            print('OFF')
                elif function == 'CLOCK SOURCE + CLOCK':
                    try:
                        freq1 = getattr(self, 'channel_%d_freq_1' % (c + 1)).data()
                        freq2 = getattr(self, 'channel_%d_freq_2' % (c + 1)).data()
                        dutyCycle = getattr(self, 'channel_%d_duty_cycle' % (c + 1)).data()
                        evTermDict = {'NO': 0, 'YES': 1}
                        evTerm = getattr(self, 'channel_%d_term' % (c + 1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK SOURCE + CLOCK parameters for channel %d' % (c + 1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    status = self._hardware('SetExternalClockChan', boardId, c, freq1, freq2, dutyCycle, evTermCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    period = int(1.0 / freq2 / 1e-07 + 0.5) * 1e-07
                    getattr(self, 'channel_%d_clock' % (c + 1)).putData(Range(None, None, period))
        status = self._hardware('StartChan', boardId, channelMask, synch)
        if status == 0:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO4 Device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

    def RESET(self):
        print('RESET')
        try:
            boardId = self.board_id.data()
            if Device.debug:
                print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        status = self._hardware('Reset', boardId)
        if status == 0:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW reset')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

    def STORE(self):
        board = int(self.board_id.data())
        events, times = self._hardware('GetRecEvents', board)
        names = [str(_plain(Data.execute('TimingEncodeEvent($1)', int(code)))) for code in events]
        origin = 0.0
        if self.rec_start_ev.isOn():
            name = self.rec_start_ev.data()
            if str(name):
                origin = float(Data.execute('TimingGetEventTime($1)', name))
                if not _math.isfinite(origin) or abs(origin) >= 3.4028234663852886e+38:
                    raise DIO4Error('Recorder start event time is undefined')
        from MDSplus import StringArray, Float64Array
        self.rec_events.putData(StringArray(names))
        self.rec_times.putData(Float64Array([time + origin for time in times]))
        for c in range(8):
            if not getattr(self, 'channel_%d' % (c + 1)).isOn():
                continue
            function = getattr(self, 'channel_%d_function' % (c + 1)).data()
            if function in ('PULSE', 'GCLOCK', 'DCLOCK'):
                phases = self._hardware('GetPhaseCount', board, c)
                trigger = getattr(self, 'channel_%d_trigger' % (c + 1))
                for phase, offset in enumerate(phases, 1):
                    getattr(self, 'channel_%d_trigger_%d' % (c + 1, phase)).putData(Data.compile('$1 + $2', trigger, float(offset)))
        return 1

    def TRIGGER(self):
        if Device.debug:
            print('TRIGGER')
        try:
            boardId = self.board_id.data()
            print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d' % (c + 1)).isOn():
                try:
                    function = getattr(self, 'channel_%d_function' % (c + 1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK':
                    trigModeDict = {'EVENT': 0, 'RISING EDGE': 1, 'FALLING EDGE': 2, 'SOFTWARE': 3}
                    try:
                        trigMode = getattr(self, 'channel_%d_trig_mode' % (c + 1)).data()
                        if trigMode not in trigModeDict.keys():
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid trigger mode')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                        if trigMode == 'SOFTWARE':
                            channelMask = channelMask | 1 << c
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Error setting trigger mode')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
        if channelMask != 0:
            status = self._hardware('Trigger', boardId, channelMask)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute trigger')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        huge = 2147483647
        if getattr(self, 'out_ev_sw').isOn():
            try:
                evName = getattr(self, 'out_ev_sw_name').data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            print('SW Event ', evCode)
            if evCode != 0:
                setattr(self, 'out_ev_sw_code', evCode)
            else:
                try:
                    evCode = getattr(self, 'out_ev_sw_code').data()
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                evTime = getattr(self, 'out_ev_sw_time').data()
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            status = self._hardware('EventTrigger', boardId, evCode)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            print('SW EVENT OFF')
        return 1

    def _hardware(self, operation, board, *args):
        """Execute a hardware operation on the locally attached DIO4 board."""
        if operation not in _OPERATIONS:
            raise ValueError('Unsupported DIO4 operation')
        args = [_plain(value) for value in args]
        try:
            with _DIO4Hardware().opened(board) as hw:
                return getattr(hw, operation)(*args)
        except Exception as exc:
            Data.execute('DevLogErr($1, $2)', self.nid, str(exc))
            raise
