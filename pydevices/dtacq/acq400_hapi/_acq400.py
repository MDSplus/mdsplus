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
import threading
import time

class MOCKUP_ACQ400:
    """Allow to induce a mockup connection."""

    """Simulates the Acq400 interface of acq400_hapi."""

    def __init__(self, dev, nchan, *a, **kw):
        self._nchan = nchan
    def nchan(self):
        """Return total number of channels of the device."""
        return self._nchan
    def fetch_all_calibration(self):
        """Read all calibrations into cal_eoff andcal_eslo fileds."""
        self.cal_eoff = [self._nchan]+[.001]*self._nchan
        self.cal_eslo = [self._nchan]+[0.01]*self._nchan
    class s0:
        """Site 0 knobs - carrier."""

        trg = 1
        SIG_CLK_S1_FREQ = 5000000

    class s1:
        """Site 1 knobs - master module."""

        trg = 1

    def pre_samples(self):
        """Return the total number of acquired pre samples."""
        for s in self.s0.transient.split(' '):
            if s.upper().startswith('PRE='):
                return int(s[4:])
        else:
            return 0
    def post_samples(self):
        """Return the total number of acquired post samples."""
        for s in self.s0.transient.split(' '):
            if s.upper().startswith('POST='):
                return int(s[5:])
        else:
            return 0

    def elapsed_samples(self):
        """Rerurn the number of total samples processed."""
        return ((self.pre_samples() + self.post_samples() - 1) // 65536 + 1) * 65536

    def read_channels(self):
        """Pull list of all channel data."""
        return numpy.zeros(
            (self._nchan, self.pre_samples()+self.post_samples()), 'i2')

    class statmon:
        """Monitors the state of the device on monitor port."""

        state = 0
        @classmethod
        def get_state(cls):
            """Return state - 0 is idle."""
            cls.state = (cls.state+1) % 7
            return cls.state
class WORKER(threading.Thread):
    """Super class for workers.

    requires work()
    """

    def __init__(self, dev):
        threading.Thread.__init__(
            self, name=dev.name+"-"+self.__class__.__name__)
        self.dprint = dev.dprint
        self.exception = None
        self.stopped = threading.Event()
        self.aborted = threading.Event()

    def stop(self):
        """Request Stream to stop."""
        self.dprint(1, "%s STOP", self.name)
        self.stopped.set()

    def abort(self):
        """Request Stream to stop."""
        self.dprint(1, "%s ABORT", self.name)
        self.aborted.set()
        self.stop()

    @property
    def was_stopped(self):
        """Check stop request - check on timeouts."""
        return self.stopped.is_set()

    @property
    def was_aborted(self):
        """Check stop request - check in every while loop."""
        return self.aborted.is_set()

    def run(self):
        """Run worker task."""
        self.dprint(1, "%s START", self.name)
        try:
            self.work()
        except Exception as e:
            if self.exception:
                e = Exception(self.exception, e)
            self.exception = e
            self.dprint(0, "%s %s: %s", self.name, e.__class__.__name__, e)
        finally:
            self.dprint(1, "%s END", self.name)


    def work(self):
        """Implement this method in sub classes."""
        raise NotImplementedError("Method WORKER.work() was not implemented.")

class WRITER(WORKER):
    """Super class for writers.

    requires write()
    """

    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.dev = dev.copy()  # make run thread safe
        self.dev.debug = dev.debug
        self.dev.use_mockup = dev.use_mockup

    def work(self):
        """Run writer task."""
        self.dev.tree.open()
        self.write()

    def write(self):
        """Implement in subclass."""
        raise NotImplementedError("Method WRITER.write() was not implemented.")

class SIMPLE_WRITER(WRITER):
    """Read transient data once available."""

    def __init__(self, dev):
        super(SIMPLE_WRITER, self).__init__(dev)
        self.first = min(-dev._trigger_pre_samples, 0)
        self.uut = dev.acq400(True)
        self.uut.s0.set_arm = 1
        while self.uut.statmon.get_state() == 0:
            pass

    def abort(self, *abort):
        """Abort and set abort."""
        super(SIMPLE_WRITER, self).abort()
        self.uut.s0.set_abort = 1

    def write(self):
        """Read channels and store in tree."""
        self.dev.callibrations  # prefetch callibrations
        while not (self.was_aborted or self.was_stopped):
            state = self.uut.statmon.get_state()
            self.dprint(5, 'STATE %d', state)
            if state==0: break
            time.sleep(.2)
        self.dprint(1, 'READ CHANNELS')
        channel_data = self.uut.read_channels()
        last = self.first + channel_data[0].size - 1
        for i, ch in enumerate(self.dev.gen_for_all(self.dev.input_node)):
            if ch.on:
                self.dprint(5, 'STORE for %s', ch)
                ch.putData(MDSplus.Signal(
                    self.dev.get_input_scaling_expr(i),
                    channel_data[i],
                    self.dev.get_dimension_expr(
                        self.first, last, self.dev.get_input_range_expr(i))))


class CLASS:
    """D-Tacq ACQ400 Base parts and methods.

    All other carrier/function combinations use this class as a parent class.

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
    MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug
    """

    _MOCKUP_ACQ400 = MOCKUP_ACQ400
    _WORKER = SIMPLE_WRITER

    use_mockup = False
    @property
    def acq400_hapi(self):
        """Import acq400_hapi."""
        import acq400_hapi
        return acq400_hapi


    def acq400(self, monitor=True):
        """Connect to uut with hapi."""
        if self.use_mockup:
            return self._MOCKUP_ACQ400(self, self.nchan)
        return self.acq400_hapi.Acq400(self._uut, monitor=monitor)

    @MDSplus.cached_property
    def uut_nchan(self):
        """Return nchan as provided by the device."""
        return self.acq400(False).nchan()

    @MDSplus.cached_property
    def callibrations(self):
        """Return EOFF and ESLO - cached."""
        uut = self.acq400(False)
        uut.fetch_all_calibration()
        return uut.cal_eoff[1:], uut.cal_eslo[1:]

    @property
    def eff_nchan(self):
        """Return the effective number of channels (min(nodes, uut_nchan))."""
        return min(self.nchan, self.uut_nchan)

    def gen_for_all(self, method, *a, **kw):
        """Return generator of results for all effective inputs."""
        return ( method(i, *a, **kw) for i in range(self.eff_nchan) )

    def list_for_all(self, method, *a, **kw):
        """Return list of results for all effective inputs."""
        return list(self.gen_for_all(method, *a, **kw))

    def get_input_range_expr(self, i):
        """Return expressions of Range for all inputs - used in dim."""
        return self.tree.tdiExecute(
            "BUILD_RANGE(,,$/$)", self.input_decimate(i), self.freq)

    def get_input_scaling_expr(self, i):
        """Generate the scaling expression for channel ch."""
        eoff, eslo = self.callibrations
        return MDSplus.TdiCompile(
            "$VALUE * $ + $", float(eslo[i]), float(eoff[i]))

    def get_dimension_expr(self, first, last, range):
        """Generate the dimension expression."""
        return MDSplus.Dimension(
            MDSplus.Window(first, last, self.trigger), range)

    def init(self):
        """Initialize device."""
        self.dprint(1, "INIT")
        uut = self.acq400()
        trg = self._trigger_mode.lower()
        if trg.startswith('hard'):
            trg_dx = 0
        elif trg.startswith('soft') or trg.startswith('auto'):
            trg_dx = 1
        # The default case is to use the trigger set by sync_role.
        if trg == 'role':
            uut.s0.sync_role = "%s %s" % (self._role, self._freq)
        else:
            # If the user has specified a trigger.
            uut.s0.sync_role = '%s %s TRG:DX=%s' % (self._role, self._freq, trg_dx)

        pre, post = self._trigger_pre_samples, self._trigger_post_samples
        if pre:  # only works with hard trigger
            uut.s1.trg = '1,1,1'
            uut.s1.event0 = '1,%d,1' % (trg_dx,)
        data = "PRE=%d POST=%d" % (pre, post)
        # Now we set the trigger to be soft when desired.
        if trg == 'auto' or pre>0:
            data += ' SOFT_TRIGGER=1'
        elif trg == 'soft' :
            data += ' SOFT_TRIGGER=0'
        self.dprint(1, data)
        uut.s0.transient = data

        #Setting WR Clock to 20MHz by first being sure that MBCLK FIN is in fact = 0
        uut.s0.SIG_CLK_MB_FIN = '0'

        #Set the Sampling rate in the ACQ:
        # MB Clock (WR Clock): 20MHz
        # mb_freq = uut.s0.SIG_CLK_MB_FREQ
        mb_freq = 20000000 #Hz
        freq = self._freq
        self.dprint(1, "Setting sample rate to %d Hz", freq)
        clockdiv = mb_freq // freq

        #The following will set the sample rate, by setting the clock div.
        uut.s1.CLKDIV = str(clockdiv)

        acq_sample_freq = uut.s0.SIG_CLK_S1_FREQ

        self.dprint(1, "After setting the sample rate the value in the ACQ is %r Hz", acq_sample_freq)

        #The following is what the ACQ script called "/mnt/local/set_clk_WR" does to set the WR clock to 20MHz
        uut.s0.SYS_CLK_FPMUX     = 'ZCLK'
        uut.s0.SIG_ZCLK_SRC      = 'INT33M'
        uut.s0.SYS_CLK_Si5326_PLAN = '1'

    def arm(self):
        """Start stream."""
        self.dprint(1, "ACQ400_ARM")
        pers = self.persistent
        if pers is None:
            pers = self.persistent = {}
        else:
            worker = pers.get('worker', None)
            if worker is not None:
                raise MDSplus.DevUNKOWN_STATE
        pers['worker'] = worker = self._WORKER(self)
        worker.start()

    def soft_trigger(self):
        """Send soft trigger."""
        self.dprint(1, "ACQ400_SOFT_TRIGGER")
        if self.use_mockup:
            return
        sock = socket.socket()
        sock.connect((self._uut, 4220))
        try:
            sock.send(b"soft_trigger\n")
            sock.recv(1)
        finally:
            sock.close()
            # self.acq400().s0.soft_trigger

    def _stop(self , abort=False):
        pers = self.persistent
        if not pers and not abort:
            raise MDSplus.DevINV_SETUP
        worker = pers.get('worker', None)
        if worker is not None:
            try:
                if abort:
                    worker.abort()
                worker.stop()
                while worker.is_alive():
                    worker.join(1)
                # allow to propagate exceptions
                if worker.exception:
                    raise worker.exception
            finally:
                del(pers['worker'])

    def store(self):
        """Stop writer and wait for it to finish."""
        self.dprint(1, "ACQ400_STORE")
        self._stop()

    def deinit(self):
        """Abort writer and wait for it to finish."""
        self.dprint(1, "ACQ400_DEINIT")
        self._stop(True)


# The user will need to change the uut and the action server
PARTS = [
    # ACTIONS
    {'path': ':ACTION',
     'type': 'text',
     'options': ('no_write_shot','write_once'),
     'help': "name of the action server or hostname:port or ip:port",
     'filter': str,
    },
    {'path': ':ACTION:INIT',
     'type': 'action',
     'options': ('no_write_shot','write_once'),
     'valueExpr': ("Action("
                   "Dispatch(head.action,'INIT',50),"
                   "Method(None,'init',head))"),
    },
    {'path': ':ACTION:ARM',
     'type': 'action',
     'options': ('no_write_shot','write_once'),
     'valueExpr': ("Action("
                   "Dispatch(head.action,'INIT',50,head.action_init),"
                   "Method(None,'arm',head))"),
    },
    {'path': ':ACTION:SOFT_TRIGGER',
     'type': 'action',
     'options': ('no_write_shot','write_once'),
     'valueExpr': ("Action("
                   "Dispatch(head.action,'TRIGGER',1),"
                   "Method(None,'soft_trigger',head))"),
    },
    {'path': ':ACTION:STORE',
     'type': 'action',
     'options': ('no_write_shot','write_once'),
     'valueExpr': ("Action("
                   "Dispatch(head.action, 'STORE', 50),"
                   "Method(None, 'store', head))"),
    },
    {'path': ':ACTION:DEINIT',
     'type': 'action',
     'options': ('no_write_shot','write_once'),
     'valueExpr': ("Action("
                   "Dispatch(head.action, 'DEINIT', 50),"
                   "Method(None, 'deinit', head))"),
    },
    # DEVICE SELECTION
    {'path': ':UUT',
     'type': 'text',
     'options': ('no_write_shot','write_once'),
     'help': "hostname of the carrier or ip address",
     'filter': str,
    },
    {'path': ':SITE',
     'type': 'numeric',
     'options': ('no_write_shot','write_once'),
     'filter': int,
     'value': 1,
    },
    # DEVICE CONFIGURATION
    {'path': ':ROLE',
     'type': 'text',
     'options': ('no_write_shot'),
     'filter': str,
     'value': 'master',
    },
    {'path': ':FREQ',
     'type': 'numeric',
     'options': ('no_write_shot',),
     'filter': int,
     'help': "sampling rate in Hz",
     'value': 1000000,
    },
    {'path': ':TRIGGER',
     'type': 'numeric',
     'options ': ('write_shot',),
     'help': 'offset timebase by this value to sync to experiment start',
     'value': 0,
    },
    {'path': ':TRIGGER:MODE',
     'type': 'text',
     'options': ('no_write_shot',),
     'filter': str,
     'help': "['hard', 'soft', 'auto', 'role']",
     'value': 'role',
    },
    {'path': ':TRIGGER:POST_SAMPLES',
     'type': 'numeric',
     'options': ('no_write_shot',),
     'filter': int,
     'value': 1000000,
    },
    {'path': ':TRIGGER:PRE_SAMPLES',
     'type': 'numeric',
     'options': ('no_write_shot',),
     'filter': int,
     'value': 0,
    },
]

class INPUTS:
    """Provide parts and access tools for input nodes."""

    @staticmethod
    def STUCTURE():
        """Can be empty but allows to create strucutres."""
        yield {'path': '.INPUT'}

    @classmethod
    def PARTS(cls, i):
        """Return iterable providing parts for channel i."""
        INPUTS_XXX = '.INPUT:CH%03d' % (i+1,)
        yield {
            'path': INPUTS_XXX,
            'type': 'signal',
            'options': ('no_write_model','write_once',),
        }
        yield {
            'path': INPUTS_XXX + ':DECIMATE',
            'type': 'numeric',
            'options': ('no_write_shot'),
            'value': 1,
            'filter': int
        }
        yield {
            'path': INPUTS_XXX + ':OFFSET',
            'type': 'numeric',
            'options': ('no_write_shot'),
            'value': 0,
            'filter': int
        }
    decimate = '_decimate'
    offset = '_offset'
    input_fmt = 'input_ch%03d%s'

    @classmethod
    def get_node(cls, self, i, sub=''):
        """Return the node of input or subnode."""
        return getattr(self, cls.input_fmt % (i+1, sub))
    @classmethod
    def get_data(cls, self, i, sub=''):
        """Return data of input or subnode."""
        return getattr(self, '_' + (cls.input_fmt % (i+1, sub)))

    class ACCESS:
        """Merged with Device class, provides access tools."""

        def input_node(self, i):
            """Return input node."""
            return INPUTS.get_node(self, i)
        def input_decimate(self, i):
            """Return node of input's decimate."""
            return INPUTS.get_node(self, i, INPUTS.decimate)
        def _input_decimate(self, i):
            """Return data of input's decimate."""
            return INPUTS.get_data(self, i, INPUTS.decimate)
        def input_offset(self, i):
            """Return node of input's offset."""
            return INPUTS.get_node(self, i, INPUTS.offset)
        def _input_offset(self, i):
            """Return data of input's offset."""
            return INPUTS.get_data(self, i, INPUTS.offset)


def GENERATE_BUILD(base_class, base_parts, inputs, suffix=None):
    """Generate Device classes in given dict."""
    def generator(nchan):
        for part in base_parts:
            yield part
        for part in inputs.STUCTURE():
            yield part
        for ch in range(nchan):
            for part in inputs.PARTS(ch):
                yield part
    def BUILD(globalsO, prefix, channel_variants):
        module = globalsO.get('__name__', '__main__')
        verbose = module == '__main__'

        root = prefix.upper()
        if suffix:
            root = "%s_%s" % (root, suffix.upper())
        for nchan in channel_variants:
            class_name = "%s_%s" % (root, nchan)
            cls = MDSplus.with_mdsrecords(
                type(class_name, (base_class, inputs.ACCESS, MDSplus.Device), {
                "nchan": nchan,
                "parts": list(generator(nchan)),
                # tell findPyDevices the class belongs to module
                '__module__': module,
            }))
            globalsO[class_name] = cls
            if verbose:
                print('# %s' % class_name)
                if verbose>1:
                    for p in cls.parts:
                        print(p)
    return BUILD

BUILD = GENERATE_BUILD(CLASS, PARTS, INPUTS)

def BUILD_ALL(globalsO, prefix, channel_variants):
    """Generate Device classes for all known variants."""
    if  globalsO.get('__name__', '__main__') == '__main__':
        print("# public classes created in this module")
    from _acq400 import BUILD
    BUILD(globalsO, prefix, channel_variants)
    from _acq400_stream import BUILD
    BUILD(globalsO, prefix, channel_variants)
    from _acq400_transient import BUILD
    BUILD(globalsO, prefix, channel_variants)
    from _acq400_multirate import BUILD
    BUILD(globalsO, prefix, channel_variants)


# tests
if __name__ == '__main__':
    import os
    import sys
    use_mockup = len(sys.argv) < 2
    MDSplus.setenv("MDS_PYDEVICE_PATH", os.path.dirname(__file__))
    MDSplus.setenv("test_path", '/tmp')
    with MDSplus.Tree("test", -1, 'new') as t:
        d = t.addDevice("DEVICE", "ACQ2106_8")
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
        time.sleep(6)
        d.soft_trigger()
        time.sleep(10)
        d.store()
    finally:
        d.deinit()
    print(d.INPUT.CH001.record)