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

class ACQ400_CONNECTOR:
    """Allow to induce a mockup connection."""

    @staticmethod
    def acq400_hapi():
        import acq400_hapi
        return acq400_hapi

    @classmethod
    def ShotController(cls, *a, **kw):
        return cls.acq400_hapi().ShotController(*a, **kw)

    @staticmethod
    def stream(host, port=4210):
        def connect():
            sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((host, port))
            sock.settimeout(1)
            def recv_into(view, toread):
                return sock.recv_into(view, toread)
            return recv_into
        return connect

    @classmethod
    def acq400(cls, host, nchan, *a, **kw):
        return cls.acq400_hapi().Acq400(host, *a, **kw)


class MOCKUP_CONNECTOR:
    """Allow to induce a mockup connection."""

    @staticmethod
    def acq400_hapi(self):
        import acq400_hapi
        return acq400_hapi

    @classmethod
    def ShotController(cls, *a, **kw):
        class ShotController:
            @staticmethod
            def run_shot():
                pass
        return ShotController


    @staticmethod
    def stream(host, port=4210):
        def connect():
            sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((host, port))
            sock.settimeout(1)
            def recv_into(view, toread):
                time.sleep(.001)
                return min(toread, 4096)
            return recv_into
        return connect


    class acq400:
        def __init__(self, host, nchan, *a, **kw):
            self._nchan = nchan
        def nchan(self):
            return self._nchan
        def fetch_all_calibration(self):
            self.cal_eoff = [self._nchan]+[.001]*self._nchan
            self.cal_eslo = [self._nchan]+[0.01]*self._nchan
        class s0:
            trg = 1
            SIG_CLK_S1_FREQ = 5000000
        class s1:
            trg = 1
        def pre_samples(self):
            return 54321
        def post_samples(self):
            return 12345
        def elapsed_samples(self):
            return 66666
        class statmon:
            @staticmethod
            def get_state():
                return 0
        def read_channels(self):
            return numpy.zeros(
                (self._nchan, self.pre_samples()+self.post_samples()), 'i2')

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
            self.dprint(0, "%s ERROR: %s", self.name, e)
        finally:
            self.dprint(1, "%s END", self.name)


    def work(self):
        """Implement this method in sub classes."""
        raise NotImplementedError

class WRITER(WORKER):
    """Super class for writers.

    requires write()
    """

    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.dev = dev.copy()  # make run thread safe

    def work(self):
        """Run writer task."""
        self.dev.tree.open()
        self.write()

    def write(self):
        """Implement this method in sub classes."""
        raise NotImplementedError

class CLASS:
    """D-Tacq ACQ400 Base parts and methods.

    All other carrier/function combinations use this class as a parent class.

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
    MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug
    """

    _WORKER = WORKER

    @property
    def _connector(self):
        """Simulate device if debug level is 6 or higher."""
        if self.debug>5:
            return MOCKUP_CONNECTOR
        else:
            return ACQ400_CONNECTOR

    def acq400(self, monitor=True):
        """Connect to uut with hapi."""
        return self._connector.acq400(self._uut, self.nchan, monitor=monitor)

    def get_stream(self):
        """Return a steam connector."""
        return self._connector.stream(self._uut)
    def get_shot_controller(self, *a, **kw):
        return self._connector.ShotController(*a, **kw)

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
        trg = self._trigger_mode
        if trg == 'hard':
            trg_dx = 0
        elif trg == 'soft':
            trg_dx = 1
        elif trg == 'auto':
            trg_dx = 1
        # The default case is to use the trigger set by sync_role.
        if trg == 'role':
            uut.s0.sync_role = "%s %s" % (self._role, self._freq)
        else:
            # If the user has specified a trigger.
            uut.s0.sync_role = '%s %s TRG:DX=%s' % (self._role, self._freq, trg_dx)

        # Now we set the trigger to be soft when desired.
        if trg == 'soft':
            uut.s0.transient = 'SOFT_TRIGGER=0'
        if trg == 'auto':
            uut.s0.transient = 'SOFT_TRIGGER=1'

        pre, post = self._trigger_pre_samples, self._trigger_post_samples
        data = "PRE=%d POST=%d" % (pre, post)
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
        uut.s0.SIG_ZCLK_SRC      = 'WR31M25'
        uut.s0.set_si5326_bypass = 'si5326_31M25-20M.txt'

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
     'value': 100000,
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
        return getattr(self, cls.input_fmt % (i+1, sub))
    @classmethod
    def get_data(cls, self, i, sub=''):
        return getattr(self, '_' + (cls.input_fmt % (i+1, sub)))

    class ACCESS:
        """Merged with Device class, provides access tools."""

        def input_node(self, i):
            return INPUTS.get_node(self, i)
        def input_data(self, i):
            return INPUTS.get_data(self, i)
        def input_decimate(self, i):
            return INPUTS.get_node(self, i, INPUTS.decimate)
        def _input_decimate(self, i):
            return INPUTS.get_data(self, i, INPUTS.decimate)
        def input_offset(self, i):
            return INPUTS.get_node(self, i, INPUTS.offset)
        def _input_offset(self, i):
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

def BUILD_ALL(globalsO, prefix, channel_variants):
    """Generate Device classes for all known variants."""
    if  globalsO.get('__name__', '__main__') == '__main__':
        print("# public classes created in this module")
    from _acq400_stream import BUILD
    BUILD(globalsO, prefix, channel_variants)
    from _acq400_transient import BUILD
    BUILD(globalsO, prefix, channel_variants)
    from _acq400_mr import BUILD
    BUILD(globalsO, prefix, channel_variants)

