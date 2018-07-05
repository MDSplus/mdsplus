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

import MDSplus
import acq

class ACQ132(acq.Acq):
    """
    D-Tacq ACQ132  32 channel transient recorder

    device support for d-tacq acq132 http://www.d-tacq.com/acq132cpci.shtml 
    """
    from copy import copy
    parts=copy(acq.Acq.acq_parts)

    for i in range(32):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
    del i
    parts.extend(acq.Acq.action_parts)
    for part in parts:                
        if part['path'] == ':ACTIVE_CHAN' :
            part['value']=32                 
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
            print("starting init");
        path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot
        if self.debugging():
            print('ACQ132 initftp path = %s tree = %s shot = %d' % (path, tree, shot))

        active_chan = self.getInteger(self.active_chan, DevBAD_ACTIVE_CHAN)
        if active_chan not in (8,16,32) :
            raise DevBAD_ACTIVE_CHAN()
        if self.debugging():
            print("have active chan")

        try:
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
        except Exception as e:
            raise DevBAD_TRIG_SRC(str(e))
        if self.debugging():
            print("have trig_src")

        try:
            clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        except Exception as e:
            raise DevBAD_CLOCK_SRC(str(e))
        if self.debugging():
            print("have clock src")

        try:
            clock_out=self.clock_out.record.getOriginalPartName().getString()[1:]
        except:
            clock_out=None

        pre_trig = self.getInteger(self.pre_trig, DevBAD_PRE_TRIG)*1024
        if self.debugging():
            print("have pre trig")

        post_trig = self.getInteger(self.post_trig, DevBAD_POST_TRIG)*1024
        if self.debugging():
            print("have post trig")

        clock_freq = self.getInteger(self.clock_freq,DevBAD_CLOCK_FREQ)
        try:
            clock_div = int(self.clock_div)
        except:
            clock_div = 1

        if self.debugging():
            print("have the settings")
#
# now create the post_shot ftp command file
#
        fd = tempfile.TemporaryFile()
        self.startInitializationFile(fd, trig_src, pre_trig, post_trig)
        if active_chan == 8 :
            chan_mask = "11110000000000001111000000000000"
        elif active_chan == 16 :
            chan_mask = "11111111000000001111111100000000"
        else :
            chan_mask = "11111111111111111111111111111111"
        fd.write("acqcmd  setChannelMask %s\n"% (chan_mask,))
        if clock_src == 'INT_CLOCK':
            if clock_out == None:
                if self.debugging():
                    print("internal clock no clock out")
                fd.write("acqcmd setInternalClock %d" % clock_freq)
            else:
                clock_out_num_str = clock_out[-1]
                clock_out_num = int(clock_out_num_str)
                setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
                if self.debugging():
                    print("internal clock clock out is %s setDIOcmd = %s" % (clock_out, setDIOcmd))
                fd.write("acqcmd setInternalClock %d DO%s\n" % (clock_freq, clock_out_num_str,))
                fd.write(setDIOcmd)         
        else:
            fd.write("acqcmd -- setExternalClock --fin %d --fout %d %s\n" % (clock_freq/1000, clock_freq/1000*clock_div, clock_src,))

        fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig, post_trig, trig_src, 'rising',))
            
        self.addGenericJSON(fd)

        fd.write("add_cmd 'get.vin 1:32'>> $settingsf\n")
        self.finishJSON(fd, auto_store)

        print("Time to make init file = %g" % (time.time()-start))
        start=time.time()
        self.doInit(fd)
        fd.close()

        print("Time for board to init = %g" % (time.time()-start))
        return  1

    INITFTP=initftp

    def store(self, arg1='checks', arg2='noauto'):
        if self.debugging():
            print("Begining store")

        self.checkTrigger(arg1, arg2)
        self.loadSettings()
        self.checkTreeAndShot(arg1, arg2)
        self.storeStatusCommands()

        preTrig = self.getPreTrig()
        postTrig = self.getPostTrig()
        if self.debugging():
            print("got preTrig %d and postTrig %d" % (preTrig, postTrig,))

        vin1 = self.settings['get.vin 1:32']
        vins = eval('MDSplus.makeArray([%s])' % (vin1,))

        if self.debugging():
            print("got the vins %s"%str(vins))
        self.ranges.record = vins
        chanMask = self.settings['getChannelMask'].split('=')[-1]
        if self.debugging():
            print("chan_mask = %s" % (chanMask,))
        clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        if self.debugging():
            print("clock_src = %s" % (clock_src,))
        if clock_src == 'INT_CLOCK' :
            intClock = float(self.settings['getInternalClock'].split()[1])
            if intClock > 16000000:
                intClock = 2000000
            delta=1./float(intClock)
            self.clock.record = MDSplus.Range(None, None, delta)
        else:
            self.clock.record = self.clock_src

        clock = self.clock.record

#
# now store each channel
#
        last_error=None
        for chan in range(32):
            try:
                self.storeChannel(chan, chanMask, preTrig, postTrig, clock, vins)
            except Exception as e:
                print("Error storing channel %d\n%s" % (chan, e,))
                last_error = e
        self.dataSocketDone()
        if last_error:
            raise last_error

        return 1

    STORE=store
