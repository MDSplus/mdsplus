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

        for site in range(1,6,2):
            slot = self.getSlot(site)
            if self.is_global.data() == 1:
                # Global controls for GAINS and OFFSETS
                slot.SC32_OFFSET_ALL = self.def_offset.data()
                print("Site {} OFFSET ALL {}".format(site, self.def_offset.data()))

                slot.SC32_G1_ALL     = self.def_gain1.data()
                print("Site {} GAIN 1 ALL {}".format(site, self.def_gain1.data()))

                slot.SC32_G2_ALL     = self.def_gain2.data()
                print("Site {} GAIN 2 ALL {}".format(site, self.def_gain2.data()))
            else:
                self.setGainsOffsets(site)

            slot.SC32_GAIN_COMMIT = 1
            print("GAINs Committed for site {}".format(site))

        super(_ACQ2106_435SC, self).init()

        # For testing purpose only. Set trigger source to external hard trigger:
        uut.s0.SIG_SRC_TRG_0 = 'EXT'

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
    
    def getSlot(self, site_number):
        uut = self.getUUT()

        try:
            if site_number   == 0: 
                slot = uut.s0
            elif site_number == 1:
                slot = uut.s1
            elif site_number == 2:
                slot = uut.s2
            elif site_number == 3:
                slot = uut.s3
            elif site_number == 4:
                slot = uut.s4
            elif site_number == 5:
                slot = uut.s5
            elif site_number == 6:
                slot = uut.s6
        except:
            pass
        
        return slot
    
    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), monitor=False, has_wr=True)
        return uut

    def setGainsOffsets(self, site):
        uut = self.getUUT()
        for ic in range(1,32):
            if site == 1:
                exec("uut.s1.SC32_OFFSET_%2.2d = self.__getattr__('INPUT_%3.3d:SC_OFFSET').data()"%(ic, ic))
                exec("uut.s1.SC32_G1_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN1' ).data()"%(ic, ic))
                exec("uut.s1.SC32_G2_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' ).data()"%(ic, ic))
            elif site == 3:
                exec("uut.s3.SC32_OFFSET_%2.2d = self.__getattr__('INPUT_%3.3d:SC_OFFSET').data()"%(ic, ic+32))
                exec("uut.s3.SC32_G1_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN1' ).data()"%(ic, ic+32))
                exec("uut.s3.SC32_G2_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' ).data()"%(ic, ic+32))
            elif site == 5:
                exec("uut.s5.SC32_OFFSET_%2.2d = self.__getattr__('INPUT_%3.3d:SC_OFFSET').data()"%(ic, ic+64))
                exec("uut.s5.SC32_G1_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN1' ).data()"%(ic, ic+64))
                exec("uut.s5.SC32_G2_%2.2d     = self.__getattr__('INPUT_%3.3d:SC_GAIN2' ).data()"%(ic, ic+64))  

    def unConditioning(self,num):
        chan     = self.__getattr__('INPUT_%3.3d' % num)
        chan_nonsc = self.__getattr__('INPUT_%3.3d:NC_INPUT' % num)
        expr = MDSplus.SUBTRACT(MDSplus.DIVIDE(chan, MDSplus.MULTIPLY(chan.SC_GAIN1, chan.SC_GAIN2)), chan.SC_OFFSET)

    def setSmooth(self,num):
        chan = self.__getattr__('INPUT_%3.3d' % num)
        chan_nonsc =self.__getattr__('INPUT_%3.3d:LR_INPUT' % num)
        chan_nonsc.setSegmentScale(MDSplus.SMOOTH(MDSplus.dVALUE(),100))

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
            {'path':':INPUT_%3.3d:NC_INPUT'%(i+1,),    'type':'SIGNAL',                                            'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:LR_INPUT'%(i+1,),    'type':'SIGNAL', 'valueExpr':'head.setSmooth(%d)' %(i+1,)},
        ]

class ACQ2106_435SC_1ST(_ACQ2106_435SC): sites=1
assemble(ACQ2106_435SC_1ST)
class ACQ2106_435SC_2ST(_ACQ2106_435SC): sites=2
assemble(ACQ2106_435SC_2ST)
class ACQ2106_435SC_3ST(_ACQ2106_435SC): sites=3
assemble(ACQ2106_435SC_3ST)

del(assemble)