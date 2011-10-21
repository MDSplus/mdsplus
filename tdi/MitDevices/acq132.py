from acq import ACQ

import tempfile
import time
import numpy
import array

import MDSplus

class ACQ132(ACQ):
    """
    D-Tacq ACQ132  32 channel transient recorder

    device support for d-tacq acq132 http://www.d-tacq.com/acq132cpci.shtml 
    """
    from copy import copy
    parts=copy(ACQ.acq_parts)

    for i in range(32):
	parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
	parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
	parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
	parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
    del i
    parts.extend(ACQ.action_parts)
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
        start=time.time()
        msg=None

	try:
            if self.debugging():
                print "starting init\n";
	    path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            msg="Must specify active chans as int in (8,16,32)"

            active_chan = int(self.active_chan)
            msg=None
            if active_chan not in (8,16,32) :
                print "active chans must be in (8, 16, 32 )"
                active_chan = 32
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
                msg="Must specify clock frequency even for external clocks"
                clock_freq = int(self.clock_freq)
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
            host = self.getMyIp()
            fd.write("acqcmd setAbort\n")
	    fd.write("host=%s\n"%(host,))
            fd.write("tree=%s\n"%(tree,))
            fd.write("shot=%s\n"%(shot,))
            fd.write("path='%s'\n"%(path,))
            fd.write("rm -f /tmp/ready\n")

            for i in range(6):
                line = 'D%1.1d' % i
                try:
		    wire = str(self.__getattr__('di%1.1d_wire' %i).record)
                    if self.debugging():
                        print "wire is %s\n" % (wire,)
                    if wire not in self.wires :
#                        print "DI%d:wire must be in %s" % (i, str(self.wires), )
                        wire = 'fpga'
                except Exception,e:
                    if self.debugging():
                        print "error retrieving wire %s\n" %(e,)
                    wire = 'fpga'
                try:
                    bus = str(self.__getattr__('di%1.1d_bus' % i).record)
                    if self.debugging():
                        print "bus is %s\n" % (bus,)
                    if bus not in self.wires :
                        print "DI%d:bus must be in %s" % (i, str(self.wires),)
                        bus = ''
                except Exception,e:
                    if self.debugging():
                        print "error retrieving bus %s\n" %(e,)
                    bus = ''
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))
                if self.debugging():
                    print "set.route %s in %s out %s\n" %(line, wire, bus,)
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
                fd.write("acqcmd -- setExternalClock --fin %d --fout %d %s\n" % (clock_freq/1000, clock_freq/1000*clock_div, clock_out,))

            if self.debugging():
                print "clock all set now move on to pre-post\n"
                print "pre trig = %d\n" % (pre_trig,)
                print "post trig = %d\n" % (post_trig,)
                print "trig_src = %s\n" % (trig_src,)
            fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig, post_trig, trig_src, 'rising',))
            if self.debugging():
                print "pre-post all set now the xml and commands\n"
            
            fd.write(". /usr/local/bin/xmlfunctions.sh\n")
            fd.write("settingsf=/tmp/settings.xml\n")
            fd.write("xmlstart > $settingsf\n")
            fd.write('xmlcmd "echo $tree" tree >> $settingsf\n')
            fd.write('xmlcmd "echo $shot" shot >> $settingsf\n')
            fd.write('xmlcmd "echo $path" path >> $settingsf\n')
            
            cmds = self.status_cmds.record
	    for cmd in cmds:
		cmd = cmd.strip()
		if self.debugging():
		    print "adding xmlcmd '%s' >> $settingsf/ to the file.\n"%(cmd,)
                fd.write("xmlcmd '%s' >> $settingsf\n"%(cmd,))
            fd.write("cat - > /etc/postshot.d/postshot.sh <<EOF\n")
            fd.write(". /usr/local/bin/xmlfunctions.sh\n")
            fd.write("settingsf=/tmp/settings.xml\n")
            fd.write("xmlacqcmd getNumSamples >> $settingsf\n")
            fd.write("xmlacqcmd getChannelMask >> $settingsf\n")
            fd.write("xmlacqcmd getInternalClock >> $settingsf\n")
            fd.write("xmlcmd date >> $settingsf\n")
            fd.write("xmlcmd hostname >> $settingsf\n")
            fd.write("xmlcmd 'sysmon -T 0' >> $settingsf\n")
            fd.write("xmlcmd 'sysmon -T 1' >> $settingsf\n")
            fd.write("xmlcmd get.channelMask >> $settingsf\n")
            fd.write("xmlcmd get.channel_mask >> $settingsf\n")
            fd.write("xmlcmd get.d-tacq.release >> $settingsf\n")
            fd.write("xmlcmd get.event0 >> $settingsf\n")
            fd.write("xmlcmd get.event1 >> $settingsf\n")
            fd.write("xmlcmd get.extClk  >> $settingsf\n")
            fd.write("xmlcmd get.ext_clk >> $settingsf\n")
            fd.write("xmlcmd get.int_clk_src >> $settingsf\n")
            fd.write("xmlcmd get.modelspec >> $settingsf\n")
            fd.write("xmlcmd get.numChannels >> $settingsf\n")
            fd.write("xmlcmd get.pulse_number >> $settingsf\n")
            fd.write("xmlcmd get.trig >> $settingsf\n")
            fd.write("xmlcmd 'get.vin 1:32'>> $settingsf\n")
            fd.write("xmlfinish >> $settingsf\n")
            fd.write("touch /tmp/ready\n")
            
            if auto_store != None :
				if self.debugging():
		    		fd.write("mdsValue 'setenv(\"\"DEBUG_DEVICES=yes\"\")'\n")
            	fd.write("mdsConnect %s\n" %host)
            	fd.write("mdsOpen %s %d\n" %(tree, shot,))
            	fd.write("mdsValue 'tcl(\"\"do /meth %s store\"\", _out)'\n" %( path, ))
            	fd.write("mdsClose\n")
            	fd.write("mdsDisconnect\n")

            fd.write("EOF\n")

            fd.flush()
            fd.seek(0,0)
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
            return ACQ.InitializationError

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
                    print "ACQ132 Error loading settings\n%s\n" %(e,)
        if settings == None :
            print "after %d tries could not load settings\n" % (tries,)
            return ACQ.SettingsNotLoaded
        
        path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot
	if self.debugging() :
	    print "xml is loaded\n"
        if tree != settings['tree'] :
            print "ACQ132 expecting tree %s got tree %s\n" % (tree, settings["tree"],)
            if arg != "nochecks" :
                return ACQ.WrongTree  # should return wrong tree error
        if path != settings['path'] :
            print "ACQ132 expecting path %s got path %s\n" % (path, settings["path"],)
            if arg != "nochecks" :
                return ACQ.WrongPath # should return wrong path error
        if shot != int(settings['shot']) :
            print "ACQ132 expecting shot %d got shot %d\n" % (shot, int(settings["shot"]),)
            if arg != "nochecks" :
                return ACQ.WrongShot # should return wrong shot error
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
            self.clock.record = self.clock_src

        clock = self.clock.record
