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

acq2106_435st = importlib.import_module('acq2106_435st')
acq400_hapi = importlib.import_module('acq400_hapi')  

class _ACQ2106_435SC(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """

    sc_parts = [
        {'path':':DEF_GAIN',  'type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':DEF_OFFSET','type':'numeric', 'value': 0, 'options':('no_write_shot',)},
    ]

    def init(self):
        super(_ACQ2106_435SC, self).init()
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        uut.s1.sc32_offset_all = self.def_offset.data()
        print("GAIN ALL {}".format(self.def_offset.data()))

        uut.s1.sc32_g1_all     = self.def_gain.data()
        print("OFFSET_ALL {}".format(self.def_gain.data()))

        print("Commiting gains")
        uut.s1.sc32_gain_commit
        print("Finish commiting")

    def setCalibration(self,num):
        chan=self.__getattr__('INPUT_%3.3d:RAW' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT,MDSplus.dVALUE()),chan.OFFSET))

    def noConditioning(self,num):
        chan    = self.__getattr__('INPUT_%3.3d:RAW' % num)
        chan_sc = self.__getattr__('INPUT_%3.3d' % num) 
        chan_sc.setSegmentScale(MDSplus.SUBTRACT(MDSplus.DIVIDE(chan, chan_sc.GAIN_SC), chan_sc.OFFSET_SC))



def assemble(cls):
    cls.parts = list(_ACQ2106_435SC.carrier_parts + _ACQ2106_435SC.sc_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {'path':':INPUT_%3.3d'%(i+1,),            'type':'SIGNAL', 'valueExpr':'head.noConditioning(%d)' %(i+1,),'options':('no_write_model','write_once',)}, 
            {'path':':INPUT_%3.3d:GAIN_SC'%(i+1,),    'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:OFFSET_SC'%(i+1,),  'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},  

            {'path':':INPUT_%3.3d:RAW'%(i+1,), 'type':'SIGNAL',  'valueExpr':'head.setCalibration(%d)' %(i+1,), 'options':('no_write_model','write_once',)},
            {'path':':INPUT_%3.3d:RAW:DECIMATE'%(i+1,),    'type':'NUMERIC', 'valueExpr':'head.def_dcim',               'options':('no_write_shot',)},           
            {'path':':INPUT_%3.3d:RAW:COEFFICIENT'%(i+1,), 'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:RAW:OFFSET'%(i+1,),      'type':'NUMERIC',                                            'options':('no_write_model', 'write_once',)},
      
        ]

class ACQ2106_435SC_1ST(_ACQ2106_435SC): sites=1
assemble(ACQ2106_435SC_1ST)
class ACQ2106_435SC_2ST(_ACQ2106_435SC): sites=2
assemble(ACQ2106_435SC_2ST)
class ACQ2106_435SC_3ST(_ACQ2106_435SC): sites=3
assemble(ACQ2106_435SC_3ST)

del(assemble)