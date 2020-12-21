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
        # If DIO-PG is present:
        {'path':':DIO_SITE',    'type':'numeric', 'value': 0.,       'options':('write_shot',)},
        # A DIO-PG source can be one of [d0...d5, TRGIN, WRTT]
        {'path':':PG_TRIG_SRC','type':'text',    'value': 'dx', 'options':('write_shot',)},
        
        {'path':':WR_INIT',     'type':'text',   'options':('write_shot',)},
            # ns per tick. (tick size in ns). uut.cC.WRTD_TICKNS: For SR=20MHz. It's the Si5326 tick at 20MHz ..
            {'path':':WR_INIT:WRTD_TICKNS', 'type':'numeric', 'value': 50, 'options':('write_shot',)}, 
            # 50msec - our "safe time for broadcast". From uut.cC.WRTD_DELTA_NS
            {'path':':WR_INIT:WRTD_DNS',    'type':'numeric', 'value': 50000000, 'options':('write_shot',)}, 
            # uut.cC.WRTD_VERBOSE: use for debugging - eg logread -f or nc localhost 4280
            {'path':':WR_INIT:WRTD_VBOSE',  'type':'numeric', 'value': 2, 'options':('write_shot',)},        
            # uut.cC.WRTD_RX_MATCHES: match any of these triggers to initiate WRTT0
            {'path':':WR_INIT:WRTD_RX_M',   'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            # uut.cC.WRTD_RX_MATCHES1: match any of these triggers to initiate WRTT1
            {'path':':WR_INIT:WRTD_RX_M1',  'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            # From uut.cC.WRTD_DELAY01: WRTD_RX_DOUBLETAP: match any of these triggers to initiate a
            # Double Tap, which is:
            # 1. WRTT0
            # 2. Delay WRTD_DELAY01 nsec.
            # 3. WRTT1
            {'path':':WR_INIT:WRTD_RX_DTP', 'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_DELAY',  'type':'numeric', 'value': 5000000, 'options':('write_shot',)}, 
            # WRTD_ID
            # For Global ID
            {'path':':WR_INIT:WRTD_ID_GLB',     'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},
            #For an individual DIO 482 site WRTD ID. This is used only in TIGA systems.
            {'path':':WR_INIT:WRTD_ID_TIGA',    'type':'text', 'value': "acq2106_999", 'options':('write_shot',)},

            {'path':':WR_INIT:WRTD_TX',     'type':'numeric', 'value': 1, 'options':('write_shot',)},
            {'path':':WR_INIT:WRTD_RX',     'type':'numeric', 'value': 1, 'options':('write_shot',)},
        

        {'path':':RUNNING',     'type':'numeric', 'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',    'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action',  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))", 'options':('no_write_shot',)},
        ]

    def init(self):
        uut = self.getUUT()

        # Sets WR "safe time for broadcasts" the message, i.e. WRTT_TAI = TAI_TIME_NOW + WRTD_DELTA_NS
        uut.cC.WRTD_DELTA_NS = self.wr_init_wrtd_dns.data()

        # Receiver:
        # Turn on RX
        uut.cC.WRTD_RX = int(self.wr_init_wrtd_rx.data())
        # Define RX matches
        matches  = str(self.wr_init_wrtd_rx_m.data())
        matches1 = str(self.wr_init_wrtd_rx_m1.data())
        print("Messages are {} and {}".format(matches, matches1))

        uut.cC.WRTD_RX_MATCHES  = matches
        uut.cC.WRTD_RX_MATCHES1 = matches1
        
        #Commit the changes for WRTD RX
        uut.cC.wrtd_commit_rx = 1

        # Transmiter:
        # Turn on TX
        uut.cC.WRTD_TX = int(self.wr_init_wrtd_tx.data())
        #Commit the changes for WRTD TX
        uut.cC.wrtd_commit_tx = 1

    INIT=init

    def trig(self, msg=''):
        uut = self.getUUT()
        pg_slot = self.getDioSlot()

        # Is the System a TIGA system?
        # In embedded software there is also a command:
        # /usr/local/bin/is_tiga
        #
        # it's not a knob and it's totally silent, it contains the following:
        # #!/bin/sh
        # [ -e /dev/acq400.0.knobs/wr_tai_trg_s1 ] && exit 0
        # exit 1
        #
        # We can emulate that in HAPI the following way:
        try:
            is_tiga = uut.s0.wr_tai_trg_s1 is not None
        except:
            is_tiga = False

        message = str(msg)
        
        self.TRIG_MSG.record = message

        if message in uut.cC.WRTD_RX_MATCHES:
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_0 = 'TRG'
        elif message in uut.cC.WRTD_RX_MATCHES1:
            # To be sure that the EVENT bus is set to TRG
            uut.s0.SIG_EVENT_SRC_1 = 'TRG'      
        else:
            print('Message does not match either of the WRTTs available')
            self.running.on = False

        if not message.strip():
            # Set WRTD_ID: message to be transmitted from this device if FTTRG or HDMI is used to trigger.
            print("Waiting for external trigger ({})...".format(str(self.wr_init_wrtd_id_glb.data()), str(self.trig_src.data())))
            uut.cC.WRTD_ID = str(self.wr_init_wrtd_id_glb.data())
            # Set trigger input on (FTTRG or HDMI)
            uut.s0.WR_TRG    = 1
            uut.s0.WR_TRG_DX = str(self.trig_src.data())

            if is_tiga:
                # Define the WRTD_ID for a particular DIO site in a TIGA system.
                # A DIO-PG can be triggered by TRGIN, and a WR message could be sent with the following ID:
                pg_slot.WRTD_ID = str(self.wr_init_wrtd_id_tiga.data())
                # Define the WRTD_MASK:
                # WRTD_TX_MASK selects the DIO units that respond
                pg_slot.WRTD_TX_MASK = (1<<(int(self.dio_site.data())+1))
                pg_slot.TRG    = 1
                pg_slot.TRG_DX = str(self.pg_trig_src.data())
        else:
            # send immediate WRTD message
            # The timestamp in the packet is:
            # WRTT_TAI = TAI_TIME_NOW + WRTD_DELTA_NS
            print("Message sent: {}".format(message))
            uut.cC.wrtd_txi = message

    TRIG=trig

    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), has_wr=True)
        return uut

    def getDioSlot(self):
        uut = self.getUUT()
        site_number  = int(self.dio_site.data())

        # Verify site_number is a valid int between 1 and 6
        # if site_number in range(1, 7):
        #     self.slot = uut.__getattr__('s' + self.dio_site.data())

        try:
            if site_number   == 0: 
                slot = uut.s0    # Only for a GPG system.
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