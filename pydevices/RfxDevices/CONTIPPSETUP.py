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
#

from MDSplus import mdsExceptions, Device, Data
from threading import Thread
from time import sleep
from ctypes import CDLL, c_uint, c_int, c_char_p, c_double

class CONTIPPSETUP(Device):
    """Probe temperature control setup"""

    parts=[ {'path':':COMMENT', 'type':'text'},
    {'path':':BOARD_ID', 'type':'numeric', 'value':0},
    {'path':':AI_CHAN_LIST', 'type':'numeric'},
    {'path':':AI_FDCH_IDX', 'type':'numeric'},
    {'path':':CLOCK_FREQ', 'type':'numeric', 'value':10},
    {'path':':AOCH_ID', 'type':'numeric', 'value':0},
    {'path':':DOCH_ID', 'type':'numeric', 'value':0},
    {'path':':TEMP_REF', 'type':'numeric', 'value':50}]

    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start',head))",
        'options':('no_write_shot',)})

    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop',head))",
        'options':('no_write_shot',)})

    parts.append({'path':':WAIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'exit',head))",
        'options':('no_write_shot',)})

    niInterfaceLib = None
    threadActive = False

    class AsynchWaveGen(Thread):
        def configure(self, device, board_id, ai_chan_list, ai_fdch_idx, clock_freq, aoch_id, doch_id, temp_ref  ):
            self.device = device
            self.board_id = board_id
            self.ai_chan_list = ai_chan_list
            self.ai_fdch_idx = ai_fdch_idx
            self.clock_freq = clock_freq
            self.aoch_id = aoch_id
            self.doch_id = doch_id
            self.temp_ref = temp_ref

        #  set tree ipp_tc_trend /shot=-1
        #  do/method :CONTR_SETUP start

        def run(self):
            ai_num_chan = len(self.ai_chan_list)
            ai_chan_list_c = (c_uint * len(self.ai_chan_list) )(*self.ai_chan_list)
            CONTIPPSETUP.niInterfaceLib.temperatureProbeControl(c_uint(self.board_id), ai_chan_list_c, c_int(ai_num_chan), c_int(self.ai_fdch_idx), c_double(self.clock_freq), c_int(self.aoch_id), c_int(self.doch_id), c_double(self.temp_ref) );
            print("Thread STOP")
            CONTIPPSETUP.threadActive = False
            return


    def restoreInfo(self):
        if CONTIPPSETUP.niInterfaceLib is None:
            CONTIPPSETUP.niInterfaceLib = CDLL("libNiInterface.so")

    def start(self):
        print('OK Init')
        self.restoreInfo()
        if CONTIPPSETUP.niInterfaceLib == 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load libNiInterface.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if CONTIPPSETUP.threadActive :
            CONTIPPSETUP.niInterfaceLib.temperatureCtrlCommand(c_char_p("start"))
        return

        self.worker = self.AsynchWaveGen()
        self.worker.daemon = True

        try:
            board_id = self.board_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            ai_chan_list = self.ai_chan_list.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing analog input channels list' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            ai_fdch_idx = self.ai_fdch_idx.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing feddbach channel reference index in the channel list' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            clock_freq = self.clock_freq.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing control loop frequency value' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            aoch_id = self.aoch_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing analog output channel. Refereence signal to power supply' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            doch_id = self.doch_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing digital output channel. Digital signal to heating cable rele\' ' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            temp_ref = self.temp_ref.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing digital temperature set point' )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        print("Start new thread")
        print("ai_chan_list ",ai_chan_list)
        print("ai_fdch_idx ",ai_fdch_idx)
        print("clock_freq ", clock_freq)
        print("aoch_id ",aoch_id)
        print("doch_id ", doch_id)
        print("temp_ref",temp_ref)
        self.worker.configure(self, board_id, ai_chan_list, ai_fdch_idx, clock_freq, aoch_id, doch_id, temp_ref);
        self.worker.start()
        print("End Initialization")
        CONTIPPSETUP.threadActive = True
        return


    def exit(self):
        print("End Initialization")
        self.restoreInfo()
        if CONTIPPSETUP.niInterfaceLib == 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load libNiInterface.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        CONTIPPSETUP.threadActive = False
        CONTIPPSETUP.niInterfaceLib.temperatureCtrlCommand(c_char_p("exit"))
        sleep(2)
        return


    def stop(self):
        self.restoreInfo()
        if CONTIPPSETUP.niInterfaceLib == 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load libNiInterface.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        CONTIPPSETUP.niInterfaceLib.temperatureCtrlCommand(c_char_p("stop"))
        sleep(2)
        return
