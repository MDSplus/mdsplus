#!/usr/bin/env python
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

import threading
import importlib

acq400_base = importlib.import_module('acq400_base')


class _ACQ2106_TR(acq400_base._ACQ400_TR_BASE):
    """
    D-Tacq ACQ2106 transient support.
    """


    """
    A child class of _ACQ400_BASE that contains the specific methods for
    taking a transient capture.
    """
    tr_base_parts = [
        {'path':':RUNNING',    'type': 'numeric',  'options':('no_write_model',)},
        {'path':':TRIG_TIME',  'type': 'numeric',  'options':('write_shot',)},
        #Trigger sources
        {'path':':TRIG_SRC',   'type':'text',      'value': 'NONE', 'options':('no_write_shot',)},
        {'path':':TRIG_DX',    'type':'text',      'value': 'dx', 'options':('no_write_shot',)},
        #Event sources
        {'path':':EVENT0_SRC', 'type': 'text',     'value': 'NONE', 'options':('no_write_shot',)},
        {'path':':EVENT0_DX',  'type': 'text',     'value': 'dx',    'options':('no_write_shot',)},
        #WRTD tree information
        {'path':':WR',         'type':'numeric',   'value': 0,  'options':('write_shot',)},
            {'path':':WR:WRTD_TREE',   'type': 'text', 'value':'WRTD','options': ('no_write_shot')},
            {'path':':WR:WRTD_T0',     'type': 'numeric', 'value': 0, 'options': ('write_shot')},
            {'path':':WR:TRIG_TAI',    'type': 'numeric', 'value': 0, 'options': ('write_shot'), 'help': 'The record of the shot number of the wrtd this data came from'},
        ]


    def init(self):
        import acq400_hapi
        # Needs the initialization given in the superclass:
        super(_ACQ2106_TR, self).init()

        uut = acq400_hapi.Acq400(self.node.data(), monitor=True)

        # When WR is not being used to trigger:
        if self.wr.data() == 0:
            if self.presamples.data() == 0:
                if 'STRIG' in str(self.trig_src.data()):
                    uut.s0.SIG_SRC_TRG_1   = 'STRIG'   # The only option for EXT triggering is using signal d0
                    #Setting the signal (dX) to use for ACQ2106 stream control
                    uut.s1.TRG       = 'enable'
                    uut.s1.TRG_DX    = 'd1'
                    uut.s1.TRG_SENSE = 'rising'
                elif 'EXT' in str(self.trig_src.data()):
                    uut.s0.SIG_SRC_TRG_0   = 'EXT'   # The only option for EXT triggering is using signal d0
                    #Setting the signal (dX) to use for ACQ2106 stream control
                    uut.s1.TRG       = 'enable'
                    uut.s1.TRG_DX    = 'd0'
                    uut.s1.TRG_SENSE = 'rising'
            else:
                #EVENT0:
                uut.s0.SIG_EVENT_SRC_0 = str(self.event0_src.data()) # Int he EVENT bus, the source needs to be TRG to make the transition PRE->POST
                uut.s0.SIG_SRC_TRG_0   = 'EXT'
                uut.s1.EVENT0       = 'enable'
                uut.s1.EVENT0_DX    = 'd0'  # This is choosen because EVENT0 needs to be TRG to make the transition from PRE->POST
                uut.s1.EVENT0_SENSE = 'rising'
                #TRG: For PRE samples we want software trigger (STRIG):
                uut.s0.SIG_SRC_TRG_1 = 'STRIG'
                #Setting the signal (dX) to use for ACQ2106 stream control
                uut.s1.TRG       = 'enable'
                uut.s1.TRG_DX    = 'd1'
                uut.s1.TRG_SENSE = 'rising'
        #When WR is used to trigger, then:
        else:
            # D1 signal:
            uut.s0.SIG_SRC_TRG_1   = 'NONE'
            # D0 signal:
            uut.s0.SIG_SRC_TRG_0   = 'NONE'

            uut.s1.TRG       = 'enable'
            uut.s1.TRG_DX    = str(self.trig_dx.data())
            uut.s1.TRG_SENSE = 'rising'

            uut.s1.EVENT0       = 'enable'
            uut.s1.EVENT0_DX    = str(self.event0_dx.data())
            uut.s1.EVENT0_SENSE = 'rising'
    INIT=init

    def state(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())
        _status = [int(x) for x in uut.s0.state.split(" ")]

        #statemon = acq400_hapi.acq400.Statusmonitor(self.node.data(), _status)
        #istate   = int(statemon.get_state())

        trstate = acq400_hapi.acq400.STATE.str(_status[0])
        
        print("TR state: {}".format(trstate))
        print("TR status: {}".format(_status))
    STATE=state

    def stop(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())
        print("PRE {}, POST {} and ELAPSED {}".format(uut.pre_samples(), uut.post_samples(), uut.elapsed_samples()))
        uut.s0.set_abort=1
    STOP = stop

    def pull(self):
        thread = threading.Thread(target = self._store)
        thread.start()
        return None
    PULL=pull

    def _pull(self):

        uut = acq400_hapi.Acq400(self.node.data())
        while uut.statmon.get_state() != 0: continue
        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        uut.fetch_all_calibration()
        eslo = uut.cal_eslo[1:]
        eoff = uut.cal_eoff[1:]
        channel_data = uut.read_channels()

        print('Trig T0  {}'.format(str(self.wr_wrtd_t0.data())))
        print('Trig TAI {}'.format(str(self.wr_trig_tai.data())))

        for ic, ch in enumerate(self.chans):
            if ch.on:
                ch.putData(channel_data[ic])
                ch.EOFF.putData(float(eoff[ic]))
                ch.ESLO.putData(float(eslo[ic]))

                #Expression to calculate the calibrarted inputs:
                expr = "{} * {} + {}".format(ch, ch.ESLO, ch.EOFF)

                ch.CAL_INPUT.putData(MDSplus.Data.compile(expr))

    _PULL=_pull



class_ch_dict = acq400_base.create_classes(
    _ACQ2106_TR, "ACQ2106_TR",
    list(_ACQ2106_TR.tr_base_parts) + list(_ACQ2106_TR.base_parts),
    acq400_base.ACQ2106_CHANNEL_CHOICES
)

globals().update(class_ch_dict)


if __name__ == '__main__':
    acq400_base.print_generated_classes(class_ch_dict)
del(class_ch_dict)

# public classes created in this module
# ACQ2106_TR_8
# ACQ2106_TR_16
# ACQ2106_TR_24
# ACQ2106_TR_32
# ACQ2106_TR_40
# ACQ2106_TR_48
# ACQ2106_TR_64
# ACQ2106_TR_80
# ACQ2106_TR_96
# ACQ2106_TR_128
# ACQ2106_TR_160
# ACQ2106_TR_192
