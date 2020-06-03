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
import time
import threading

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
        {'path':':COMMENT',     'type':'text',    'options':('no_write_shot',)},
        {'path':':TRIG_MSG',    'type':'text',    'options':('write_shot',)},
        {'path':':TRIG_SRC',    'type':'text',    'value': 'WRTTx', 'options':('write_shot',)},
        {'path':':EVENT0_SRC',  'type':'text',    'value': 'WRTTx', 'options':('write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric', 'value': 0,       'options':('write_shot',)},
        {'path':':T0',          'type':'numeric', 'value': 0.1,       'options':('write_shot',)},
        {'path':':WRTT0_MSG',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
        {'path':':WRTT1_MSG',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
        {'path':':WR_INIT',     'type':'text',   'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_TICKNS', 'type':'numeric', 'value': 50, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DNS',    'type':'numeric', 'value': 50000000, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_VBOSE',  'type':'numeric', 'value': 2, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RTP',    'type':'numeric', 'value': 15, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_M',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_M1',  'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_DTP', 'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DELAY',  'type':'numeric', 'value': 5000000, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_ID',     'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_TX',     'type':'numeric', 'value': 0, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX',     'type':'numeric', 'value': 0, 'options':('write_shot',)},

        {'path':':RUNNING',     'type':'numeric', 'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',    'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",'options':('no_write_shot',)},
        ]

    def init(self, newmsg=''):
        import acq400_hapi
        #uut = acq400_hapi.Acq400(self.node.data())
        uut = acq400_hapi.Acq2106('172.20.240.13', has_wr=True)

        msgs  = str(newmsg)
        wrmgs = msgs.split(":")

        self.wrtt0_msg.record = str(wrmgs[0])
        self.wrtt1_msg.record = str(wrmgs[1])

        #Record the state of the WRTD environment:
        self.wr_init_wrtd_delay.record  = MDSplus.Int64(uut.cC.WRTD_DELAY01)
        self.wr_init_wrtd_dns.record    = MDSplus.Int64(uut.cC.WRTD_DELTA_NS) # 50msec - our "safe time for broadcast"
        self.wr_init_wrtd_id.record     = uut.cC.WRTD_ID
        self.wr_init_wrtd_rx_dtp.record = uut.cC.WRTD_RX_DOUBLETAP            # Match for DOUBLETAP: set WRTT0, delay WRTD_DELAY01, set WRTT1
        self.wr_init_wrtd_rx_m.record   = str(wrmgs[0])                       # Match for WRTT0
        self.wr_init_wrtd_rx_m1.record  = str(wrmgs[1])                       # Match for WRTT1
        self.wr_init_wrtd_tickns.record = MDSplus.Int64(uut.cC.WRTD_TICKNS)   # For SR=20MHz, for ACQ423, this number will be much bigger. It's the Si5326 tick at 20MHz ..
        self.wr_init_wrtd_tx.record     = MDSplus.Int64(uut.cC.WRTD_TX)
        self.wr_init_wrtd_rx.record     = MDSplus.Int64(uut.cC.WRTD_RX)
        self.wr_init_wrtd_vbose.record  = MDSplus.Int64(uut.cC.WRTD_VERBOSE)  # use for debugging - eg logread -f or nc localhost 4280

        # Turn on RX:
        uut.cC.WRTD_RX = '1'
        #Commit the changes for WRTD RX
        uut.cC.wrtd_commit_rx = 1

        # Define RX matches:
        uut.cC.WRTD_RX_MATCHES  = str(wrmgs[0])
        uut.cC.WRTD_RX_MATCHES1 = str(wrmgs[1])
        #Commit the changes for WRTD RX
        uut.cC.wrtd_commit_rx = 1

    INIT=init

    def trig(self, msg=''):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), has_wr=True)
        # uut = acq400_hapi.Acq400(self.node.data())

        message = str(msg)

        self.TRIG_MSG.record = message

        # Choose the source (eg. WRTT0 or WRTT1) that use go through the bus (TRG, EVENT) and the signal (d0, d1)
        if message in uut.cC.WRTD_RX_MATCHES:
            uut.s0.SIG_SRC_TRG_0   = 'WRTT0'
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_0 = 'TRG'
            # Save choices in tree node:
            self.trig_src.record   = 'WRTT0'
            self.event0_src.record = 'WRTT0'

        elif message in uut.cC.WRTD_RX_MATCHES1:
            uut.s0.SIG_SRC_TRG_1   = 'WRTT1'
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_1 = 'TRG'
            # Save choices in tree node:
            self.trig_src.record   = 'WRTT1'
            self.event0_src.record = 'WRTT1'
        
        else:
            print('Message does not match either of the WRTTs available')


        # uut.cC.WRTD_ID = message

        # # Commit the changes for WRTD TX
        # uut.cC.wrtd_commit_tx = 1

        wrtdtx = '1 --tx_id=' + message
        uut.s0.wrtd_tx_immediate = wrtdtx
    
        self.trig_time.putData(MDSplus.Int64(uut.s0.wr_tai_cur))

        # Reseting the RX matches to its orignal default values found in the acq2106:
        # /mnt/local/sysconfig/wr.sh
        # uut.cC.wrtd_reset_tx = 1

    TRIG=trig