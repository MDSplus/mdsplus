#
# Copyright (c) 2020, Massachusetts Institute of Technology All rights reserved.
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
import MDSplus

class ACQ2106_WRTD(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    White Rabbitt Trigger Distribution Device

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
		MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug

    """

    parts=[
        {'path':':NODE',        'type':'text',    'options':('no_write_shot',)},
        {'path':':HOSTNAME',    'type':'text',    'options':('no_write_shot',)},
        {'path':':COMMENT',     'type':'text',    'options':('no_write_shot',)},
        {'path':':TRIG_MSG',    'type':'text',    'options':('write_shot',)},
        {'path':':TRIG_SRC',    'type':'text',    'value': 'FPTRG', 'options':('write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric', 'value': 0.,       'options':('write_shot',)},
        {'path':':T0',          'type':'numeric', 'value': 0.,       'options':('write_shot',)},
        {'path':':WR_INIT',     'type':'text',   'options':('write_shot',)},
            # ns per tick. (tick size in ns). uut.s11.WRTD_TICKNS: For SR=20MHz. It's the Si5326 tick at 20MHz ..
            {'path':':WR_INIT:WRTD_TICKNS', 'type':'numeric', 'value': 50, 'options':('write_shot',)}, 
            # 50msec - our "safe time for broadcast". From uut.s11.WRTD_DELTA_NS
            {'path':':WR_INIT:WRTD_DNS',    'type':'numeric', 'value': 50000000, 'options':('write_shot',)}, 
            # uut.s11.WRTD_VERBOSE: use for debugging - eg logread -f or nc localhost 4280
            {'path':':WR_INIT:WRTD_VBOSE',  'type':'numeric', 'value': 2, 'options':('write_shot',)},        
            # uut.s11.WRTD_RX_MATCHES: match any of these triggers to initiate WRTT0
            {'path':':WR_INIT:WRTD_RX_M',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            # uut.s11.WRTD_RX_MATCHES1: match any of these triggers to initiate WRTT1
            {'path':':WR_INIT:WRTD_RX_M1',  'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            # From uut.s11.WRTD_DELAY01: WRTD_RX_DOUBLETAP: match any of these triggers to initiate a
            # “Double Tap, which is:
            # 1. WRTT0
            # 2. Delay WRTD_DELAY01 nsec.
            # 3. WRTT1
            {'path':':WR_INIT:WRTD_RX_DTP', 'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DELAY',  'type':'numeric', 'value': 5000000, 'options':('write_shot',)},   
            {'path':':WR_INIT:WRTD_ID',     'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_TX',     'type':'numeric', 'value': 0, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX',     'type':'numeric', 'value': 0, 'options':('write_shot',)},

        {'path':':RUNNING',     'type':'numeric', 'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',    'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options':('no_write_shot',)},
        ]

    def init(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), has_wr=True)

        # WR TRIGGER SOURCE:
        print("WR TRG Source {}".format(str(self.trig_src.data())))
        uut.s0.WR_TRG_DX = str(self.trig_src.data())

        # Global WR settings:
        # Set WRTD_ID: message to be transmitted from this device if FTTRG or HDMI triggered.
        print("WRTD_ID {}".format(str(self.wr_init_wrtd_id.data())))
        uut.s11.WRTD_ID = str(self.wr_init_wrtd_id.data())

        # Sets WR "safe time for broadcasts" the message, i.e. WRTT_TAI = TAI_TIME_NOW + WRTD_DELTA_NS
        uut.s11.WRTD_DELTA_NS = self.wr_init_wrtd_dns.data()

        # Receiver:
        # Turn on RX
        uut.s11.WRTD_RX = int(self.wr_init_wrtd_rx.data())
        # Define RX matches
        matches  = str(self.wr_init_wrtd_rx_m.data())
        matches1 = str(self.wr_init_wrtd_rx_m1.data())
        print("Messages are {} and {}".format(matches, matches1))

        uut.s11.WRTD_RX_MATCHES  = matches
        uut.s11.WRTD_RX_MATCHES1 = matches1
        
        #Commit the changes for WRTD RX
        uut.s11.wrtd_commit_rx = 1

        # Transmiter:
        # Turn on TX
        uut.s11.WRTD_TX = int(self.wr_init_wrtd_tx.data())
        #Commit the changes for WRTD TX
        uut.s11.wrtd_commit_tx = 1

    INIT=init

    def trig(self, msg=''):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), has_wr=True)

        message = str(msg)
        
        self.TRIG_MSG.record = message

        if message in uut.s11.WRTD_RX_MATCHES:
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_0 = 'TRG'
        elif message in uut.s11.WRTD_RX_MATCHES1:
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_1 = 'TRG'      
        else:
            print('Message does not match either of the WRTTs available')
            self.running.on = False

        if not message.strip():
            # Set trigger input:
            print('Trigger has no message. Setting external trigger source. Waiting for trigger...')
            uut.s0.WR_TRG_DX = str(self.trig_src.data())
        else:
            # send immediate WRTD message
            # The timestamp in the packet is:
            # WRTT_TAI = TAI_TIME_NOW + WRTD_DELTA_NS
            print("Message to be sent is {}".format(message))
            uut.s11.wrtd_txi = message


        # WR TAI Trigger time:
        # TODO: convert wr_tai_trg (or wr_tai_stamp) to tai time in seconds
        # self.trig_time.putData(MDSplus.Int64(uut.s0.wr_tai_trg))

        # Reseting the RX matches to its orignal default values found in the acq2106:
        # /mnt/local/sysconfig/wr.sh
        # uut.s11.wrtd_reset_tx = 1

    TRIG=trig