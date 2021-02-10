#!/usr/bin/env python
#
# Copyright (c) 2021, Massachusetts Institute of Technology All rights reserved.
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
import importlib

acq2106_435st = importlib.import_module('acq2106_435st')

class _ACQ2106_435SC(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """
    
    sc_parts = [
        # IS_GLOBAL controls if the GAINS and OFFSETS are set globally or per channel
        {'path':':IS_GLOBAL',  'type':'numeric', 'value': 0, 'options':('no_write_shot',)},
        {'path':':DEF_GAIN1',  'type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':DEF_GAIN2',  'type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':DEF_OFFSET', 'type':'numeric', 'value': 0, 'options':('no_write_shot',)},
    ]

    def init(self):
        uut = self.getUUT()
        slots = super(_ACQ2106_435SC, self).getSlots()

        for card in range(self.sites):

            if self.is_global.data() == 1:
                # Global controls for GAINS and OFFSETS
                slots[card].SC32_OFFSET_ALL = self.def_offset.data()
                print("Site {} OFFSET ALL {}".format(card, self.def_offset.data()))

                slots[card].SC32_G1_ALL     = self.def_gain1.data()
                print("Site {} GAIN 1 ALL {}".format(card, self.def_gain1.data()))

                slots[card].SC32_G2_ALL     = self.def_gain2.data()
                print("Site {} GAIN 2 ALL {}".format(card, self.def_gain2.data()))
            else:
                self.setGainsOffsets(card)

            slots[card].SC32_GAIN_COMMIT = 1
            print("GAINs Committed for site {}".format(card))

        # TODO: Choose between possible SR
        # For testing purpose only. Set CLKDIV knowing that:
        # a special config in /mnt/local/rc.user gives a SR=40KHz at bootime:
        # WR additions for WRCLK 20M
        # cp /mnt/local/si5326_31M25-20M48.txt /etc/si5326.d/
        # /usr/local/CARE/WR/set_clk_WR 20M48
        # 20.48MHz / 512 => 40.0kHz
        # => uut.s1.CLKDIV = '1'
        # To get a SR=40KHz, then:
        #uut.s1.CLKDIV = '1'
        # To get a SR=20KHz, then:
        #uut.s1.CLKDIV = '2'
        
        super(_ACQ2106_435SC, self).init()

        # For testing purpose only. Set trigger source to external hard trigger:
        uut.s0.SIG_SRC_TRG_0 = 'EXT'
    INIT=init

    def store(self):
        uut = self.getUUT()

        chans_sc  = []
        nchannels = uut.nchan()
        for ii in range(nchannels):
            chans_sc.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        for ic, ch in enumerate(chans_sc):
            chan_unsc = self.__getattr__('INPUT_%3.3d:NC_INPUT' %(ic+1))
            if ch.on:
                chan_unsc.record = MDSplus.BUILD_SIGNAL(ch.data() * (1.0/(ch.SC_GAIN1 * ch.SC_GAIN2)) - ch.SC_OFFSET, MDSplus.RAW_OF(ch), MDSplus.DIM_OF(ch))
    STORE=store
    
    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), monitor=False, has_wr=True)
        return uut

    def setGainsOffsets(self, site):
        uut = self.getUUT()
        for ic in range(1,32+1):
            if site == 1:
                setattr(uut.s1, 'SC32_OFFSET_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_OFFSET' % (ic,)).data())
                setattr(uut.s1, 'SC32_G1_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN1' % (ic,)).data())
                setattr(uut.s1, 'SC32_G2_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN2' % (ic,)).data())
            elif site == 3:
                setattr(uut.s3, 'SC32_OFFSET_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_OFFSET' % (ic+32,)).data())
                setattr(uut.s3, 'SC32_G1_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN1' % (ic+32,)).data())
                setattr(uut.s3, 'SC32_G2_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN2' % (ic+32,)).data())
            elif site == 5:
                setattr(uut.s5, 'SC32_OFFSET_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_OFFSET' % (ic+64,)).data())
                setattr(uut.s5, 'SC32_G1_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN1' % (ic+64,)).data())
                setattr(uut.s5, 'SC32_G2_%2.2d' % (ic,), getattr(self, 'INPUT_%3.3d:SC_GAIN2' % (ic+64,)).data())

    # TODO: a function to smooth/filter data while being adquired
    # def setSmooth(self,num):
    #     chan = self.__getattr__('INPUT_%3.3d' % num)
    #     chan_nonsc =self.__getattr__('INPUT_%3.3d:LR_INPUT' % num)
    #     chan_nonsc.setSegmentScale(MDSplus.SMOOTH(MDSplus.dVALUE(),100))

def assemble(cls):
    cls.parts = list(_ACQ2106_435SC.carrier_parts + _ACQ2106_435SC.sc_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {'path':':INPUT_%3.3d'%(i+1,),             'type':'SIGNAL',  'valueExpr':'head.setChanScale(%d)' %(i+1,),'options':('no_write_model','write_once',)}, 
            {'path':':INPUT_%3.3d:DECIMATE'%(i+1,),    'type':'NUMERIC', 'valueExpr':'head.def_dcim',                'options':('no_write_shot',)},           
            {'path':':INPUT_%3.3d:COEFFICIENT'%(i+1,), 'type':'NUMERIC',                                             'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:OFFSET'%(i+1,),      'type':'NUMERIC',                                             'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:SC_GAIN1'%(i+1,),    'type':'NUMERIC', 'valueExpr':'head.def_gain1',               'options':('no_write_shot',)},
            {'path':':INPUT_%3.3d:SC_GAIN2'%(i+1,),    'type':'NUMERIC', 'valueExpr':'head.def_gain2',               'options':('no_write_shot',)},
            {'path':':INPUT_%3.3d:SC_OFFSET'%(i+1,),   'type':'NUMERIC', 'valueExpr':'head.def_offset',              'options':('no_write_shot',)},   
            {'path':':INPUT_%3.3d:NC_INPUT'%(i+1,),    'type':'SIGNAL',                                              'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:LR_INPUT'%(i+1,),    'type':'SIGNAL'},
        ]

class ACQ2106_435SC_1ST(_ACQ2106_435SC): sites=1
assemble(ACQ2106_435SC_1ST)
class ACQ2106_435SC_2ST(_ACQ2106_435SC): sites=2
assemble(ACQ2106_435SC_2ST)
class ACQ2106_435SC_3ST(_ACQ2106_435SC): sites=3
assemble(ACQ2106_435SC_3ST)

del(assemble)
