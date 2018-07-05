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

from MDSplus import Device,Data,Action,Dispatch,Method, makeArray, Range, Signal, Window, Dimension

from tempfile import *
from Dt200WriteMaster import Dt200WriteMaster

from time import sleep, time
import os
import numpy
import array

class DT196A(Device):
    """
    D-Tacq ACQ196  96 channel transient recorder

    """

    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254','options':('no_write_shot',)},
        {'path':':BOARD','type':'text','value':'192.168.0.0','options':('no_write_shot',)},
        {'path':':COMMENT','type':'text'},
        ]

    for i in range(6):
        parts.append({'path':':DI%1.1d'%(i,),'type':'axis','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:WIRE'%(i,),'type':'text','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:BUS'%(i,),'type':'text','options':('no_write_shot',)})


    parts2=[
        {'path':':CLOCK_SRC','type':'text','value':'INT','options':('no_write_shot',)},
        {'path':':CLOCK_DIV','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':DAQ_MEM','type':'numeric','value':512,'options':('no_write_shot',)},
        {'path':':ACTIVE_CHAN','type':'numeric','value':96,'options':('no_write_shot',)},
        {'path':':TRIG_SRC','type':'text','value':'DI3','options':('no_write_shot',)},
        {'path':':POST_TRIG','type':'numeric','value':128,'options':('no_write_shot',)},
        {'path':':PRE_TRIG','type':'numeric','value':0,'options':('no_write_shot',)},
        {'path':':RANGES','type':'text','options':('write_once',)},
        {'path':':STATUS_CMDS','type':'text','value':makeArray(['cat /proc/cmdline', 'get.d-tacq.release']),'options':('no_write_shot',)},
        {'path':':BOARD_STATUS','type':'signal','options':('write_shot',)},
        ]
    parts.extend(parts2)
    del parts2
    for i in range(96):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:FILTER_COEFS'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:RAW'%(i+1,),'type':'SIGNAL', 'options':('no_write_model','write_once')})

    parts.append({'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INITFTP',head))",
                  'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'WAITFTP',head))",
                  'options':('no_write_shot',)})

    trig_sources=[ 'DI0',
                   'DI1',
                   'DI2',
                   'DI3',
                   'DI4',
                   'DI5',
                   ]
    clock_sources = trig_sources
    clock_sources.append('INT')
    clock_sources.append('MASTER')

    wires = [ 'fpga','mezz','rio','pxi','lemo', 'none', 'fpga pxi', ' ']

    del i
    def getPreTrig(self,str) :
	parts = str.split('=')
        pre_trig = int(parts[2].split(' ')[0])
	return pre_trig

    def getPostTrig(self,str) :
        parts = str.split('=')
        post_trig = int(parts[3].split(' ')[0])
        return post_trig

    def getBoardIp(self):
        try:
            boardip=str(self.node.record)
            if len(boardip) == 0 :
                raise Exception, "boardid record empty"
        except:
            try:
                print "trying to use the hub to get the ip"
                boardip=Dt200WriteMaster(int(self.board), "/sbin/ifconfig eth0 | grep 'inet addr'", 1)[0].split(':')[1].split()[0]
            except:
                raise Exception, "could not get board ip from either tree or hub"
        return boardip

    def readRawData(self, name, pre, start, end, inc) :
