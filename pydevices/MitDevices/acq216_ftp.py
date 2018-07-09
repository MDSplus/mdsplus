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

from acq_ftp import ACQ_FTP

from tempfile import *
import time
import os
import numpy
import array

import MDSplus

class ACQ216_FTP(ACQ_FTP):
    """
    D-Tacq ACQ216  16 channel transient recorder
    
    """

    from copy import copy
    parts=copy(ACQ_FTP.acq_parts)

    for i in range(16):
	parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
	parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
	parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
	parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
	parts.append({'path':':INPUT_%2.2d:VIN'%(i+1,),'type':'NUMERIC', 'value':10, 'options':('no_write_shot')})
    del i
    parts.extend(ACQ_FTP.clock_parts)

    def initftp(self, arg):
        """
        Initialize the device
        Send parameters
        Arm hardware
        """
        start=time.time()
        msg=None

	try:
	    path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            msg="Must specify active chans as int in (2,4,8,16)"

            active_chan = int(self.active_chan)
            msg=None
            if active_chan not in (2,4,8,16) :
                print "active chans must be in (2, 4, 8, 16 )"
                active_chan = 16
            msg="Could not read trigger source"
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
            msg="Could not read clock source"
            clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
            msg=None
            clock_out=self.clock_out.record.getOriginalPartName().getString()[1:]
            msg="Must specify pre trigger samples"
            pre_trig=int(self.pre_trig.data()*1024)
            msg="Must specify post trigger samples"
            post_trig=int(self.post_trig.data()*1024)
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

#
# now create the post_shot ftp command file
#
            fname = "/home/mdsftp/scratch/%s_%s_%s.sh" % (tree, shot, path)
            fd=open(fname, 'w')
            for i in range(6):
                line = 'D%1.1d' % i
                try:
		    wire = str(self.__getattr__('di%1.1d_wire' %i).record)
                    if wire not in self.wires :
                        print "DI%d:wire must be in %s" % (i, str(self.wires), )
                        wire = 'fpga'
                except:
                    wire = 'fpga'
                try:
                    bus = str(self.__getattr__('di%1.1d_bus' % i).record)
                    if bus not in self.wires :
                        print "DI%d:bus must be in %s" % (i, str(self.wires),)
                        bus = ''
                except:
                    bus = ''
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))

            fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
            if clock_src == 'INT_CLOCK':
		if clock_out == None:
		    if self.debugging:
			print "internal clock no clock out\n"
                    fd.write("acqcmd setInternalClock %d\n" % clock_freq)
		else:
		    clock_out_num_str = clock_out[-1]
		    clock_out_num = int(clock_out_num_str)
		    setDIOcmd = 'acqcmd -- setDIO '+'-'*clock_out_num+'1'+'-'*(6-clock_out_num)+'\n'
                    if self.debugging:
                        print "internal clock clock out is %s setDIOcmd = %s\n" % (clock_out, setDIOcmd,)
		    fd.write("acqcmd setInternalClock %d DO%s\n" % (clock_freq, clock_out_num_str,))
		    fd.write(setDIOcmd)		
            else:
                if (clock_div != 1) :
                    fd.write("acqcmd setExternalClock %s %d DO2\n" % (clock_src, clock_div,))
                else:
                    fd.write("acqcmd setExternalClock %s\n" % clock_src)

            for chan in range(16):
                fd.write("set.vin %d %d\n" % (chan+1, int(self.__getattr__('input_%2.2d_vin' % (chan+1,)))))

            fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig,post_trig,trig_src,'rising',))
            fd.write(". /usr/local/bin/xmlfunctions.sh\n")
            fd.write("settingsf=/tmp/%s_%s_%s.xml\n"%(tree, shot, path,))

	    cmds = self.status_cmds.record
	    for cmd in cmds:
		cmd = cmd.strip()
		if self.debugging():
		    print 'adding /xmlcmd "%s" >> $settingsf/ to the file.\n'%(cmd,)
		fd.write('xmlcmd "%s" >> $settingsf\n'%(cmd,))

            fd.flush()
	    fd.close()

	    print "Time to make init file = %g\n" % (time.time()-start)
	    start=time.time()
            self.doInit(tree,shot,path)
	    print "Time for board to init = %g\n" % (time.time()-start)
            return  1

        except Exception,e:
            print "%s\n" % (str(e),)
            return 0

    INITFTP=initftp
        
    def storeftp(self, arg):

        try:
            from xml.marshal.generic import load
        except:
            print "you must install PyXML to use this deprecated device.  Please switch to acq216 device type"

 	path = self.local_path
        tree = self.local_tree
        shot = self.tree.shot
        CPCIDataDir = os.getenv('CPCI_DATA_DIR')
	if not CPCIDataDir:
	    raise 'CPCI_DATA_DIR environment variable must be defined'
        dataDir="%s/%s/%s/%s"%(CPCIDataDir, tree, shot, path,)
        try :
	    settingsf = open("%s/settings.xml"%(dataDir,), "r")
	except :
	    raise Exception,"Could not open Settings file %s/settings.xml"%(dataDir,)
        try :
            settings = load(settingsf)
        except:
            settingsf.close()
	    raise Exception, "Could not parse XML settings"
        settingsf.close()
	if self.debugging() :
	    print "xml is loaded\n"
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
        vins = MDSplus.makeArray(numpy.array(settings['get.vin'].split(',')).astype('float'))
        if self.debugging:
            print "got the vins "
            print vins
	self.ranges.record = MDSplus.makeArray(numpy.array(settings['get.vin'].split(',')).astype('float'))
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
		    chanFileName="%s/%2.2d"%(dataDir, chan+1,)
                    if self.debugging():
			print "about to readRawData(%s, preTrig=%d, start=%d, end=%d, inc=%d)" % (chanFileName, preTrig, start, end, inc)
                    buf = self.readRawData(chanFileName, preTrig, start, end, inc)
		    if inc == 1:
			dim = MDSplus.Dimension(MDSplus.Window(start, end, self.trig_src ), clock)
                    else:
			dim = MDSplus.Data.compile('Map($,$)', MDSplus.Dimension(MDSplus.Window(start/inc, end/inc, self.trig_src), clock), MDSplus.Range(start, end, inc))
		    dat = MDSplus.Data.compile(
                        '_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)',
                        vins[chan*2], vins[chan*2+1], buf,dim) 
                    exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
	return 1

    STOREFTP=storeftp
