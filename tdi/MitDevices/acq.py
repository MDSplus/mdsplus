import os
import numpy
import array
import ftplib
import tempfile
import socket
from xml.marshal.generic import dumps, loads, load 

import MDSplus
import acq200, transport


class ACQ(MDSplus.Device):
    """
    Abstract class to subclass the d-tacq acqxxx device types.  Contains
    members and methods that all of the acqxxx  devices share:


    members:
      parts - the shared parts.  This should have the channels and the actions appended to it
      actions - the actions.  This gets appended after the channels

    methods:
      debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    
    """
    
    acq_parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254'},
        {'path':':COMMENT','type':'text'},
        ]

    for i in range(6):
        acq_parts.append({'path':':DI%1.1d'%(i,),'type':'axis','options':('no_write_shot',)})
        acq_parts.append({'path':':DI%1.1d:WIRE'%(i,),'type':'text','options':('no_write_shot',)})
        acq_parts.append({'path':':DI%1.1d:BUS'%(i,),'type':'text','options':('no_write_shot',)})


    parts2=[
        {'path':':INT_CLOCK','type':'axis','options':('no_write_model','write_once')},       
        {'path':':TRIG_SRC','type':'numeric','valueExpr':'head.di3','options':('no_write_shot',)},
        {'path':':TRIG_EDGE','type':'text','value':'rising','options':('no_write_shot',)},
        {'path':':CLOCK_SRC','type':'numeric','valueExpr':'head.int_clock','options':('no_write_shot',)},
        {'path':':CLOCK_DIV','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':CLOCK_EDGE','type':'text','value':'rising','options':('no_write_shot',)},
        {'path':':CLOCK_FREQ','type':'numeric','value':1000000,'options':('no_write_shot',)},
        {'path':':CLOCK_OUT','type':'numeric','valueExpr':'head.di2','options':('no_write_shot',)},

        {'path':':DAQ_MEM','type':'numeric','value':512,'options':('no_write_shot',)},
        {'path':':ACTIVE_CHAN','type':'numeric','value':16,'options':('no_write_shot',)},
        {'path':':POST_TRIG','type':'numeric','value':128,'options':('no_write_shot',)},
        {'path':':PRE_TRIG','type':'numeric','value':0,'options':('no_write_shot',)},
        {'path':':RANGES','type':'numeric','options':('write_once','no_write_model',)},
        {'path':':STATUS_CMDS','type':'text','value':MDSplus.makeArray(['cat /proc/cmdline', 'get.d-tacq.release']),'options':('no_write_shot',)},
        {'path':':BOARD_STATUS','type':'signal','options':('write_shot',)},
        {'path':':CLOCK','type':'axis','options':('no_write_model','write_once')},
        ]
    acq_parts.extend(parts2)
    del parts2

    action_parts=[
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INITFTP',head,'auto'))",
         'options':('no_write_shot',)},
        {'path':':STORE_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'WAITFTP',head))",
         'options':('no_write_shot',)}]

#
#  ACQ specific error codes
#  defined in includes/mitdevices_msg.h
#
    InitializationError = 0x277CA592
    SettingsNotLoaded = 0x277CA59A
    WrongTree = 0x277CA5A2
    WrongPath = 0x277CA5AA
    WrongShot = 0x277CA5B2

    debug=None
    data_socket = -1

    wires = [ 'fpga','mezz','rio','pxi','lemo', 'none', 'fpga pxi', ' ']
    

    def debugging(self):
	if self.debug == None :
            self.debug=os.getenv("DEBUG_DEVICES")
	return(self.debug)

    def getPreTrig(self,str) :
	parts = str.split('=')
        pre_trig = int(parts[2].split(' ')[0])
	return pre_trig

    def getPostTrig(self,str) :
        parts = str.split('=')
        post_trig = int(parts[3].split(' ')[0])
        return post_trig

    def getBoardIp(self):
        boardip=str(self.node.record)
        if len(boardip) == 0 :
            raise Exception, "boardid record empty"
        return boardip

    def dataSocketDone(self):
	self.data_socket.send("done\n");
	self.data_socket.shutdown(socket.SHUT_RDWR)
	self.data_socket.close()
	self.data_socket=-1

    def connectAndFlushData(self):
        import select
	import time
        if self.data_socket == -1 :
            self.data_socket = socket.socket()
            self.data_socket.connect((self.getBoardIp(), 54547))
        rr, rw, ie = select.select([self.data_socket], [], [], 0)
        if len(rr) > 0 :
            if self.debugging():
                print "flushing old data from socket"
            dummy = self.data_socket.recv(99999, socket.MSG_DONTWAIT)
