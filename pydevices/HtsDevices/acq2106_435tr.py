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

acq2106_435st = importlib.import_module('acq2106_435st')

class _ACQ2106_435TR(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """

    tr_parts = [
        # PRE and POST samples
        {'path':':PRESAMPLES','type':'numeric',   'value': int(1e5), 'options':('no_write_shot',)},
        {'path':':POSTSAMPLES','type':'numeric',  'value': int(1e5), 'options':('no_write_shot',)},
        #Trigger sources
        {'path':':TRIG_SRC',   'type':'text',      'value': 'NONE', 'options':('no_write_shot',)},
        #Event sources
        {'path':':EVENT0_SRC', 'type': 'text',     'value': 'NONE', 'options':('no_write_shot',)},
        ]

    def init(self):
        import acq400_hapi

        MIN_FREQUENCY = 10000

        uut = self.getUUT()

        uut.s0.set_knob('set_abort', '1')

        if self.ext_clock.length > 0:
            raise Exception('External Clock is not supported')

        freq = int(self.freq.data())
        # D-Tacq Recommendation: the minimum sample rate is 10kHz.
        if freq < MIN_FREQUENCY:
            raise MDSplus.DevBAD_PARAMETER(
                "Sample rate should be greater or equal than 10kHz")

        mode = self.trig_mode.data()
        if mode == 'hard':
            role = 'master'
            trg = 'hard'
        elif mode == 'soft':
            role = 'master'
            trg = 'soft'
        else:
            role = mode.split(":")[0]
            trg = mode.split(":")[1]

        print("Role is {} and {} trigger".format(role, trg))

        if trg == 'hard':
            trg_dx = 'd0'
        elif trg == 'automatic':
            trg_dx = 'd1'
        elif trg == 'soft':
            trg_dx = 'd1'

        # USAGE sync_role {fpmaster|rpmaster|master|slave|solo} [CLKHZ] [FIN]
        # modifiers [CLK|TRG:SENSE=falling|rising] [CLK|TRG:DX=d0|d1]
        # modifiers [TRG=int|ext]
        # modifiers [CLKDIV=div]
        uut.s0.sync_role = '%s %s TRG:DX=%s' % (role, self.freq.data(), trg_dx)

        # Fetching all calibration information from every channel.
        uut.fetch_all_calibration()
        coeffs = uut.cal_eslo[1:]
        eoff = uut.cal_eoff[1:]

        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d' % (ii+1)))

        for ic, ch in enumerate(self.chans):
            if ch.on:
                ch.OFFSET.putData(float(eoff[ic]))
                ch.COEFFICIENT.putData(float(coeffs[ic]))

        # Hardware decimation:
        if self.debug:
            print("Hardware Filter (NACC) from tree node is {}".format(
                int(self.hw_filter.data())))

        # Hardware Filter: Accumulate/Decimate filter. Accumulate nacc_samp samples, then output one value.
        nacc_samp = int(self.hw_filter.data())
        print("Number of sites in use {}".format(self.sites))

        # Ask UUT what are the sites that are actually being populatee with a 435ELF
        self.slots = []
        for (site, module) in sorted(uut.modules.items()):
            site_number = int(site)
            if site_number == 1:
                self.slots.append(uut.s1)
            elif site_number == 2:
                self.slots.append(uut.s2)
            elif site_number == 3:
                self.slots.append(uut.s3)
            elif site_number == 4:
                self.slots.append(uut.s4)
            elif site_number == 5:
                self.slots.append(uut.s5)
            elif site_number == 6:
                self.slots.append(uut.s6)

        for card in range(self.sites):
            if 1 <= nacc_samp <= 32:
                self.slots[card].nacc = ('%d' % nacc_samp).strip()
            else:
                print(
                    "WARNING: Hardware Filter samples must be in the range [0,32]. 0 => Disabled == 1")
                self.slots[card].nacc = '1'


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

        uut.s0.transient = 'SOFT_TRIGGER=1'

        # If PRE samples different from zero
        uut.s0.transient = "PRE={} POST={}".format(self.presamples.data(), self.postsamples.data())

        print("{}".format(uut.s0.transient))

        # Initializing Sources to NONE:
        # D0 signal:
        uut.s0.SIG_SRC_TRG_0   = 'NONE'
        # D1 signal:
        uut.s0.SIG_SRC_TRG_1   = 'NONE'

        #Trigger sources choices:
        # d0:
        srcs_0 = ['EXT', 'HDMI', 'HOSTB', 'GPG0', 'DSP0', 'WRTT0', 'nc']
        # d1:
        srcs_1 = ['STRIG', 'HOSTA', 'HDMI_GPIO', 'GPG1', 'DSP1', 'FP_SYNC', 'WRTT1']

        # event_src = ['TRG', 'GPG', 'FP_GPIO', 'MOD']

        if str(self.trig_src.data()) in srcs_1:
            uut.s0.SIG_SRC_TRG_1   = str(self.trig_src.data())
            #Setting the signal (dX) to use for ACQ2106 stream control
            uut.s1.TRG       = 'enable'
            uut.s1.TRG_DX    = 'd1'
            uut.s1.TRG_SENSE = 'rising'

            #EVENT0 setting in d0:
            if str(self.event0_src.data()) in srcs_0:
                uut.s0.SIG_SRC_TRG_0   = str(self.event0_src.data())
                uut.s1.EVENT0       = 'enable'
                uut.s1.EVENT0_DX    = 'd0'
                uut.s1.EVENT0_SENSE = 'rising'
                uut.s0.SIG_EVENT_SRC_0 = 'TRG' # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
            else:
               print("EVENT0 source should be one of {}, not {}".format(srcs_0, str(self.event0_src.data())))

        elif str(self.trig_src.data()) in srcs_0:
            uut.s0.SIG_SRC_TRG_0   = str(self.trig_src.data())
            #Setting the signal (dX) to use for ACQ2106 stream control
            uut.s1.TRG       = 'enable'
            uut.s1.TRG_DX    = 'd0'
            uut.s1.TRG_SENSE = 'rising'

            #EVENT0 setting in d1:
            if str(self.event0_src.data()) in srcs_1:
                uut.s0.SIG_SRC_TRG_0   = str(self.event0_src.data())
                uut.s1.EVENT0       = 'enable'
                uut.s1.EVENT0_DX    = 'd1'
                uut.s1.EVENT0_SENSE = 'rising'
                uut.s0.SIG_EVENT_SRC_0 = 'TRG' # In the EVENT bus, the source needs to be TRG to make the transition PRE->POST
            else:
               print("EVENT0 source should be one of {}".format(srcs_1))
        else:
            print("TRG source was set to {}".format(str(self.trig_src.data())))

    INIT=init


    def arm(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())

        shot_controller = acq400_hapi.ShotController([uut])
        print("Using HAPI ShotController to run the shot.")
        shot_controller.run_shot()

    ARM=arm


    def state(self):
        import acq400_hapi

        # State of the Transient recorder NOW (when this method was run)
        uut = self.getUUT400()

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

        channel_data = uut.read_channels()

        # print('Trig T0  {}'.format(str(self.wr_wrtd_t0.data())))
        # print('Trig TAI {}'.format(str(self.wr_trig_tai.data())))

        for ic, ch in enumerate(self.chans):
            if ch.on:
                
                start_idx     = - self.presamples.data() + 1
                end_idx       = self.postsamples.data()
                total_samples = uut.elapsed_samples()

                clock_period  = 1./self.freq.data()

                # self.wr_wrtd_t0 is the reference to the node in the WRTD device. (secs)
                # self.wr_wrtd_tai  is the reference to the node in WRTD device. (TAI time)

                mdswindow = MDSplus.Window(start_idx, end_idx, self.wr_wrtd_t0)   
                # mdswindow = MDSplus.Window(start_idx, end_idx, self.wr_trig_tai)
                mdsrange  = MDSplus.Range(None, None, clock_period)
                dim       = MDSplus.Dimension(mdswindow, mdsrange)

                # Calibrarted signal
                signal = MDSplus.Signal(channel_data[ic], None, dim)

                ch.putData(signal)

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

def assemble(cls):
    cls.parts = list(_ACQ2106_435TR.carrier_parts + _ACQ2106_435TR.tr_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {'path':':INPUT_%3.3d'%(i+1,),             'type':'SIGNAL',  'valueExpr':'head.setChanScale(%d)' %(i+1,),'options':('no_write_model','write_once',)}, 
            {'path':':INPUT_%3.3d:DECIMATE'%(i+1,),    'type':'NUMERIC', 'valueExpr':'head.def_dcim',                'options':('no_write_shot',)},           
            {'path':':INPUT_%3.3d:COEFFICIENT'%(i+1,), 'type':'NUMERIC',                                             'options':('no_write_model', 'write_once',)},
            {'path':':INPUT_%3.3d:OFFSET'%(i+1,),      'type':'NUMERIC',                                             'options':('no_write_model', 'write_once',)},
        ]


class ACQ2106_435TR_1ST(_ACQ2106_435TR): 
    sites=1

assemble(ACQ2106_435TR_1ST)

class ACQ2106_435TR_2ST(_ACQ2106_435TR):
    sites=2

assemble(ACQ2106_435TR_2ST)

class ACQ2106_435TR_3ST(_ACQ2106_435TR):
    sites=3

assemble(ACQ2106_435TR_3ST)

class ACQ2106_435TR_4ST(_ACQ2106_435TR):
    sites=4

assemble(ACQ2106_435TR_4ST)

class ACQ2106_435TR_5ST(_ACQ2106_435TR):
    sites=5

assemble(ACQ2106_435TR_5ST)

class ACQ2106_435TR_6ST(_ACQ2106_435TR):
    sites=6

assemble(ACQ2106_435TR_6ST)

del(assemble)