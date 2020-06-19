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

import numpy

import _acq400

class CLASS(_acq400.CLASS):
    """
    A sub-class of _ACQ400_TR_BASE that includes functions for MR data
    and the extra nodes for MR processing.
    """

    def init(self):
        import acq400_hapi
        # args.uuts = [ acq400_hapi.Acq2106(u, has_comms=False) for u in args.uut ]
        uut = acq400_hapi.Acq2106(self.node.data())
        # master = args.uuts[0]
        self.STL.putData("/home/dt100/PROJECTS/acq400_hapi/user_apps/STL/acq2106_test10.stl")
        with open(self.STL.data(), 'r') as fp:
            stl = fp.read()
        lit_stl = self.denormalise_stl(stl)
        uut.s0.SIG_SRC_TRG_0 = 'NONE'
        # for u in args.uuts:
        # acq400_hapi.Acq400UI.exec_args(uut, args)
        uut.s0.gpg_trg = '1,0,1'
        uut.s0.gpg_clk = '1,1,1'
        uut.s0.GPG_ENABLE = '0'
        uut.load_gpg(lit_stl, 2)
        evsel0 = self.evsel0.data()
        uut.set_MR(True, evsel0=evsel0, evsel1=evsel0+1, MR10DEC=self.MR10DEC.data())
        uut.s0.set_knob('SIG_EVENT_SRC_%d' % (evsel0,), 'GPG')
        uut.s0.set_knob('SIG_EVENT_SRC_%d' % (evsel0+1,), 'GPG')
        uut.s0.GPG_ENABLE = '1'

    def arm(self):
        """ARM the acq2106_MR setup."""
        def selects_trg_src(uut, src):
            def select_trg_src():
                uut.s0.SIG_SRC_TRG_0 = src
            return select_trg_src
        uut = self.acq400()
        shot_controller = self.acq400_hapi.ShotController(uut)
        shot_controller.run_shot(
            remote_trigger=selects_trg_src(uut, self._trg0_src))

    def denormalise_stl(self, stl):
        # lines = args.stl.splitlines()
        lines = stl.splitlines()
        stl_literal_lines = []
        for line in lines:
            if line.startswith('#') or len(line) < 2:
                # if args.verbose:
                #     print(line)
                continue
            else:
                action = line.split('#')[0]
                if action.startswith('+'): # support relative increments
                    delayp = '+'
                    action  = action[1:]
                else:
                    delayp = ''
                delay, state = [int(x) for x in action.split(',')]
                # delayk = int(delay * self.Fclk.data() / 1000000)
                delayk = int(delay * 40000000 / 1000000)
                delaym = delayk - delayk % self.MR10DEC.data()
                state = state << self.evsel0.data()
                elem = "%s%d,%02x" % (delayp, delaym, state)
                stl_literal_lines.append(elem)
                # if args.verbose:
                #     print(line)
        return "\n".join(stl_literal_lines)

    def store(self):
        uut = self.acq400()

        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        uut.fetch_all_calibration()
        eslo = uut.cal_eslo[1:]
        eoff = uut.cal_eoff[1:]
        channel_data = uut.read_channels()

        for ic, ch in enumerate(self.chans):
            if ch.on:
                # TODO: the calib should be part of the signal
                # SIGNAL($VALUE * .ESLO + .EOFF, raw, dim)
                ch.putData(channel_data[ic])
                ch.EOFF.putData(float(eoff[ic]))
                ch.ESLO.putData(float(eslo[ic]))
                ch.CAL_INPUT.putData(
                    ch.tree.tdiCompile("$ * $ + $", ch, ch.ESLO, ch.EOFF))

        decims = uut.read_decims()
        dt = 1 / ((round(float(uut.s0.SIG_CLK_MB_FREQ.split(" ")[1]), -4)) * 1e-9)
        tb = numpy.zeros(decims.shape[-1])
        ttime = 0
        for ix, dec in enumerate(decims):
                tb[ix] = ttime
                ttime += float(dec) * dt
        self.DECIMS.putData(decims)
        self.DT.putData(dt)
        self.TB_NS.putData(tb)

    def deinit(self):
        self.store()

PARTS = _acq400.PARTS + [
    {'path':':DT',
     'type':'numeric',
     'options':('write_shot',),
    },
    {'path':':TB_NS',
     'type':'signal',
     'options':('no_write_model','write_once',),
    },
    {'path':':DECIMS',
     'type':'signal',
     'options':('no_write_model','write_once',),
    },
    {'path':':Fclk',
     'type':'numeric',
     'options':('write_shot',),
     'value':40000000,
    },
    {'path':':trg0_src',
     'type':'text',
     'options':('write_model',),
     'value':'EXT',
    },
    {'path':':evsel0',
     'type':'numeric',
     'options':('write_model',),
     'value':4,
    },
    {'path':':MR10DEC',
     'type':'numeric',
     'options':('write_model',),
     'value':8,
    },
    {'path':':STL',
     'type':'text',
     'options':('write_model',),
    }
]

INPUTS = _acq400.INPUTS

BUILD = _acq400.GENERATE_BUILD(CLASS, PARTS, INPUTS, 'MR')
