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

import time

import _acq400


class WRITER(_acq400.WRITER):
    """Write data to tree."""

    def __init__(self, dev):
        super(WRITER, self).__init__(dev)
        self.uut = dev.acq400()
        self.controller = dev.acq400_hapi.ShotController(self.uut)
        dev.dprint(1, "Using HAPI ShotController to run the shot.")

    def write(self):
        """Consume data and write to tree."""
        self.controller.run_shot()
        # wait for done
        while self.uut.statmon.get_state() != 0:
            time.sleep(.1)
        channel_data = self.uut.read_channels()
        self.dprint('Trig T0  %s' % (self.dev._trigger,))
        self.dprint('Trig TAI %s' % (self.dev.trigger.record,))

        first = - self._trigger_pre_samples
        last = self.samples.data() - 1

        for i, ch in enumerate(self.dev.gen_for_all(self.dev.input_node)):
            if ch.on:
                ch.putData(MDSplus.Signal(
                    self.dev.get_input_scaling_expr(i),
                    channel_data[i],
                    self.dev.get_dimension_expr(
                        first, last, self.get_input_range_expr(i))))

class CLASS(_acq400.CLASS):
    """Transient capture."""

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

    def stop(self):
        """Stop the acquisiton."""
        uut =  self.acq400()
        print("PRE %d, POST %d and ELAPSED %d" % (
            uut.pre_samples(), uut.post_samples(), uut.elapsed_samples()))
        uut.s0.set_abort=1


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