#
# now store each channel
#
	for chan in range(32):
	    if self.debugging():
		print "working on channel %d" % chan
            chan_node = self.__getattr__('input_%2.2d' % (chan+1,))
            if chan_node.on :
                if self.debugging():
                    print "it is on so ..."
                if chanMask[chan:chan+1] == '1' :
                    try:
                        start = max(int(self.__getattr__('input_%2.2d_startidx'%(chan+1,))),-preTrig)
                    except:
                        start = -preTrig
                    try:
			end = min(int(self.__getattr__('input_%2.2d_endidx'%(chan+1,))),postTrig-1)
                    except:
                        end = postTrig-1
                    try:
                        inc = max(int(self.__getattr__('input_%2.2d_inc'%(chan+1,))),1)
                    except:
                        inc = 1
#
# could do the coeffs
#

                    if self.debugging():
			print "about to readRawData(%d, preTrig=%d, start=%d, end=%d, inc=%d)" % (chan+1, preTrig, start, end, inc)
                    try:
                        buf = self.readRawData(chan+1, preTrig, start, end, inc)
                        if self.debugging():
                            print "readRawData returned %s\n" % (type(buf),)
                        if inc == 1:
                            dim = MDSplus.Dimension(MDSplus.Window(start, end, self.trig_src ), clock)
                        else:
                            dim = MDSplus.Data.compile('Map($,$)', MDSplus.Dimension(MDSplus.Window(start/inc, end/inc, self.trig_src), clock), MDSplus.Range(start, end, inc))
                        dat = MDSplus.Data.compile(
                            '_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)',
                            vins[chan*2], vins[chan*2+1], buf,dim) 
                        exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
                    except Exception, e:
                        print "error processingig channel %d\n%s\n" %(chan+1, e,)
	self.dataSocketDone()
	return 1

    STORE=store
