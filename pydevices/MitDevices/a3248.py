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

from MDSplus import Device

class A3248(Device):
    """
    AEON 3248  4 channel transient recorder
    
    Methods:
    Add() - add a DT216B device to the tree open for edit
    Init() - initialize the AEON 3248 device - implimented in TDI 
                write setup parameters and waveforms to the device
    Store() - store the data acquired by the device - implimented in TDI
    Trigger() - trigger the device - implimented in TDI
    Help() - Print this message
    
    Nodes:
    
    :NAME - CTS name of device
    :COMMENT - user comment string
    :EXT_CLOCK
    :FREQUENCY - in KHZ
    :TRIGGER
    :GAIN -  Gain for all of the channels 1,2,4,8
    :OFFSET - Offset for all of the channels 0 .. 255 == 0  ..  5.1V
    :PRETRIG  - Pretrigger samples - up to 32k rounded down to nearest multiple of 128
    :CLOCK - place for the module to store the internal clock expression
    :INPUT_[1-4] - place for module to store data in volts (reference to INPUT_NN:RAW)
                  :START_IDX - first sample to store for this channel
                  :END_IDX - last sample to store for this channel
                  :INC - decimation factor for this channel

    :INIT_ACTION - dispatching information for INIT
    :STORE_ACTION - dispatching information for STORE
    """
    
    parts=[
        {'path':':NAME','type':'text','value':'a3248_1','options':('no_write_shot',)},
        {'path':':COMMENT','type':'text'},
        {'path':':ext_clock','type':'axis', 'options':('no_write_shot',)},
        {'path':':frequency','type':'numeric', 'value':1000, 'options':('no_write_shot',)},
        {'path':':trigger','type':'numeric', 'value':0.0, 'options':('no_write_shot',)},
        {'path':':gain','type':'numeric', 'value':1, 'options':('no_write_shot',)},
        {'path':':offset','type':'numeric', 'value':128, 'options':('no_write_shot',)},
        {'path':':pretrig','type':'numeric', 'value':0, 'options':('no_write_shot',)},
        {'path':':clock','type':'axis', 'options':('no_write_model',)},
        ]
    for i in range(4):
        parts.append({'path':':INPUT_%1.1d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%1.1d:START_IDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%1.1d:END_IDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
    parts.append({'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                  'options':('no_write_shot',)})
    clock_freqs = (10000, 6670, 5000, 4000, 3330, 2860, 2500, 0, 1000, 667, 500, 400, 333, 286, 250, 0)
    dts = (1E-7,1.5E-7,2E-7,2.5E-7,3E-7,3.5E-7,4E-7,0.0,1E-6,1.5E-6,2E-6,2.5E-6,3E-6,3.5E-6,4E-6,0.0)
    def getInteger(self, node, cls):
        try:
            ans = int(node.record)
        except Exception, e:
            print "AEON 3248 error reading %s erro is\n%s" %(node, e,)
            raise cls()
        return ans

    def getString(self, node, cls):
        try:
            ans = str(node.record)
        except Exception, e:
            print "AEON 3248 error reading %s erro is\n%s" %(node, e,)
            raise cls()
        return ans

    def init(self):
        import math
        from MDSplus import Data
        from MDSplus.mdsExceptions import DevBAD_GAIN
        from MDSplus.mdsExceptions import DevBAD_NAME
        from MDSplus.mdsExceptions import DevBAD_OFFSET
        from MDSplus.mdsExceptions import DevBAD_PRE_TRIG
        from MDSplus.mdsExceptions import DevBAD_CLOCK_FREQ

        if self.debug:
            print "A3248 INIT starting"
        name = self.getString(self.name, DevBAD_NAME)
        gain = self.getInteger(self.gain, DevBAD_GAIN)
        if not gain in (1,2,4,8):
            raise(DevBAD_GAIN, "Gain must be one of 1,r or 8")
        gain_code = int(math.log(gain, 2))
        offset = self.getInteger(self.offset, DevBAD_OFFSET)
        offset = max(min(offset, 255), 0)
        gain_offset_reg = (gain_code<<8) | offset

        pretrig = max(min(self.getInteger(self.pretrig, DevBAD_PRE_TRIG),32768),0)
        
        posttrig_code = (32768 - pretrig) << 7
        clock = self.getInteger(self.frequency, DevBAD_CLOCK_FREQ)
        if not clock in self.clock_freqs:
          raise(DevBAD_CLOCK_FREQ, "Clock must be in ".self.clock_freqs)
        if self.ext_clock.length > 0 :
            clock_code = 7
        else :
            clock_code = 0
            for f in  self.clock_freqs:
                if clock == f:
                    break
                clock_code = clock_code+1
        status_reg = clock_code << 8
        if pretrig != 0:
            status_reg = status_reg | posttrig_code | (1 << 12)
        status = Data.execute('AEON_ARM("%s", %d, %d)' % (name, gain_offset_reg, status_reg,))
        if (status & 1) == 0:
            raise DevCAMAC_ERROR('AEON 3248 Arming %s status %d'%(name, status,))        
    INIT=init

    def trig(self):
        from MDSplus import Data
        from MDSplus.mdsExceptions import DevBAD_NAME
        name = self.getString(self.name, DevBAD_NAME)
        status = Data.execute('AEON_TRIGGER("%s")' % (name,))
        if (status & 1) == 0:
            raise DevCAMAC_ERROR('AEON 3248 Triggering %s status %d'%(name, status,))        
    TRIGGER=trig

    def store(self):
        """ Store method for the aeon 3248 digitizer  """
        from MDSplus import Data
        from MDSplus import Range
        from MDSplus.mdsExceptions import DevBAD_NAME
        from MDSplus.mdsExceptions import DevNOT_TRIGGERED

        if self.debug:
            print "starting A3248 store"
        name = self.getString(self.name, DevBAD_NAME)
        status = Data.execute('AEON_CHECKTRIGGER("%s")' % (name,))
        if not (status & 1) :
            raise DevNOT_TRIGGERED("Module %s not triggered"%name)
        setup_vector = Data.execute('AEON_GETSETUP("%s")' % (name,))
        status_reg = int(setup_vector[0])
        gain_reg = int(setup_vector[1])
        addr = int(setup_vector[2])       
        if self.debug:
            print "get dt"
        dt = self.dts[int((status_reg >> 8)&0xF)]   
        pts = 32768
        if status_reg & 0x00F1 :
            pts = (status_reg&0xFF)*128
        if self.debug:
            print "gain reg is %d"%gain_reg
        gain_code = ((gain_reg >> 8) & 3)
        gain =  (1 << gain_code)
        if self.debug:
            print "gain is %d"%gain
        offset = gain_reg &0xFF
        offset = int(offset)
        if self.debug:
            print "store clock record"
        if dt == 0 :
            self.clock.record=self.ext_clock
        else :
            self.clock.record=Range(None, None, dt)

        for chan in range(4):
            self.storeChannel(name, chan, addr, pts, gain, offset)
        return 1
    STORE=store

    def dw_setup(self):
        from MDSplus import Data
        Data.execute('a3248__dw_setup($)', self.nid)
        return 1
    DW_SETUP=dw_setup

    def storeChannel(self, name, chan, addr, pts, gain, offset):
        import MDSplus
        
        chan_node = self.__getattr__('input_%1.1d' % (chan+1,))
        if chan_node.on :
            if self.debug:
                print "it is on so ..."
            start=0
            end=pts-1
            try:
                start = max(int(self.__getattr__('input_%1.1d_start_idx'%(chan+1,))),0)
            except:
                pass
            try:
                end = min(int(self.__getattr__('input_%1.1d_end_idx'%(chan+1,))),pts-1)
            except:
                pass
            if self.debug:
                print "about to aeon_getchannel(%s, %d, %d %d)" % (name, addr, chan, end+1,)
            buf = MDSplus.Data.execute('aeon_getchannel("%s", %d, %d, %d)' % (name, addr, chan, end+1,)) 
            dim = MDSplus.Dimension(MDSplus.Window(start, end, self.trigger ), self.clock)
            if self.debug:
                print "about to make dat"
	        print "gain = %d"%gain
                print "offset =%d"%offset
                print "dim is %s"% str(dim)
                print "start is %d end is %d"%(start, end,)
            dat = MDSplus.Data.compile(
                        'build_signal(build_with_units(($value - $2)*.02/$1, "V") ,build_with_units($3,"Counts"),$4)',
                        gain, offset, buf[start : end], dim) 
            exec('c=self.input_'+'%1d'%(chan+1,)+'.record=dat')

    def help(self):
        """ Help method to describe the methods and nodes of the AEON_3248 module type """
        help(A3248)
        return 1
    HELP=help
