import numpy
import array
import MDSplus
import acq

class ACQ216(acq.ACQ):
    """
    D-Tacq ACQ216  16 channel transient recorder
    
    device support for d-tacq acq216 http://www.d-tacq.com/acq216cpci.shtml
    """
    from copy import copy
    parts=copy(acq.ACQ.acq_parts)

    for i in range(16):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:VIN'%(i+1,),'type':'NUMERIC', 'value':10, 'options':('no_write_shot')})
    del i
    parts.extend(acq.ACQ.action_parts)
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
        start=time.time()
        msg=None

        try:
            if self.debugging():
                print "starting init\n";
            path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            msg="Must specify active chans as int in (4,8,16)"

            active_chan = int(self.active_chan)
            msg=None
            if active_chan not in (4,8,16) :
                print "active chans must be in (4,8,16)"
                active_chan = 16
            if self.debugging():
                print "have active chan\n";

            msg="Could not read trigger source"
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
            if self.debugging():
                print "have trig_src\n";
            msg="Could not read clock source"
            clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
            if self.debugging():
                print "have clock src\n";
            try:
                clock_out=self.clock_out.record.getOriginalPartName().getString()[1:]
            except:
                clock_out=None
            msg="Must specify pre trigger samples"
            pre_trig=int(self.pre_trig.data()*1024)
            if self.debugging():
                print "have pre trig\n";
            msg="Must specify post trigger samples"
            post_trig=int(self.post_trig.data()*1024)
            if self.debugging():
                print "have post trig\n";
            msg=None
            if clock_src == "INT_CLOCK":
                msg="Must specify clock frequency in clock_freq node for internal clock"
                clock_freq = int(self.clock_freq)
                clock_div = 1
                msg=None
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
                    if self.debugging():
                        print "internal clock clock out is %s setDIOcmd = %s\n" % (clock_out, setDIOcmd,)
                    fd.write("acqcmd setInternalClock %d DO%s\n" % (clock_freq, clock_out_num_str,))
                    fd.write(setDIOcmd)         
            else:
 #               if (clock_div != 1) :
 #                   fd.write("acqcmd setExternalClock %s %d DO2\n" % (clock_src, clock_div,))
 #               else:
 #                   fd.write("acqcmd setExternalClock %s\n" % clock_src)
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
                print "pre-post all set now the xml and commands\n"

            self.addGenericXMLStuff(fd)

            fd.write("xmlcmd 'get.vin'>> $settingsf\n")
            self.finishXMLStuff(fd, auto_store)

            print "Time to make init file = %g\n" % (time.time()-start)
            start=time.time()
            self.doInit(fd)
            fd.close()

            print "Time for board to init = %g\n" % (time.time()-start)
            return  1

        except Exception,e:
            if msg != None:
                print 'error = %s\nmsg = %s\n' %(msg, str(e),)
            else:
                print "%s\n" % (str(e),)
            return acq.ACQ.InitializationError

    INITFTP=initftp
        
    def store(self, arg):
        import MitDevices
        import time
        if self.debugging():
            print "Begining store\n"
	self.data_socket=-1
        if not self.triggered():
            print "ACQ216 Device not triggered\n"
            return MitDevices.DevNotTriggered

        complete = 0
        tries = 0
        settings = None
        while not complete and tries < 10 :
            try:
                tries = tries + 1
                settings = self.loadSettings()
                complete=1
            except Exception,e:
                if self.debugging():
                    print "ACQ216 Error loading settings\n%s\n" %(e,)
        if settings == None :
            print "after %d tries could not load settings\n" % (tries,)
            return acq.ACQ.SettingsNotLoaded
        
        path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot
        if self.debugging() :
            print "xml is loaded\n"
        if tree != settings['tree'] :
            print "ACQ216 open tree is %s board armed with tree %s\n" % (tree, settings["tree"],)
            if arg != "nochecks" :
                return acq.ACQ.WrongTree
        if path != settings['path'] :
            print "ACQ216 device tree path %s, board armed with path %s\n" % (path, settings["path"],)
            if arg != "nochecks" :
                return acq.ACQ.WrongPath
        if shot != int(settings['shot']) :
            print "ACQ216 open shot is %d, board armed with shot %d\n" % (shot, int(settings["shot"]),)
            if arg != "nochecks" :
                return acq.ACQ.WrongShot
        status = []
        cmds = self.status_cmds.record
        for cmd in cmds:
            cmd = cmd.strip()
            if self.debugging():
                print "about to append answer for /%s/\n" % (cmd,)
                print "   which is /%s/\n" %(settings[cmd],)
            status.append(settings[cmd])
            if self.debugging():
                print "%s returned %s\n" % (cmd, settings[cmd],)
        if self.debugging():
            print "about to write board_status signal"
        self.board_status.record = MDSplus.Signal(cmds, None, status)

        numSampsStr = settings['getNumSamples']
        preTrig = self.getPreTrig(numSampsStr)
        postTrig = self.getPostTrig(numSampsStr)
        if self.debugging():
            print "got preTrig %d and postTrig %d\n" % (preTrig, postTrig,)
        vin1 = settings['get.vin']
        vins = eval('MDSplus.makeArray([%s,])' % (vin1,))

        if self.debugging():
            print "got the vins "
            print vins
        self.ranges.record = vins
        chanMask = settings['getChannelMask'].split('=')[-1]
        if self.debugging():
            print "chan_mask = %s\n" % (chanMask,)
        clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        if self.debugging():
            print "clock_src = %s\n" % (clock_src,)
        if clock_src == 'INT_CLOCK' :
            intClock = float(settings['getInternalClock'].split()[1])
            delta=1./float(intClock)
            self.clock.record = MDSplus.Range(None, None, delta)
        else:
            self.clock.record = self.clock_src

        clock = self.clock.record
#
# now store each channel
#
        for chan in range(16):
            self.storeChannel(chan, chanMask, preTrig, postTrig, clock, vins)

        self.dataSocketDone()
        return 1

    STORE=store
