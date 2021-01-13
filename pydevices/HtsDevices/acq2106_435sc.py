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

import MDSplus
import importlib
import os
import numbers

acq2106_435st = importlib.import_module('acq2106_435st')
acq400_hapi = importlib.import_module('acq400_hapi')  

class _ACQ2106_435SC(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """

    sc_parts = [
        {'path':':DEF_GAIN1',  'type':'numeric', 'value': '1', 'options':('no_write_shot',)},
        {'path':':DEF_GAIN2',  'type':'numeric', 'value': '1', 'options':('no_write_shot',)},
        {'path':':DEF_OFFSET', 'type':'numeric', 'value': '0', 'options':('no_write_shot',)},
    ]

    def init(self):
        super(_ACQ2106_435SC, self).init()
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        chans_sc  = []
        nchannels = uut.nchan()
        for ii in range(nchannels):
            chans_sc.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        for site in range(1,6,2):
            if site == 1:
                for ic, ch in enumerate(chans_sc):
                    if ic < 33:
                        uut_s1_sc32_offset = uut.s1.__getattr__('SC32_OFFSET_%2.2d' %(ic+1))
                        uut_s1_sc32_g1     = uut.s1.__getattr__('SC32_G1_%2.2d' %(ic+1))
                        uut_s1_sc32_g2     = uut.s1.__getattr__('SC32_G2_%2.2d' %(ic+1))
                        if ch.on:
                            uut_s1_sc32_offset = self.__getattr__('INPUT_%3.3d:SC_GAIN1' %(ic+1)).data()
                            uut_s1_sc32_g1     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' %(ic+1)).data()
                            uut_s1_sc32_g2     = self.__getattr__('INPUT_%3.3d:SC_OFFSET'%(ic+1)).data()
                if isinstance(self.def_offset.data() , numbers.Number) and isinstance(self.def_gain1.data() , numbers.Number) and isinstance(self.def_gain2.data() , numbers.Number):
                   # Global controls for GAINS and OFFSETS
                    uut.s1.SC32_OFFSET_ALL = self.def_offset.data()
                    print("OFFSET ALL {}".format(self.def_offset.data()))

                    uut.s1.SC32_G1_ALL     = self.def_gain1.data()
                    print("GAIN 1 ALL {}".format(self.def_gain1.data()))

                    uut.s1.SC32_G2_ALL     = self.def_gain2.data()
                    print("GAIN 2 ALL {}".format(self.def_gain2.data()))

                uut.s1.SC32_GAIN_COMMIT = 1
                print("GAIN Committed for site {}".format(site))

            elif site == 3:
                for ic, ch in enumerate(chans_sc):
                    if ic < 33:
                        uut_s1_sc32_offset = uut.s3.__getattr__('SC32_OFFSET_%2.2d' %(ic+1))
                        uut_s1_sc32_g1     = uut.s3.__getattr__('SC32_G1_%2.2d' %(ic+1))
                        uut_s1_sc32_g2     = uut.s3.__getattr__('SC32_G2_%2.2d' %(ic+1))
                        if ch.on:
                            uut_s1_sc32_offset = self.__getattr__('INPUT_%3.3d:SC_GAIN1' %(ic+1)).data()
                            uut_s1_sc32_g1     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' %(ic+1)).data()
                            uut_s1_sc32_g2     = self.__getattr__('INPUT_%3.3d:SC_OFFSET'%(ic+1)).data()
                if isinstance(self.def_offset.data() , numbers.Number) and isinstance(self.def_gain1.data() , numbers.Number) and isinstance(self.def_gain2.data() , numbers.Number):
                   # Global controls for GAINS and OFFSETS
                    uut.s3.SC32_OFFSET_ALL = self.def_offset.data()
                    print("OFFSET ALL {}".format(self.def_offset.data()))

                    uut.s3.SC32_G1_ALL     = self.def_gain1.data()
                    print("GAIN 1 ALL {}".format(self.def_gain1.data()))

                    uut.s3.SC32_G2_ALL     = self.def_gain2.data()
                    print("GAIN 2 ALL {}".format(self.def_gain2.data()))

                uut.s3.SC32_GAIN_COMMIT = 1
                print("GAIN Committed for site {}".format(site))

            elif site == 5:
                for ic, ch in enumerate(chans_sc):
                    if ic < 33:
                        uut_s5_sc32_offset = uut.s5.__getattr__('SC32_OFFSET_%2.2d' %(ic+1))
                        uut_s5_sc32_g1     = uut.s5.__getattr__('SC32_G1_%2.2d' %(ic+1))
                        uut_s5_sc32_g2     = uut.s5.__getattr__('SC32_G2_%2.2d' %(ic+1))
                        if ch.on:
                            uut_s5_sc32_offset = self.__getattr__('INPUT_%3.3d:SC_GAIN1' %(ic+1)).data()
                            uut_s5_sc32_g1     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' %(ic+1)).data()
                            uut_s5_sc32_g2     = self.__getattr__('INPUT_%3.3d:SC_OFFSET'%(ic+1)).data()
                if isinstance(self.def_offset.data() , numbers.Number) and isinstance(self.def_gain1.data() , numbers.Number) and isinstance(self.def_gain2.data() , numbers.Number):
                   # Global controls for GAINS and OFFSETS
                    uut.s5.SC32_OFFSET_ALL = self.def_offset.data()
                    print("OFFSET ALL {}".format(self.def_offset.data()))

                    uut.s5.SC32_G1_ALL     = self.def_gain1.data()
                    print("GAIN 1 ALL {}".format(self.def_gain1.data()))

                    uut.s5.SC32_G2_ALL     = self.def_gain2.data()
                    print("GAIN 2 ALL {}".format(self.def_gain2.data()))

                uut.s5.SC32_GAIN_COMMIT = 1
                print("GAIN Committed for site {}".format(site))


    def store(self):
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        chans_sc  = []
        nchannels = uut.nchan()
        for ii in range(nchannels):
            chans_sc.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        for ic, ch in enumerate(chans_sc):
            chan_unsc = self.__getattr__('INPUT_%3.3d:NON_SC' %(ic+1))
            if ch.on:
                chan_unsc.record = MDSplus.BUILD_SIGNAL(ch.data() * (1.0/ch.SC_GAIN1 * ch.SC_GAIN2) - ch.SC_OFFSET, MDSplus.RAW_OF(ch), MDSplus.DIM_OF(ch))

    STORE=store


    def unConditioning(self,num):
        chan     = self.__getattr__('INPUT_%3.3d' % num)
        chan_raw = self.__getattr__('INPUT_%3.3d:NON_SC' % num)
        expr = MDSplus.SUBTRACT(MDSplus.DIVIDE(chan, MDSplus.MULTIPLY(chan.SC_GAIN1, chan.SC_GAIN2)), chan.SC_OFFSET)

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
            #{'path':':INPUT_%3.3d:NON_SC'%(i+1,),          'type':'SIGNAL',  'valueExpr':'SUBTRACT(DIVIDE(INPUT_%3.3d, MULTIPLY(INPUT_%3.3d_SC_GAIN1, INPUT_%3.3d_SC_GAIN2)), INPUT_%3.3d_SC_OFFSET)' %(i+1,i+1,i+1,i+1,), 'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:NON_SC'%(i+1,),      'type':'SIGNAL',                                              'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:LOW_RES'%(i+1,),     'type':'SIGNAL',                                              'options':('no_write_model','write_once',)},
        ]

class ACQ2106_435SC_1ST(_ACQ2106_435SC): sites=1
assemble(ACQ2106_435SC_1ST)
class ACQ2106_435SC_2ST(_ACQ2106_435SC): sites=2
assemble(ACQ2106_435SC_2ST)
class ACQ2106_435SC_3ST(_ACQ2106_435SC): sites=3
assemble(ACQ2106_435SC_3ST)

del(assemble)