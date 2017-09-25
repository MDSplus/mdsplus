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
from ctypes import CDLL,c_int,c_double
from threading import Thread
from time import sleep

class ACQIPPSETUP(Device):
    """IPP probe & thermocoupels acquisition setup"""
    parts=[ {'path':':COMMENT', 'type':'text'}]
    parts.append({'path':'.PROBE', 'type':'structure'})
    parts.append({'path':'.PROBE:START_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.PROBE:END_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.PROBE:CLOCK_FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.PROBE:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':'.SWEEP_WAVE', 'type':'structure'})
    parts.append({'path':'.SWEEP_WAVE:DEV_TYPE', 'type':'text','value':'NI6368'})
    parts.append({'path':'.SWEEP_WAVE:BOARD_ID', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:AO_CHAN', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:MIN', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:MAX', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.SWEEP_WAVE:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':'.TC', 'type':'structure'})
    parts.append({'path':'.TC:START_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.TC:END_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.TC:CLOCK_FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.TC:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_wave_gen',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_wave_gen',head))",
        'options':('no_write_shot',)})

    parts.append({'path':'.SWEEP_WAVE:WAVE_EXPR', 'type':'numeric'})

    isRunning = False
    niInterfaceLib = None

    class AsynchWaveGen(Thread):

        def configure(self, device):
            self.device = device
            self.nid = device.getNid()
        #      set tree ipp_probes/shot=2
        #  do/method :ACQIPPSETUP start_wave_gen

        def run(self):

            if ACQIPPSETUP.niInterfaceLib is None:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot load libNiInterface.so')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                board_id = self.device.sweep_wave_board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing Board Id' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                channel = self.device.sweep_wave_ao_chan.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing output channel number 0..3' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                minValue = self.device.sweep_wave_min.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing min sweep value' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                maxValue = self.device.sweep_wave_max.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing max sweep value' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                waverate = self.device.sweep_wave_freq.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing frequency sweep value' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                trigMode = self.device.sweep_wave_trig_mode.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Missing trig mode sweep value' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            level  = ( maxValue - minValue ) /2.;
            offset = ( maxValue + minValue ) / 2.;

            print("Offset  ", offset, " Vpp level ", level * 2)

            offset = offset / 10.;
            level = level / 10.;

            print("Reference Offset  ", offset, " Vpp level ", level * 2)

            if(trigMode == 'EXTERNAL'):
                 softwareTrigger = 0;
            else:
                 softwareTrigger = 1;

            ACQIPPSETUP.niInterfaceLib.generateWaveformOnOneChannel_6368(c_int(board_id), c_int(channel), c_double(offset), c_double(level), c_int(waverate), c_int(softwareTrigger) );

            """
    	      count = 0
    	      delay = 2;
            while count < 5:
                time.sleep(delay)
                count += 1
                print " %s" % (  time.ctime(time.time()) )
            """
            return


    def restoreInfo(self):
        if ACQIPPSETUP.niInterfaceLib is None:
            ACQIPPSETUP.niInterfaceLib = CDLL("libNiInterface.so")


    def start_wave_gen(self):
        print('======= Initialize waveform generation ========')

        if ACQIPPSETUP.isRunning :
            print("Is running stop thread")
            ACQIPPSETUP.niInterfaceLib.stopWaveGeneration()

        self.worker = self.AsynchWaveGen()
        self.worker.daemon = True

        self.restoreInfo()

        #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(nSamples), (numTrigger))
        #niInterfaceLib.generateWaveformOnOneChannel_6368(uint8_t selectedCard, uint8_t channel, double offset, double level, uint32_t waverate, char softwareTrigger);
        #niInterfaceLib.generateWaveformOnOneChannel_6368(c_int(board_id), c_int(channel), c_double(offset), c_double(level), c_int(waverate), c_int(softwareTrigger) );

        ACQIPPSETUP.isRunning = True
        self.worker.configure(self);
        self.worker.start()

        print("===============================================")

        return


    def stop_wave_gen(self):

        print('========= Stop waveform generation ============')
        self.restoreInfo()
        ACQIPPSETUP.isRunning = False
        ACQIPPSETUP.niInterfaceLib.stopWaveGeneration()
        sleep(2)
        print("===============================================")

        return