#        print "readRawData(self, %s, %d, %d, %d, %d)" % (name, pre, start, end, inc)
        f = open(name, mode='rb')
        try:
            f.seek((pre+start)*2)
            binValues = array.array('H')
	    binValues.read(f,end-start+1)
            ans = numpy.array(binValues, dtype=numpy.int16)
	    if inc > 1 :
		asns = ans[::inc]
            f.close()
        except Exception,e :
	   print "readRawData - %s" % e
           raise e
        return ans

    def check(self, expression, message):
        try:
            ans = eval(expression)
            return ans
        except:
            raise Exception, message

    def timeoutHandler(self,sig,stack):
        raise Exception("Timeout occurred")

    def getState(self):
        """Get the current state"""
        import socket,signal
        s=socket.socket()
        state="Unknown"
        try:
            signal.signal(signal.SIGALRM,self.timeoutHandler)
            signal.alarm(60)
            s.connect((self.getBoardIp(),54545))
            state=s.recv(100)[0:-1]
        except Exception,e:
            print "Error getting board state: %s" % (str(e),)
        signal.alarm(0)
        s.close()
        return state

    def doInit(self,tree,shot,path):
        """Get the current state"""
        import socket,signal
        status=1
        s=socket.socket()
        try:
            signal.signal(signal.SIGALRM,self.timeoutHandler)
            signal.alarm(15)
            s.connect((self.getBoardIp(),54546))
            s.send("%s %d %s" % (tree,shot,path))
        except Exception,e:
            status=0
            print "Error sending doInit: %s" % (str(e),)
        signal.alarm(0)
        s.close()
        return status

    def initftp(self, arg):
        """
        Initialize the device
        Send parameters
        Arm hardware
        """
        start=time()
        msg=None
        debug=os.getenv("DEBUG_DEVICES")

	try:
	    path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            msg="Must specify active chans as int in (32,64,96)"
            #active_chans = self.check("int(self.active_chans)", "Must specify active chans as int in (32,64,96)")
            active_chan = int(self.active_chan)
            msg=None
            if active_chan not in (32,64,96) :
                print "active chans must be in (32, 64, 96 )"
                active_chan = 96
            msg="Could not read trigger source"
            #trig_src=self.check("str(self.trig_src.record)", "Could not read trigger source")
            trig_src=str(self.trig_src.record)
            msg=None
            if not trig_src in self.trig_sources:
                raise Exception, "Trig_src must be in %s" % str(self.trig_sources)
            msg="Could not read clock source"
            #clock_src=self.check("str(self.clock_src.record)", "Could not read clock source")
            clock_src=str(self.clock_src.record)
            msg=None
            if not clock_src in self.clock_sources:
                raise Exception, "clock_src must be in %s" % str(self.clock_sources)
            msg="Must specify pre trigger samples"
            #pre_trig=self.check('int(self.pre_trig.data()*1024)', "Must specify pre trigger samples")
            pre_trig=int(self.pre_trig.data()*1024)
            msg="Must specify post trigger samples"
            #post_trig=self.check('int(self.post_trig.data()*1024)', "Must specify post trigger samples")
            post_trig=int(self.post_trig.data()*1024)
            msg=None
            if clock_src == "INT" or clock_src == "MASTER":
                msg="Must specify clock frequency in clock_div node for internal clock"
		#clock_freq = self.check('int(self.clock_div)', "Must specify clock frequency in clock_div node for internal clock")
                clock_freq = int(self.clock_div)
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
                line = 'd%1.1d' % i
                try:
                    #wire = eval('str(self.di%1.1d_wire.record)' %i)
		    wire = str(self.__getattr__('di%1.1d_wire' %i).record)
                    if wire not in self.wires :
                        print "DI%d:wire must be in %s" % (i, str(self.wires), )
                        wire = 'fpga'
                except:
                    wire = 'fpga'
                try:
                    #bus = eval('str(self.di%1.1d_bus.record)' % i)
                    bus = str(self.__getattr__('di%1.1d_bus' % i).record)
                    if bus not in self.wires :
                        print "DI%d:bus must be in %s" % (i, str(self.wires),)
                        bus = ''
                except:
                    bus = ''
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))

            fd.write("acqcmd  setChannelMask " + '1' * active_chan+"\n")
            if clock_src == 'INT' or clock_src == 'MASTER' :
                fd.write("acqcmd setInternalClock %d\n" % clock_freq)
                if clock_src == 'MASTER' :
                    fd.write("acqcmd '-- setDIO -1-----'\n")