#            self.data_socket.close()
#            self.data_socket = -1
	    time.sleep(.01)
            self.connectAndFlushData()

    def readRawData(self, chan, pre, start, end, inc, retrying) :
	if self.debugging():
	    print " starting readRawData(chan=%d, pre=%d. start=%d, end=%d, inc=%d)\n" %(chan,pre, start, end, inc,)
	try:
            self.connectAndFlushData()
	    self.data_socket.settimeout(10.)
#            self.data_socket.send("/dev/acq200/data/%02d %d\n" % (chan,end,))
            self.data_socket.send("/dev/acq200/data/%02d\n" % (chan,))
	    f=self.data_socket.makefile("r",32768)
            bytes_to_read = 2*(end+pre+1)
	    buf = f.read(bytes_to_read)
            binValues = array.array('h')
            binValues.fromstring(buf)
            ans = numpy.ndarray(buffer=binValues,
                                dtype=numpy.int16,
                                offset=(pre+start)*2,
                                strides=(inc*2),
                                shape=((end-start+1)/inc))

	except socket.timeout, e:
	    if not retrying :
                print "Timeout - closing socket and retrying\n"
                self.data_socket.shutdown(socket.SHUT_RDWR)
                self.data_socket.close()
	        self.data_socket=-1
	        return self.readRawData(chan, pre, start, end, inc, True)
	    else:
		 raise Exception(e)
	except Exception, e:
	    print "ACQ error reading channel %d\n, %s\n" % (chan, e,)
	    raise e
	if self.debugging():
	    print "Read Raw data pre=%d start=%d end = %d inc=%d returning len = %d" % (pre, start, end, inc, len(ans),)
        return ans

    def triggered(self):
        import time
        complete = 0
        tries = 0
        while not complete and tries < 120 :
            state = self.getBoardState()
            if self.debugging():
                print "get state returned %s\n" % (state,)
            if state == "ACQ32:4 ST_POSTPROCESS" :
                tries +=1
                time.sleep(1)
            else:
                complete=1
        state = self.getBoardState()
        if self.debugging():
            print "get state after loop returned %s\n" % (state,)
        if state != "Ready" :
            print "ACQ196 device not triggered /%s/\n"% (self.getBoardState(),)
            return 0  
        return 1

    def getBoardState(self):
        """Get the current state"""
        import socket,time
	for tries in range(5):
            s=socket.socket()
	    s.settimeout(5.) 
            state="Unknown"
            try:
                s.connect((self.getBoardIp(),54545))
                state=s.recv(100)[0:-1]
	        if self.debugging():
		    print "getBoardState  returning /%s/\n" % (state,)
                s.close()
                return state
            except Exception,e:
                print "Error getting board state: %s" % (str(e),)
	        state = "off-line"
                s.close()
            time.sleep(3)
                  
    def getMyIp(self):
        import socket

        s=socket.socket()
	try:
            s.connect((self.getBoardIp(),54545))
            hostip = s.getsockname()[0]
            state=s.recv(100)[0:-1]
            if self.debugging():
                print "getMyIp  read /%s/\n" % (state,)
	except Exception,e:
	    hostip = ""
	    print "could not connect to board to verify local IP address\n%s\n" %(e,)
        s.close()
	return hostip

    def addGenericXMLStuff(self, fd):
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

    def finishXMLStuff(self, fd, auto_store):
        fd.write("xmlfinish >> $settingsf\n")
        fd.write("touch /tmp/ready\n")
            
        if auto_store != None :
            if self.debugging():
                fd.write("mdsValue 'setenv(\"\"DEBUG_DEVICES=yes\"\")'\n")
            fd.write("touch /tmp/ready\n")
            fd.write("mdsConnect %s\n" %self.getMyIp())
            fd.write("mdsOpen %s %d\n" %(self.local_tree, self.tree.shot,))
            fd.write("mdsValue 'tcl(\"\"do /meth %s store\"\", _out),_out'\n" %( self.local_path, ))
            if self.debugging():
                fd.write("mdsValue 'write(*,_out)'\n")
            fd.write("mdsClose\n")
            fd.write("mdsDisconnect\n")
            fd.write("rm /tmp/ready\n")

        fd.write("EOF\n")
        fd.write("chmod a+x /etc/postshot.d/postshot.sh\n")
        fd.flush()
        fd.seek(0,0)

    def storeChannel(self, chan, chanMask, preTrig, postTrig, clock, vins):
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
                    buf = self.readRawData(chan+1, preTrig, start, end, inc, False)
                    if self.debugging():
                        print "readRawData returned %s\n" % (type(buf),)
                    if inc == 1:
                        dim = MDSplus.Dimension(MDSplus.Window(start, end, self.trig_src ), clock)
                    else:
                        dim = MDSplus.Dimension(MDSplus.Window(start/inc, end/inc, self.trig_src ), MDSplus.Range(None, None, clock.evaluate().getDelta()*inc))
                    dat = MDSplus.Data.compile(
                        'build_signal(build_with_units((($1+ ($2-$1)*($value - -32768)/(32767 - -32768 ))), "V") ,build_with_units($3,"Counts"),$4)',
                        vins[chan*2], vins[chan*2+1], buf,dim) 
                    exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
                except Exception, e:
                    print "error processing channel %d\n%s\n" %(chan+1, e,)

    def startInitializationFile(self, fd, trig_src, pre_trig, post_trig):
        host = self.getMyIp()
        fd.write("acqcmd setAbort\n")
        fd.write("host=%s\n"%(host,))
        fd.write("tree=%s\n"%(self.local_tree,))
        fd.write("shot=%s\n"%(self.tree.shot,))
        fd.write("path='%s'\n"%(self.local_path,))
        fd.write("rm -f /tmp/ready\n")

        for i in range(6):
            line = 'D%1.1d' % i
            try:
                wire = str(self.__getattr__('di%1.1d_wire' %i).record)
                if self.debugging():
                    print "wire is %s\n" % (wire,)
                if wire not in self.wires :
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
            if wire != 'fpga' and bus != '' :
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))
                if self.debugging():
                    print "set.route %s in %s out %s\n" %(line, wire, bus,)

    def loadSettings(self):
        settingsfd = tempfile.TemporaryFile()
        ftp = ftplib.FTP(self.getBoardIp())
        ftp.login('dt100', 'dt100')
        ftp.retrlines("RETR /tmp/settings.xml", lambda s, w=settingsfd.write: w(s+"\n"))
        settingsfd.seek(0,0)
        if self.debugging():
            print "got the settings\n"
            
        try :
            settings = load(settingsfd)
        except Exception,e:
            settingsfd.close()
	    raise Exception, "Could not parse XML settings\n%s" %(e,)
        settingsfd.close()
        if self.debugging():
            print "The settings are loaded\n"
        return settings
    
    def doInit(self,fd):
        """Tell the board to arm"""
        import socket
        if self.debugging():
	    print "starting doInit"
        status=1
        try:
            ftp = ftplib.FTP(self.getBoardIp())
            ftp.login('dt100','dt100')
            ftp.storlines("STOR /tmp/initialize",fd)
        except Exception,e:
            print "Error sending arm commands via ftp to %s\n%s\n" % (self.getBoardIp(), e,)
            return
        s=socket.socket()
        try:
            s.settimeout(10.)
            s.connect((self.getBoardIp(),54546))
            s.send("initialize")
        except Exception,e:
            status=0
            print "Error sending doInit: %s" % (str(e),)
        s.close()
        if self.debugging():
            print "finishing doInit"
        return status

    def waitftp(self, arg) :
        import time
        """Wait for board to finish digitizing and storing the data"""
        state = self.getBoardState()
	tries = 0
	while (state == "ACQ32:4 ST_POSTPROCESS" or state == "Ready" ) and tries < 90 :
	    tries = tries + 1
	    state = self.getBoardState()
	    time.sleep(2)

	if state == 'off-line' :
	    return 662470754  # device not triggered - change to offline
	if state == "ACQ32:1 ST_ARM" or state == "ACQ32:2 ST_RUN" :
            return 662470754  # device not triggered - change to offline
	if state == "ACQ32:4 ST_POSTPROCESS" :
            return 662470754  # device not triggered - change to stuck
	if state == "ACQ32:0 ST_STOP" or state == "Ready":
            for chan in range(int(self.active_chan), 0, -1):
                chan_node = self.__getattr__('input_%2.2d' % (chan,))
                if chan_node.on :
                    max_chan = chan_node
                    break

            if max_chan.rlength > 0:
		return 1
	    else:
		print "Triggered, but data not stored !"
                return 0  # should be triggered but not stored
	else:
	    print "ACQxxx UNKNOWN BOARD state /%s/\n" % (state,)
	    return 662470754  # device not triggered - change to unknown state

    WAITFTP=waitftp

    def trigger(self, arg):
        if self.debugging():
            print "starting trigger"
        boardip = self.getBoardIp()
        try:
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
        except:
            print "could not read trigger source"
            return 0
        if self.debugging() :
            print "executing trigger on board %s, trig_src is %s."% (boardip, trig_src,)
        trig_src = trig_src[2:]
        
        try:
            UUT = acq200.Acq200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            route = UUT.uut.acq2sh('get.route D%s'%(trig_src,))
            d1 = UUT.uut.acq2sh('set.route d%s in fpga out'%(trig_src,))
            d2 = UUT.uut.acq2sh('set.dtacq dio_bit %s P'%(trig_src,))
            d3 = UUT.uut.acq2sh('set.route %s' %(route,))
            d4 = UUT.uut.acq2sh('set.dtacq dio_bit %s -'%(trig_src,))
            
        except:
            print "could not send command to the board"
        if self.debugging():
            print "got back: %s\n" % (route,)
	    print "     and: %s\n" % (d1,)
	    print "     and: %s\n" % (d2,)
	    print "     and: %s\n" % (d3,)
	    print "     and: %s\n" % (d4,)
        return 1
    TRIGGER=trigger

    def acqcmd(self, arg):
        boardip = self.getBoardIp()
        
        try:
            UUT = acq200.Acq200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            a = UUT.uut.acqcmd(str(arg))
        except:
            print "could not send %s to the board" %(str(arg),)
            return 0
        print "%s  %s -> %s"%(boardip, arg, a,)
        return 1
    ACQCMD=acqcmd

    def acq2sh(self, arg):
        boardip = self.getBoardIp()
        
        try:
            UUT = acq200.Acq200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            a = UUT.uut.acq2sh(str(arg))
        except:
            print "could not send %s to the board" %(str(arg),)
            return 0
        print "%s  %s -> %s"%(boardip, arg, a,)
        return 1
    ACQ2SH=acq2sh

    def getnumsamples(self, arg):
        self.acqcmd('getNumSamples')
        return 1
    GETNUMSAMPLES=getnumsamples
    
    def getstate(self, arg):
        self.acqcmd('getState')
        return 1
    GETSTATE=getstate
