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

import threading
import time

import _acq400

class SHOTCONNECTOR:
    """Adds get_shot_controller to CLASS."""

    def get_shot_controller(self, *a, **kw):
        """Instantiate a ShotController."""
        if self.use_mockup:
            class ShotController:
                @staticmethod
                def run_shot():
                    pass
            return ShotController
        else:
            return self.acq400_hapi.ShotController(*a, **kw)

class MOCKUP_ACQ400(_acq400.MOCKUP_ACQ400):
    """Extends MOCKUP with statmon."""

    class statmon:
        """Monitors the state of the device on monitor port."""

        @staticmethod
        def get_state():
            """Return state - 0 is idle."""
            return 0

class WRITER(_acq400.WRITER):
    """Write data to tree."""

    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.uut = dev.acq400(True)
        self.first = - max(dev._trigger_pre_samples, 0)
        dev.dprint(1, "Using HAPI ShotController to run the shot.")
        controller = dev.get_shot_controller([self.uut])
        threading.Thread(target=controller.run_shot).start()

    def abort(self, *abort):
        """Abort and set abort."""
        super(WRITER, self).abort()
        self.uut.s0.set_abort=1

    def write(self):
        """Consume data and write to tree."""
        self.dev.callibrations  # prefetch callibrations
        while (self.uut.statmon.get_state() != 0
               and not self.was_stopped
               and not self.was_aborted):
            time.sleep(.1)
        self.dprint(1, 'READ CHANNELS')
        channel_data = self.uut.read_channels()
        self.dprint(1, 'Trig T0  %s', self.dev._trigger)
        self.dprint(1, 'Trig TAI %s', self.dev.trigger.record)

        last = self.first + channel_data[0].size - 1

        for i, ch in enumerate(self.dev.gen_for_all(self.dev.input_node)):
            if ch.on:
                self.dprint(5, 'STORE for %s', ch)
                ch.putData(MDSplus.Signal(
                    self.dev.get_input_scaling_expr(i),
                    channel_data[i],
                    self.dev.get_dimension_expr(
                        self.first, last, self.dev.get_input_range_expr(i))))

class CLASS(_acq400.CLASS, SHOTCONNECTOR):
    """Transient capture."""

    _MOCKUP_ACQ400 = MOCKUP_ACQ400
    _WORKER = WRITER

    def init(self):
        """Initialize device."""
        self.dprint(1, "ACQ400_TRANSIENT_INIT")
        _acq400.CLASS.init(self)
        uut = self.acq400()

        # Initializing Sources to NONE:
        # D0 signal:
        uut.s0.SIG_SRC_TRG_0   = 'NONE'
        # D1 signal:
        uut.s0.SIG_SRC_TRG_1   = 'NONE'

        #Trigger sources choices:
        # d0:
        srcs_0 = ['EXT', 'HDMI', 'HOSTB', 'GPG0', 'DSP0', 'WRTT0', 'nc', 'NONE']
        # d1:
        srcs_1 = ['STRIG', 'HOSTA', 'HDMI_GPIO', 'GPG1', 'DSP1', 'FP_SYNC', 'WRTT1', 'NONE']

        # When WR is not being used to trigger:
        if self._wrtd == 0:
            # TRIGGER setting
            trig_src = self._trigger_src
            if trig_src in srcs_1:
                # uut.s0.SIG_SRC_TRG_1 = 'STRIG'
                uut.s0.SIG_SRC_TRG_1 = trig_src
                #Setting the signal (dX) to use for ACQ2106 stream control
                uut.s1.TRG = 'enable'
                uut.s1.TRG_DX = 'd1'
                uut.s1.TRG_SENSE = 'rising'
                #EVENT0 setting in d0:
                uut.s0.SIG_SRC_TRG_0 = self._trigger_event0_src
                uut.s1.EVENT0 = 'enable'
                uut.s1.EVENT0_DX = 'd0'
                uut.s1.EVENT0_SENSE = 'rising'
                # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
                uut.s0.SIG_EVENT_SRC_0 = 'TRG'
            elif trig_src in srcs_0:
                # uut.s0.SIG_SRC_TRG_0 = 'EXT'
                uut.s0.SIG_SRC_TRG_0 = trig_src
                #Setting the signal (dX) to use for ACQ2106 stream control
                uut.s1.TRG = 'enable'
                uut.s1.TRG_DX = 'd0'
                uut.s1.TRG_SENSE = 'rising'
                #EVENT0 setting in d1:
                uut.s0.SIG_SRC_TRG_0= self._trigger_event0_src
                uut.s1.EVENT0  = 'enable'
                uut.s1.EVENT0_DX = 'd1'
                uut.s1.EVENT0_SENSE = 'rising'
                # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
                uut.s0.SIG_EVENT_SRC_0 = 'TRG'
        else:
            #If WR is used to trigger, then:
            uut.s1.TRG = 'enable'
            uut.s1.TRG_DX = self._trigger_dx
            uut.s1.TRG_SENSE = 'rising'

            uut.s1.EVENT0 = 'enable'
            uut.s1.EVENT0_DX = self._trigger_event0_dx
            uut.s1.EVENT0_SENSE = 'rising'

    def state(self):
        """State of the Transient recorder NOW (when this method was run)."""
        uut = self.acq400()
        _status = [int(x) for x in uut.s0.state.split(" ")]
        trstate = uut.STATE.str(_status[0])

        print("TR state: %s" % (trstate,))
        print("TR status: %s" % (_status,))