# NOTE existing modules may have clock_div filled in with the desired freq even
#      though it is external clock.
#      for now IGNORE
#
#            else:
#                if (clock_div != 1) :
#                    fd.write("acqcmd setExternalClock %s %d DO2\n" % (clock_src, clock_div,))
#
# the following is not generic
# the clock is output on d2 and comes from DI0
#
                    fd.write("set.route d2 in fpga out pxi\n")
                    fd.write("acqcmd '-- setDIO --1-----'\n")
                    fd.write("set.ext_clk DI0\n")
            else :
                fd.write("acqcmd setExternalClock %s\n" % clock_src)

            fd.write("set.pre_post_mode %d %d %s %s\n" %(pre_trig,post_trig,trig_src,'rising',))
            fd.write(". /usr/local/bin/xmlfunctions.sh\n")
            fd.write("settingsf=/tmp/%s_%s_%s.xml\n"%(tree, shot, path,))

	    cmds = self.status_cmds.record
	    for cmd in cmds:
		cmd = cmd.strip()
		if debug:
		    print 'adding /xmlcmd "%s" >> $settingsf/ to the file.\n'%(cmd,)
		fd.write('xmlcmd "%s" >> $settingsf\n'%(cmd,))

            fd.flush()
	    fd.close()

	    print "Time to make init file = %g\n" % (time()-start)
	    start=time()

            self.doInit(tree,shot,path)
	    print "Time for board to init = %g\n" % (time()-start)
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

        debug=os.getenv("DEBUG_DEVICES")

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
	if debug :
	    print "xml is loaded\n"
        status = []
        cmds = self.status_cmds.record
        for cmd in cmds:
	    cmd = cmd.strip()
	    if debug:
		print "about to append answer for /%s/\n" % (cmd,)
		print "   which is /%s/\n" %(settings[cmd],)
	    status.append(settings[cmd])
	    if debug:
		print "%s returned %s\n" % (cmd, settings[cmd],)
	if debug:
	    print "about to write board_status signal"
	self.board_status.record = Signal(cmds, None, status)

        numSampsStr = settings['getNumSamples']
	preTrig = self.getPreTrig(numSampsStr)
        postTrig = self.getPostTrig(numSampsStr)
        vins = makeArray(numpy.array(settings['get.vin'].split(',')).astype('float'))
	self.ranges.record = vins
        chanMask = settings['getChannelMask'].split('=')[-1]
	if self.clock_src.record.lower() == 'int' or self.clock_src.record.lower() == 'master':
	    #intClkStr=settings['getInternalClock'].split()[0].split('=')[1]
            #intClock=int(intClikStr)
	    intClock = float(settings['getInternalClock'].split()[1])
            delta=1./float(intClock)
	else:
	    delta = 0

        trig_src = self.__getattr__(str(self.trig_src.record).lower())
#
# now store each channel
#
	for chan in range(96):
	    if debug:
		print "working on channel %d" % chan
	    #chan_node = eval('self.input_%2.2d' % (chan+1,))
            chan_node = self.__getattr__('input_%2.2d' % (chan+1,))
            if chan_node.on :
                if debug:
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
                    buf = self.readRawData(chanFileName, preTrig, start, end, inc)
		    if delta != 0 :
			axis = Range(None, None, delta/inc)
		    else:
			#clockExpr = 'self.%s'% str(self.clock_src.record)
			#clock_src = eval(clockExpr.lower())
                        clock_src = self.__getattr__(str(self.clock_src.record).lower())
                        axis = clock_src

		    if inc == 1:
			dim = Dimension(Window(start, end, trig_src ), axis)
                    else:
			dim = Data.compile('Map($,$)', Dimension(Window(start/inc, end/inc, trig_src), axis), Range(start, end, inc))
#                    dat = Data.compile('build_signal(build_with_units( $*(0. + $value), "V") ,build_with_units($,"Counts"),$)', coefficent, buf,dim)
		    dat = Data.compile('_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)', vins[chan*2], vins[chan*2+1], buf,dim)
                    exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
	return 1

    STOREFTP=storeftp

    def waitftp(self, arg) :
        """Wait for board to finish digitizing and ftp'ing data to host"""
        state = self.getState()
        if state == 'ARMED' or state == 'RUN':
            return 662470754
            raise Exception, "device Not triggered"
        for chan in range(int(self.active_chan), 0, -1):
            chan_node = self.__getattr__('input_%2.2d' % (chan,))
            if chan_node.on :
                max_chan = chan_node
		break
        tries = 0
        while tries < 60 :
            if max_chan.rlength > 0:
                break
            sleep(3)
            tries = tries+1
        if tries == 60:
            print "Triggered, but data not stored !"
            return 0

        return 1
    WAITFTP=waitftp
