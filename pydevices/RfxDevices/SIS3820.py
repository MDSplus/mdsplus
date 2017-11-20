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

from MDSplus import mdsExceptions, Device, Signal, Data, Dimension, Window, Range, Int32Array
from ctypes import c_int

class SIS3820(Device):
    """SIS3820 Struck Multi Purpose Scaler"""
    parts=[{'path':':BASE_ADDR', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':IP_ADDR', 'type':'text'},
        {'path':':OP_MODE', 'type':'text', 'value':'MULTI CHANNEL SCALER'},
        {'path':':LNE_MODE', 'type':'text', 'value':'INTERNAL 10MHZ'},
        {'path':':LNE_SOURCE', 'type':'numeric'},
        {'path':':SCAN_COUNT', 'type':'numeric'}]

    for i in range(0, 32):
        if i < 10:
            parts.append({'path':'.CHANNEL_0%d'%(i), 'type':'structure'})
            parts.append({'path':'.CHANNEL_0%d:DATA'%(i),'type':'signal', 'options':('no_write_model','compress_on_put')})
        else:
            parts.append({'path':'.CHANNEL_%d'%(i), 'type':'structure'})
            parts.append({'path':'.CHANNEL_%d:DATA'%(i),'type':'signal', 'options':('no_write_model','compress_on_put')})
    del(i)

    parts.append({'path':':INIT_ACTION','type':'action',
    'valueExpr':"Action(Dispatch('VME_SERVER','INIT',50,None),Method(None,'init',head))",
    'options':('no_write_shot',)})
    parts.append({'path':':ARM_ACTION','type':'action',
    'valueExpr':"Action(Dispatch('VME_SERVER','ARM',50,None),Method(None,'arm',head))",
    'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
    'valueExpr':"Action(Dispatch('VME_SERVER','STORE',50,None),Method(None,'store',head))",
    'options':('no_write_shot',)})

    # Init function
    def init(self):
        print('************ START INIT ************')
    # Get IP Address
        try:
            ipAddr = self.ip_addr.data()
            print('IP Addr=',ipAddr)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Remote IP Address')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Get Base Address
        try:
            baseAddr = self.base_addr.data()
            print('Base Addr =',baseAddr)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Base Address specification')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Get OP Mode
        opModeDict = {'SCALER':0, 'MULTI CHANNEL SCALER':1, 'VME FIFO WRITE':2}
        try:
            opMode = opModeDict[self.op_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid OP Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('OP Mode=',opMode)
    # Get LNE Mode
        lneModeDict = {'VME':0, 'CONTROL SIGNAL':1, 'INTERNAL 10MHZ':2, 'CHANNEL N':3, 'PRESET':4}
        try:
            lneMode = lneModeDict[self.lne_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid LNE Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('LNE Mode=',lneMode)
    # Get LNE Source
        try:
            lneSource = self.lne_source.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid LNE Source')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('LNE Source=',lneSource)
    # Get Scan Count
        try:
            scanCount = self.scan_count.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Scan Count')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('Scan Count=',scanCount)
    # Get Channels Setup
        channelMask = 0
        for a in range(0, 32):
            if a < 10:
                if getattr(self, 'channel_0%d'%(a)).isOn():
                    print('Channel_0' + str(a) + ' IS ON')
                    channelMask = channelMask | (1 << a)
            else:
                if getattr(self, 'channel_%d'%(a)).isOn():
                    print('Channel_' + str(a) + ' IS ON')
                    channelMask = channelMask | (1 << a)
        del a
        print('Channel Mask= ', channelMask)
    # Connect to SIS3820 via MdsIP
        status = Data.execute('MdsConnect("'+ ipAddr +'")')
        if status == 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, "Cannot Connect to VME. See VME console for details")
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # init SIS3820 Configuration
        if status > 0:
            status = Data.execute('MdsValue("SIS3820->sis3820_init(val($1),val($2),val($3),val($4),val($5))",$1,$2,$3,$4,$5)',baseAddr,opMode,lneMode,scanCount,channelMask)
            if status != 0:
                Data.execute('MdsDisconnect()')
                Data.execute('DevLogErr($1,$2)', self.nid, "Cannot execute HW initialization. See VME console for details")
                raise mdsExceptions.TclFAILED_ESSENTIAL
            Data.execute('MdsDisconnect()')
        print('************ END INIT ************')
        return
########################################### END INIT #######################################

    # Arm Function
    def arm(self):
        print('************ START ARM ************')
    # Get IP Address
        try:
            ipAddr = self.ip_addr.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Remote IP Address')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Get Base Address
        try:
            baseAddr = self.base_addr.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Base Address specification')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Connect to SIS3820 via MDS IP
        status = Data.execute('MdsConnect("'+ ipAddr +'")')
        if status == 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Connect to VME. See VME console for details')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # ARM SIS3820
        status = Data.execute('MdsValue("SIS3820->sis3820_arm(val($1))", $1)', baseAddr)
        if status != 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute HW ARM. See VME console for details')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        Data.execute('MdsDisconnect()')
        print('************ END ARM ************')
        return
########################################### END ARM #######################################

    # Store Function
    def store(self):
        print('************ START STORE ************')
    # Get IP Address
        try:
            ipAddr = self.ip_addr.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Remote IP Address')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Get Base Address
        try:
            baseAddr = self.base_addr.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Base Address specification')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Get Scan Count
        try:
            scanCount = self.scan_count.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid Scan Count')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('Scan Count=',scanCount)
    # Get LNE Mode
        lneModeDict = {'VME':0, 'CONTROL SIGNAL':1, 'INTERNAL 10MHZ':2, 'CHANNEL N':3, 'PRESET':4}
        try:
            lneMode = lneModeDict[self.lne_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid LNE Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('LNE Mode=',lneMode)
    # Get LNE Source
        try:
            lneSource = self.lne_source.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid LNE Source')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('LNE Source=',lneSource)
    # Get Channels Setup
        channelMask = 0
        for a in range(0, 32):
            if a < 10:
                if getattr(self, 'channel_0%d'%(a)).isOn():
                    print('Channel_0' + str(a) + ' IS ON')
                    channelMask = channelMask | (1 << a)
            else:
                if getattr(self, 'channel_%d'%(a)).isOn():
                    print('Channel_' + str(a) + ' IS ON')
                    channelMask = channelMask | (1 << a)
        del a
        print('Channel Mask= ', channelMask)
    # Connect to SIS3820 via MDS IP
        status = Data.execute('MdsConnect("'+ ipAddr +'")')
        if status == 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot Connect to VME. See VME console for details')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Wait End Acquisition
        status = Data.execute('MdsValue("SIS3820->sis3820_waitEndAcquisition(val($1), val($2))", $1, $2)', baseAddr, scanCount)
        if status != 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute HW Acquisition. See VME console for details')
            raise mdsExceptions.TclFAILED_ESSENTIAL
    # Pre Store Fase
        status = Data.execute('MdsValue("SIS3820->sis3820_preStore(val($1), val($2))", $1, $2)', baseAddr, channelMask)
        if status != 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot execute HW Acquisition. See VME console for details')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        DataArray = c_int * scanCount
        rawChan = []
        rawChan = DataArray()

        if lneMode == 2:
            trigTime = 0
            clockPeriod = 10e-6
        else:
            try:
                clk = self.lne_source.evaluate()
                clockPeriod = clk.delta
                trigTime = clk.begin
                #ending = clk.end
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid LNE Source')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        for chan in range(0,32):
            if channelMask & ( 1 << chan ):
            # Read Chan Data
                rawChan = Data.execute('MdsValue("SIS3820->sis3820_readChData:dsc( val($1))", $1)', chan)
            # Build the Dimension object in a single call
                dim = Dimension(Window(0, scanCount, trigTime), Range(None, None, clockPeriod))
            # Put all togenther in a "Signal" object.
                convExpr = Data.compile("$VALUE")
            # Use MDSplus Int32Array object
                rawMdsData = Int32Array( rawChan )
            # Every MDSplus data type can have units associated with it
                rawMdsData.setUnits("Count")
                convExpr.setUnits("Count")
            # Build the signal object
                signal = Signal(convExpr, rawMdsData, dim)
            # Write the signal in the tree
                if chan < 10:
                    try:
                        self.__getattr__('channel_0%d_data'%(chan)).putData(signal)
                    except:
                        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot write Signal in the tree')
                        print('Cannot write Signal in the tree')
                else:
                    try:
                        self.__getattr__('channel_%d_data'%(chan)).putData(signal)
                    except:
                        Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot write Signal in the tree')
                        print('Cannot write Signal in the tree')

        Data.execute('MdsDisconnect()')
        del chan
        print('************ END STORE ************')
        return
########################################### END STORE #######################################