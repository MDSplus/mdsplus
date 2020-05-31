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
import paramiko


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
        {'path':':WRTT0_MSG',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
        {'path':':WRTT1_MSG',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
        {'path':':WR_INIT',     'type':'text',   'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_TICKNS', 'type':'text', 'value': "WRTD_TICKNS=50", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DNS',    'type':'text', 'value': "WRTD_DELTA_NS=50000000", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_VBOSE',  'type':'text', 'value': "WRTD_VERBOSE=2", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RTP',    'type':'text', 'value': "WRTD_RTPRIO=15", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_M',   'type':'text', 'value': "WRTD_RX_MATCHES=$(hostname)", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_M1',  'type':'text', 'value': "WRTD_RX_MATCHES1=$(hostname)", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX_DTP', 'type':'text', 'value': "WRTD_RX_DOUBLETAP=double_tap", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DELAY',  'type':'text', 'value': "WRTD_DELAY01=5000000", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_ID',     'type':'text', 'value': "WRTD_ID=wrtt0", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_TX',     'type':'text', 'value': "WRTD_TX=0", 'options':('write_shot',)},

        {'path':':RUNNING',     'type':'numeric', 'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',    'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",'options':('no_write_shot',)},
        ]

    def init(self, newmsg=''):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())

        msgs  = str(newmsg)
        wrmgs = msgs.split(":")

        self.wrtt0_msg.record = str(wrmgs[0])
        self.wrtt1_msg.record = str(wrmgs[1])

        # a='export WRTD_TICKNS=50'                  # For SR=20MHz, for ACQ423, this number will be much bigger. It's the Si5326 tick at 20MHz ..
        # b='export WRTD_DELTA_NS=50000000'          # 50msec - our "safe time for broadcast"
        # c='export WRTD_VERBOSE=2'                  # use for debugging - eg logread -f or nc localhost 4280

        # # Peter's test configuration:
        # # runs in REAL TIME mode, maybe good for WRTD_DNS..
        # d='export WRTD_RTPRIO=15'

        # #Match for WRTT0:
        # e='export WRTD_RX_MATCHES=$(hostname),wrtt0,' + str(wrmgs[0])

        # #Match for WRTT1:
        # f='export WRTD_RX_MATCHES1=wrtt1,' + str(wrmgs[1])

        # # Match for DOUBLETAP:
        # # set WRTT0, delay WRTD_DELAY01, set WRTT1
        # g='export WRTD_RX_DOUBLETAP=double_tap' 
        # h='export WRTD_DELAY01=5000000'
        # i='export WRTD_ID=wrtt0'
        # j='WRTD_TX=0'

        #Record the state of the WRTD environment:
        self.wrtd_delay  = uut.s11.WRTD_DELAY01
        self.wrtd_dns    = uut.s11.WRTD_DELTA_NS
        self.wrtd_id     = uut.s11.WRTD_ID
        self.wrtd_rx     = uut.s11.WRTD_RX
        self.wrtd_rx_dtp = uut.s11.WRTD_RX_DOUBLETAP
        self.wrtd_rx_m   = uut.s11.WRTD_RX_MATCHES
        self.wrtd_rx_m1  = uut.s11.WRTD_RX_MATCHES1
        self.wrtd_tickns = uut.s11.WRTD_TICKNS
        self.wrtd_tx     = uut.s11.WRTD_TX
        self.wrtd_vbose  = uut.s11.WRTD_VERBOSE

        # Define RX matches:
        uut.s11.WRTD_RX_MATCHES  = str(wrmgs[0])
        uut.s11.WRTD_RX_MATCHES1 = str(wrmgs[1])

        #Commit the changes for WRTD RX
        uut.s11.wrtd_commit_rx = 1

    INIT=init

    def trig(self, msg=''):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data())

        message = str(msg)

        self.TRIG_MSG.record = message

        if message in self.WRTT0_MSG.data():
            uut.s0.SIG_SRC_TRG_0   = 'WRTT0'
            uut.s0.SIG_EVENT_SRC_0 = 'WRTT0'
            # Save choices in tree node:
            self.trig_src.record   = 'WRTT0'
            self.event0_src.record = 'WRTT0'

        elif message in self.WRTT1_MSG.data():
            uut.s0.SIG_SRC_TRG_1   = 'WRTT1'
            uut.s0.SIG_EVENT_SRC_0 = 'WRTT1'
            # Save choices in tree node:
            self.trig_src.record   = 'WRTT1'
            self.event0_src.record = 'WRTT1'
        
        else:
            print('Message does not match either of the WRTTs available')


        wrtdtx = '1 --tx_id=' + message
        uut.s0.wrtd_tx_immediate = wrtdtx
    
        self.trig_time.putData(MDSplus.Int64(uut.s0.wr_tai_cur))

    TRIG=trig