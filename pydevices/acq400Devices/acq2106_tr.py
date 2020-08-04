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

import importlib

acq400_base = importlib.import_module('acq400_base')
acq400_hapi = importlib.import_module('acq400_hapi')

class _ACQ2106_TR(acq400_base._ACQ400_TR_BASE):
    """
    D-Tacq ACQ2106 transient support.
    """
    def init(self):
        super(_ACQ2106_TR, self).init()
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        #Setting WR Clock to 20MHz by first being sure that MBCLK FIN is in fact = 0
        uut.s0.SIG_CLK_MB_FIN = '0'

        #Set the Sampling rate in the ACQ:
        # MB Clock (WR Clock): 20MHz
        # mb_freq = uut.s0.SIG_CLK_MB_FREQ
        mb_freq = 20000000.00 #Hz

        self.dprint(1, "Setting sample rate to %r Hz", self.freq.data())
        clockdiv      = mb_freq/self.freq.data()

        #The following will set the sample rate, by setting the clock div.
        uut.s1.CLKDIV = "{}".format(clockdiv)

        acq_sample_freq = uut.s0.SIG_CLK_S1_FREQ
        
        self.dprint(1, "After setting the sample rate the value in the ACQ is%r Hz", acq_sample_freq)

        #The following is what the ACQ script called "/mnt/local/set_clk_WR" does to set the WR clock to 20MHz
        uut.s0.SYS_CLK_FPMUX     = 'ZCLK'
        uut.s0.SIG_ZCLK_SRC      = 'WR31M25'
        uut.s0.set_si5326_bypass = 'si5326_31M25-20M.txt'


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
