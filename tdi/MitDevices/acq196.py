import numpy
import array
import MDSplus
import acq

class ACQ196(acq.ACQ):
    """
    D-Tacq ACQ196  96 channel transient recorder

    device support for d-tacq acq196 http://www.d-tacq.com/acq196cpci.shtml     
    """
    from copy import copy
    parts=copy(acq.ACQ.acq_parts)

    for i in range(96):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
    del i
    parts.extend(acq.ACQ.action_parts)
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
        start=time.time()
        msg=None

        try:
            if self.debugging():
                print "starting init\n";
            path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            if self.debugging():
               print 'ACQ196 initftp path = %s tree = %s shot = %d\n' % (path, tree, shot)
            msg="Must specify active chans as int in (32,64,96)"

            active_chan = int(self.active_chan)
            msg=None
            if active_chan not in (32,64,96) :
                print "active chans must be in (32, 64, 96 )"
                active_chan = 96
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
                    fd.write("acqcmd setExternalClock %s %d\n" % (clock_src, clock_div,))
#
# set the channel mask 2 times
#
            fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
#
#  set the pre_post mode last
#
            fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig, post_trig, trig_src, 'rising',))
            
            self.addGenericXMLStuff(fd)

            fd.write("xmlcmd 'get.vin 1:32'>> $settingsf\n")
            fd.write("xmlcmd 'get.vin 33:64'>> $settingsf\n")
            fd.write("xmlcmd 'get.vin 65:96'>> $settingsf\n")
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

        if not self.triggered():
            print "ACQ196 Device not triggered\n"
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
                    print "ACQ196 Error loading settings\n%s\n" %(e,)
        if settings == None :
            print "after %d tries could not load settings\n" % (tries,)
            return acq.ACQ.SettingsNotLoaded
        
        path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot
        if self.debugging() :
            print "xml is loaded\n"
        if tree != settings['tree'] :
            print "ACQ196 open tree is %s board armed with tree %s\n" % (tree, settings["tree"],)
            if arg != "nochecks" :
                return acq.ACQ.WrongTree
        if path != settings['path'] :
            print "ACQ196 device tree path %s, board armed with path %s\n" % (path, settings["path"],)
            if arg != "nochecks" :
                return acq.ACQ.WrongPath
        if shot != int(settings['shot']) :
            print "ACQ196 open shot is %d, board armed with shot %d\n" % (shot, int(settings["shot"]),)
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
        vin1 = settings['get.vin 1:32']
        vin2 = settings['get.vin 33:64']
        vin3 = settings['get.vin 65:96']
        active_chan = int(self.active_chan)
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
	    if self.debugging():
		print "it is external clock\n"
	    try:
		clock_div = int(self.clock_div)
	    except:
		clock_div = 1
	    if self.debugging():
		print "clock div is %d\n" % (clock_div,)
	    if clock_div == 1 :
        	self.clock.record = self.clock_src
	    else:
		if self.debugging():
		    print "external clock with divider %d  clock source is %s\n" % ( clock_div, clock_src,)
		clk = self.clock_src
		try : 
		    while type(clk) != MDSplus.compound.Range :
			clk = clk.record
		    if self.debugging():
			print "I found the Range record - now writing the clock with the divide\n"
                    self.clock.record = MDSplus.Range(clk.getBegin(), clk.getEnding(), clk.getDelta()*clock_div)
		except:
		    print "could not find Range record for clock to construct divided clock storing it as undivided\n"
		    self.clock.record  = clock_src
		if self.debugging():
		    print "divided clock stored\n"

#        clock = self.clock.record
        clock = self.clock
#
# now store each channel
#
        for chan in range(96):
            self.storeChannel(chan, chanMask, preTrig, postTrig, clock, vins)

        self.dataSocketDone()
        return 1

    STORE=store
