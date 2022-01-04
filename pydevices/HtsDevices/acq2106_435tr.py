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
# DITRLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import MDSplus
import importlib
import numpy

acq2106_435st = importlib.import_module('acq2106_435st')

class _ACQ2106_435TR(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """

    tr_parts = [
        # PRE and POST samples
        {
            'path':':PRESAMPLES','type':'numeric',   
            'value': int(1e5), 
            'options':('no_write_shot',)
        },
        {
            'path':':POSTSAMPLES','type':'numeric',  
            'value': int(1e5), 
            'options':('no_write_shot',)
        },
        # Trigger sources
        {
            'path':':TRIG_SRC',   
            'type':'text',      
            'value': 'STRIG', 
            'options':('no_write_shot',)
        },
        # Event sources
        {
            'path':':EVENT0_SRC', 
            'type': 'text',     
            'value': 'EXT', 
            'options':('no_write_shot',)
        },
        ]

    def init(self):

        # Here, the argument to the init of the superclass, i.e. the value True, means that the 
        # arming will be donw in this sub-class. In other words, cvalue True will not
        # start a MDSWorker thread of the super-class, with the purpose of letting the sub-class 
        # to arm the digitazer:
        # True ==> the transient recording will arm the digitazer.
        super(_ACQ2106_435TR, self).init(resampling = False, armed_by_transient = True)

        # Transient capture may be configured programmatically as follows, where
        # PRE, POST are pre-trigger, post-trigger capture lengths in samples.
        #
        # transient [PRE=N] [POST=N] [OSAM=1] [SOFT_TRIGGER=1]
        #
        # Set trigger condition.
        # With PRE=0:
        # set.site 1 trg=1,0,1 - external TRG, rising
        # set.site 1 event0=0,0,0 - no event
        #
        # With PRE > 0:
        # set.site 1 trg=1,1,1 - local (SOFT) TRG
        # set.site 1 event0=1,0,1 - external rising edge causes PRE - > POST
        uut = self.getUUT()

        uut.s0.transient = 'SOFT_TRIGGER=1'

        # If PRE samples different from zero
        uut.s0.transient = "PRE={} POST={}".format(self.presamples.data(), self.postsamples.data())

        print("{}".format(uut.s0.transient))

        # Initializing Sources to NONE:
        # D0 signal:
        uut.s0.SIG_SRC_TRG_0   = 'NONE'
        # D1 signal:
        uut.s0.SIG_SRC_TRG_1   = 'NONE'

        # Trigger sources choices:
        # d0:
        srcs_0 = ['EXT', 'HDMI', 'HOSTB', 'GPG0', 'DSP0', 'WRTT0', 'nc']
        # d1:
        srcs_1 = ['STRIG', 'HOSTA', 'HDMI_GPIO', 'GPG1', 'DSP1', 'FP_SYNC', 'WRTT1']

        if str(self.trig_src.data()) in srcs_1:
            uut.s0.SIG_SRC_TRG_1   = str(self.trig_src.data())
            # Setting the signal (dX) to use for ACQ2106 stream control
            uut.s1.TRG       = 'enable'
            uut.s1.TRG_DX    = 'd1'
            uut.s1.TRG_SENSE = 'rising'

            # EVENT0 setting in d0:
            if str(self.event0_src.data()) in srcs_0:
                uut.s0.SIG_SRC_TRG_0   = str(self.event0_src.data())
                uut.s1.EVENT0          = 'enable'
                uut.s1.EVENT0_DX       = 'd0'
                uut.s1.EVENT0_SENSE    = 'rising'
                uut.s0.SIG_EVENT_SRC_0 = 'TRG' # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
            else:
               print("EVENT0 source should be one of {}, not {}".format(srcs_0, str(self.event0_src.data())))

        elif str(self.trig_src.data()) in srcs_0:
            uut.s0.SIG_SRC_TRG_0   = str(self.trig_src.data())
            # Setting the signal (dX) to use for ACQ2106 stream control
            uut.s1.TRG       = 'enable'
            uut.s1.TRG_DX    = 'd0'
            uut.s1.TRG_SENSE = 'rising'

            # EVENT0 setting in d1:
            if str(self.event0_src.data()) in srcs_1:
                uut.s0.SIG_SRC_TRG_0   = str(self.event0_src.data())
                uut.s1.EVENT0          = 'enable'
                uut.s1.EVENT0_DX       = 'd1'
                uut.s1.EVENT0_SENSE    = 'rising'
                uut.s0.SIG_EVENT_SRC_0 = 'TRG' # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
            else:
               print("EVENT0 source should be one of {}".format(srcs_1))
        else:
            if self.debug:
                print("TRG source was set to {}".format(str(self.trig_src.data())))

        self.arm()

    INIT=init


    def arm(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())

        shot_controller = acq400_hapi.ShotController([uut])
        if self.debug:
            print("Using HAPI ShotController to run the shot.")
        shot_controller.run_shot()


    def state(self):
        import acq400_hapi

        # State of the Transient recorder NOW (when this method was run)
        uut = self.getUUT()

        _status = [int(x) for x in uut.s0.state.split(" ")]

        trstate = acq400_hapi.acq400.STATE.str(_status[0])
        
        print("TR state: {}".format(trstate))
        print("TR status: {}".format(_status))
   
    STATE=state

    def stop(self):
        uut = self.getUUT()

        print("{}".format(uut.s0.transient))
        uut.s0.set_abort=1
        
        self.running.on = False
    
    STOP = stop

    def store(self):
        uut = self.getUUT()
        
        while uut.statmon.get_state() != 0: continue

        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        # See D-Tacq 4G User Guide: For 24 bit ADC, Section 11.1, normalise the raw value first by dividing by 256 ( >> 8 ).
        channel_data = numpy.right_shift(uut.read_channels(), 8)

        for ic, ch in enumerate(self.chans):
            if ch.on:
                
                start_idx     = - self.presamples.data() + 1
                end_idx       = self.postsamples.data()

                clock_period  = 1./self.freq.data()

                # When White Rabbit is used, we can get the trigger time from it:
                # self.wr_wrtd_t0 is the reference to the node in the WRTD device. (secs)
                # self.wr_wrtd_tai  is the reference to the node in WRTD device. (TAI time)
                # mdswindow = MDSplus.Window(start_idx, end_idx, self.wr_wrtd_t0)   
                # mdswindow = MDSplus.Window(start_idx, end_idx, self.wr_trig_tai)

                mdswindow = MDSplus.Window(start_idx, end_idx, 0)
                mdsrange  = MDSplus.Range(None, None, clock_period)
                dim       = MDSplus.Dimension(mdswindow, mdsrange)

                raw_signal = MDSplus.Signal(channel_data[ic], None, dim)
                ch.RAW_INPUT.putData(raw_signal)

    STORE=store
        
    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), has_wr=True)
        return uut

def assemble(cls):
    cls.parts = list(_ACQ2106_435TR.carrier_parts + _ACQ2106_435TR.tr_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i+1,),             
                'type': 'SIGNAL',  
                'valueExpr': 
                     'ADD(MULTIPLY(head.INPUT_%3.3d.RAW_INPUT, head.INPUT_%3.3d.COEFFICIENT), head.INPUT_%3.3d.OFFSET)'
                      % (i+1,i+1,i+1),
                'options': ('no_write_model','write_once',)
            }, 

            {
                'path': ':INPUT_%3.3d:DECIMATE' % (i+1,),
                'type': 'NUMERIC',
                'valueExpr': 'head.def_dcim',
                'options': ('no_write_shot',)
            },  
                     
            {
                'path': ':INPUT_%3.3d:COEFFICIENT' % (i+1,), 
                'type': 'NUMERIC',  
                'options': ('no_write_model', 'write_once',)
            },

            {
                'path': ':INPUT_%3.3d:OFFSET' % (i+1,),
                'type': 'NUMERIC', 
                'options': ('no_write_model', 'write_once',)
            },

            {
                'path': ':INPUT_%3.3d:RAW_INPUT' % (i+1,),
                'type': 'SIGNAL',  
                'options': ('no_write_model','write_once',)
            },
        ]


class ACQ2106_435TR_1ST(_ACQ2106_435TR): 
    sites = 1

assemble(ACQ2106_435TR_1ST)

class ACQ2106_435TR_2ST(_ACQ2106_435TR):
    sites = 2

assemble(ACQ2106_435TR_2ST)

class ACQ2106_435TR_3ST(_ACQ2106_435TR):
    sites = 3

assemble(ACQ2106_435TR_3ST)

class ACQ2106_435TR_4ST(_ACQ2106_435TR):
    sites = 4

assemble(ACQ2106_435TR_4ST)

class ACQ2106_435TR_5ST(_ACQ2106_435TR):
    sites = 5

assemble(ACQ2106_435TR_5ST)

class ACQ2106_435TR_6ST(_ACQ2106_435TR):
    sites = 6

assemble(ACQ2106_435TR_6ST)

del(assemble)