PARTS = _acq400.PARTS + [
    {'path':':TRIG_TIME',
     'type': 'numeric',
     'options':('write_shot',),
    },
    #Trigger sources
    {'path': ':TRIGGER:SRC',
     'type': 'text',
     'options': ('write_shot',),
     'filter': str,
     'value': 'NONE',
    },
    {'path': ':TRIGGER:DX',
     'type': 'text',
     'options': ('write_shot',),
     'filter': str,
     'value': 'dx',
    },
    #Event sources
    {'path': ':TRIGGER:EVENT0_SRC',
     'type': 'text',
     'options': ('write_shot',),
     'filter': str,
     'value': 'NONE',
    },
    {'path':':TRIGGER:EVENT0_DX',
     'type': 'text',
     'options': ('write_shot',),
     'filter': str,
     'value': 'dx',
    },
    #WRTD tree infilterion
    {'path': ':WRTD',
     'type': 'numeric',
     'options': ('write_shot',),
     'filter': int,
     'value': 0,
    },
    {'path': ':WRTD:TREE',
     'type': 'text',
     'options': ('no_write_shot'),
     'filter': str,
     'value': 'WRTD',
    },
]

INPUTS = _acq400.INPUTS

BUILD = _acq400.GENERATE_BUILD(CLASS, PARTS, INPUTS, "TR")

# tests
if __name__ == '__main__':
    import os
    MDSplus.Device._Device__cached_mds_pydevice_path = None
    MDSplus.setenv("MDS_PYDEVICE_PATH", os.path.dirname(__file__))
    MDSplus.setenv("test_path", '/tmp')
    with MDSplus.Tree("test",-1,'new') as t:
        # MDSplus.Device.findPyDevices()['ACQ2106_ST_32'].Add(t, 'DEV')
        d = t.addDevice("DEVICE","ACQ2106_TR_8")
        t.write()
    t.open()
    d.ACTION.record = 'ACTION_SERVER'
    d.UUT.record = "daq-e5-qxt-1"
    d.TRIGGER.MODE.record = 'soft'
    t.createPulse(1)
    t.open(shot=1)
    d.debug = 5
    d.use_mockup = False
    try:
        d.init()
        d.arm()
        time.sleep(1)
        d.soft_trigger()
        time.sleep(2)
        d.store()
    finally:
        d.deinit()
    print(d.INPUT.CH001.record)
