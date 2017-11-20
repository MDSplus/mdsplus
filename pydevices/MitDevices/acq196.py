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

class ACQ196(acq.Acq):
    """
    D-Tacq ACQ196  96 channel transient recorder

    device support for d-tacq acq196 http://www.d-tacq.com/acq196cpci.shtml     
    """
    from copy import copy
    parts=copy(acq.Acq.acq_parts)

    for i in range(96):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
    del i
    parts.extend(acq.Acq.action_parts)
    for part in parts:                
        if part['path'] == ':ACTIVE_CHAN' :
            part['value']=96                 
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
        if self.debugging():
            print 'ACQ196 initftp path = %s tree = %s shot = %d\n' % (path, tree, shot)

        active_chan = self.getInteger(self.active_chan, DevBAD_ACTIVE_CHAN)
        if active_chan not in (32,64,96) :
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
        try:
            clock_div = int(self.clock_div)
        except:
            clock_div = 1
        if self.debugging():
            print "have the settings\n";


#
# now create the post_shot ftp command file
#
#            fd = tempfile.TemporaryFile()
        fd = tempfile.NamedTemporaryFile(mode='w+b', bufsize=-1, suffix='.tmp', prefix='tmp', dir='/tmp', delete= not self.debugging())
        if self.debugging():
            print 'opened temporary file %s\n'% fd.name
        self.startInitializationFile(fd, trig_src, pre_trig, post_trig)
        fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
        if clock_src == 'INT_CLOCK':
            if clock_out == None:
                if self.debugging():
                    print "internal clock no clock out\n"
                fd.write("acqcmd setInternalClock %d\n" % clock_freq)
            else:
                clock_out_num_str = clock_out[-1]
                clock_out_num = int(clock_out_num_str)
                setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
                if self.debugging():
                    print "internal clock clock out is %s setDIOcmd = %s\n" % (clock_out, setDIOcmd,)
                fd.write("acqcmd setInternalClock %d DO%s\n" % (clock_freq, clock_out_num_str,))
                fd.write(setDIOcmd)         
        else:
            if (clock_out != None) :
                clock_out_num_str = clock_out[-1]
                clock_out_num = int(clock_out_num_str)
                setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
                fd.write("acqcmd setExternalClock %s %d DO%s\n" % (clock_src, clock_div,clock_out_num_str))
                fd.write(setDIOcmd)
            else:
                fd.write("acqcmd setExternalClock %s %d\n" % (clock_src, clock_div,))
#
# set the channel mask 2 times
#
        fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
        fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
#
#  set the pre_post mode last
#
        fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig, post_trig, trig_src, 'rising',))
            
        self.addGenericJSON(fd)

        fd.write("add_cmd 'get.vin 1:32'>> $settingsf\n")
        fd.write("add_cmd 'get.vin 33:64'>> $settingsf\n")
        fd.write("add_cmd 'get.vin 65:96'>> $settingsf\n")
        self.finishJSON(fd, auto_store)

        print "Time to make init file = %g\n" % (time.time()-start)
        start=time.time()
        self.doInit(fd)

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

        vin1 = self.settings['get.vin 1:32']
        vin2 = self.settings['get.vin 33:64']
        vin3 = self.settings['get.vin 65:96']
        active_chan = int(self.active_chan.record)
	if active_chan == 96 :
            vins = eval('MDSplus.makeArray([%s, %s, %s])' % (vin1, vin2, vin3,))
	else :
	    if active_chan == 64 :
	        vins = eval('MDSplus.makeArray([%s, %s])' % (vin1, vin2,))
	    else :
                vins = eval('MDSplus.makeArray([%s])' % (vin1,))
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
        for chan in range(96):
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
