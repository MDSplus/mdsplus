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


import numpy as np
import MDSplus
import threading
import Queue
import socket
import time
import inspect


try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

INPFMT = ':INPUT_%3.3d'

class _ACQ1001_TR(MDSplus.Device):
    """
    D-Tacq ACQ1001 transient support.

    """

    base_parts=[
        # The user will need to change the hostname to the relevant hostname/IP.
        {'path':':NODE','type':'text','value':'acq1001_329', 'options':('no_write_shot',)},
        {'path':':SITE','type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':TRIG_MODE','type':'text', 'value': 'role_default', 'options':('no_write_shot',)},
        {'path':':ROLE','type':'text', 'value': 'fpmaster', 'options':('no_write_shot',)},
        {'path':':FREQ','type':'numeric', 'value': int(1e6), 'options':('no_write_shot',)},
        {'path':':INIT_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",'options':('no_write_shot',)},
        {'path':':ARM_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',51,None),Method(None,'ARM',head))",'options':('no_write_shot',)},
        {'path':':PULL_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',52,None),Method(None,'PULL',head))",'options':('no_write_shot',)},
        ]

    trig_types=[ 'hard', 'soft', 'automatic']


    def setChanScale(self,num):
        chan=self.__getattr__('INPUT_%3.3d' % num)


    def init(self):
        print('Running init')

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        trig_types=[ 'hard', 'soft', 'automatic']
        trg = self.trig_mode.data()

        if trg == 'hard':
            trg_dx = 0
        elif trg == 'automatic':
            trg_dx = 1
        elif trg == 'soft':
            trg_dx = 1

        # The default case is to use the trigger set by sync_role.
        if self.trig_mode.data() == 'role_default':
            uut.s0.sync_role = "{} {}".format(self.role.data(), self.freq.data())
        else:
            # If the user has specified a trigger.
            uut.s0.sync_role = '{} {} TRG:DX={}'.format(self.role.data(), self.freq.data(), trg_dx)

        # Now we set the trigger to be soft when desired.
        if trg == 'soft':
            uut.s0.transient = 'SOFT_TRIGGER=0'
        if trg == 'automatic':
            uut.s0.transient = 'SOFT_TRIGGER=1'

        print('Finished init')
    INIT = init


    def arm(self):
        print("Capturing now.")
        uut = acq400_hapi.Acq400(self.node.data())
        shot_controller = acq400_hapi.ShotController([uut])
        shot_controller.run_shot()
        print("Finished capture.")
    ARM=arm

    # TODO: Change to store.
    def pull(self):
        print("Starting data collection now.")

        uut = acq400_hapi.Acq400(self.node.data())
        self.chans = []
        nchans = uut.nchan()
        for ii in range(nchans):
            self.chans.append(getattr(self, 'INPUT_%3.3d'%(ii+1)))

        channel_data = uut.read_channels()
        for ic, ch in enumerate(self.chans):
            if ch.on:
                ch.putData(channel_data[ic])

    PULL=pull


def assemble(cls):
    cls.parts = list(_ACQ1001_TR.base_parts)
    for ch in range(1, cls.nchan+1):
        cls.parts.append({'path':INPFMT%(ch,), 'type':'signal','options':('no_write_model','write_once',),
                          'valueExpr':'head.setChanScale(%d)' %(ch,)})
        cls.parts.append({'path':INPFMT%(ch,)+':DECIMATE', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':COEFFICIENT','type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':OFFSET', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})


chan_combos = [8, 16, 24, 32, 40, 48, 64, 80, 96, 128, 160, 192]
class_ch_dict = {}

for channel_count in chan_combos:
    name_str = "ACQ1001_TR_" + str(channel_count)
    class_ch_dict[name_str] = type(name_str, (_ACQ1001_TR,), {"nchan": channel_count})
    assemble(class_ch_dict[name_str])

for key,val in class_ch_dict.items():
        exec(key + '=val')

