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

import numpy
import array
import MDSplus
import acq

class ACQ216(acq.Acq):
    """
    D-Tacq ACQ216  16 channel transient recorder
    
    device support for d-tacq acq216 http://www.d-tacq.com/acq216cpci.shtml
    """
    from copy import copy
    parts=copy(acq.Acq.acq_parts)

    for i in range(16):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:VIN'%(i+1,),'type':'NUMERIC', 'value':10, 'options':('no_write_shot')})
    del i
    parts.extend(acq.Acq.action_parts)
    for part in parts:                
        if part['path'] == ':ACTIVE_CHAN' :
            part['value']=16                 
    del part
    
    def initftp(self, auto_store=None):
        """
        Initialize the device
        Send parameters
        Arm hardware
        """
        import tempfile
        import time
        from MDSplus.mdsExceptions import DevBAD_ACTIVE_CHAN
        from MDSplus.mdsExceptions import DevBAD_TRIG_SRC
        from MDSplus.mdsExceptions import DevBAD_CLOCK_SRC
        from MDSplus.mdsExceptions import DevBAD_PRE_TRIG
        from MDSplus.mdsExceptions import DevBAD_POST_TRIG
        from MDSplus.mdsExceptions import DevBAD_CLOCK_FREQ

        start=time.time()

        if self.debugging():
            print "starting init\n";
        path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot

        active_chan = self.getInteger(self.active_chan, DevBAD_ACTIVE_CHAN)
        if active_chan not in (4,8,16) :
            raise DevBAD_ACTIVE_CHAN()
        if self.debugging():
            print "have active chan\n";

        try:
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
        except Exception, e:
            raise DevBAD_TRIG_SRC(str(e))
        if self.debugging():
            print "have trig_src\n";

        try:
            clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        except Exception, e:
            raise DevBAD_CLOCK_SRC(str(e))
        if self.debugging():
            print "have clock src\n";

        try:
            clock_out=self.clock_out.record.getOriginalPartName().getString()[1:]
        except:
            clock_out=None

        pre_trig = self.getInteger(self.pre_trig, DevBAD_PRE_TRIG)*1024
        if self.debugging():
            print "have pre trig\n";

        post_trig = self.getInteger(self.post_trig, DevBAD_POST_TRIG)*1024
        if self.debugging():
            print "have post trig\n";

        if clock_src == "INT_CLOCK":
            clock_freq = self.getInteger(self.clock_freq,DevBAD_CLOCK_FREQ)
            clock_div = 1
        else :
            try:
                clock_div = int(self.clock_div)
            except:
                clock_div = 1
        if self.debugging():
            print "have the settings\n";


#
# now create the post_shot ftp command file
#
        try:
            fd = tempfile.TemporaryFile()
            self.startInitializationFile(fd, trig_src, pre_trig, post_trig)
            fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
            for chan in range(16):
		vin = self.__getattr__('input_%2.2d_vin' % (chan+1,))
		if (vin == 2.5) :
		    vin_str = "2.5"
		elif (vin == 4) :
		    vin_str = "4"
		elif (vin == 6) :
		    vin_str = "6"
		elif (vin == 10) :
		    vin_str = "10"
		else :
		    vin_str = "10"		
                fd.write("set.vin %d %s\n" % (chan+1, vin_str))
            if clock_src == 'INT_CLOCK':
                if clock_out == None:
                    if self.debugging():
                        print "internal clock no clock out\n"
                    fd.write("acqcmd setInternalClock %d\n" % clock_freq)
                else:
                    clock_out_num_str = clock_out[-1]
                    clock_out_num = int(clock_out_num_str)
                    setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
#    force the routing for this clock output
#    regardless of the settings for this line 
#    above
		    setRoutecmd = 'set.route d%1.1d in fpga out pxi\n' % (clock_out_num,)
                    if self.debugging():
                        print "internal clock clock out is %s setDIOcmd = %s\n" % (clock_out, setDIOcmd,)
                    fd.write("acqcmd setInternalClock %d DO%s\n" % (clock_freq, clock_out_num_str,))
                    fd.write(setDIOcmd)
		    fd.write(setRoutecmd)         
            else:
                if (clock_out != None) :
                    clock_out_num_str = clock_out[-1]
                    clock_out_num = int(clock_out_num_str)
                    setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
                    fd.write("acqcmd setExternalClock %s %d DO%s\n" % (clock_src, clock_div,clock_out_num_str))
                    fd.write(setDIOcmd)
                else:
                    fd.write("acqcmd setExternalClock %s\n" % clock_src)
#
# set the channel mask twice as per Peter Milne
#
            fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")            
            if self.debugging():
                print "routes all set now move on to pre-post\n"
                print "pre trig = %d\n" % (pre_trig,)
                print "post trig = %d\n" % (post_trig,)
                print "trig_src = %s\n" % (trig_src,)
            fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig, post_trig, trig_src, 'rising',))
            if self.debugging():
                print "pre-post all set now the JSON and commands\n"

            self.addGenericJSON(fd)

            fd.write("add_cmd 'get.vin'>> $settingsf\n")
            self.finishJSON(fd, auto_store)

            print "Time to make init file = %g\n" % (time.time()-start)
            start=time.time()

            self.doInit(fd)

        except Exception,e:
            try:
                fd.close()
            except:
                pass 

            raise

        fd.close()
       
        print "Time for board to init = %g\n" % (time.time()-start)
        return  1

    INITFTP=initftp
        
    def store(self, arg1='checks', arg2='noauto'):
        if self.debugging():
            print "Begining store\n"

        self.checkTrigger(arg1, arg2)
        self.loadSettings()
        self.checkTreeAndShot(arg1, arg2)
        self.storeStatusCommands()

        preTrig = self.getPreTrig()
        postTrig = self.getPostTrig()
        if self.debugging():
            print "got preTrig %d and postTrig %d\n" % (preTrig, postTrig,)

        vin1 = self.settings['get.vin']
        vins = eval('MDSplus.makeArray([%s,])' % (vin1,))

        if self.debugging():
            print "got the vins "
            print vins
        self.ranges.record = vins
        chanMask = self.settings['getChannelMask'].split('=')[-1]
        if self.debugging():
            print "chan_mask = %s\n" % (chanMask,)

        self.storeClock()
        clock = self.clock

#
# now store each channel
#
        last_error=None
        for chan in range(16):
            try:
                self.storeChannel(chan, chanMask, preTrig, postTrig, clock, vins)
            except e:
                print "Error storing channel %d\n%s" % (chan, e,)
                last_error = e

        self.dataSocketDone()
        if last_error:
            raise last_error

        return 1

    STORE=store
