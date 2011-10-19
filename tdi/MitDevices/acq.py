import time
import os
import numpy
import array
import ftplib
import tempfile
import socket
from xml.marshal.generic import dumps, loads, load 

import MDSplus
from MitDevices import acq200, transport


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
        {'path':':RANGES','type':'numeric','options':('write_once',)},
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

    def readRawData(self, chan, pre, start, end, inc) :
	if self.debugging():
	    print " starting readRawData(chan=%d, pre=%d. start=%d, end=%d, inc=%d)\n" %(chan,pre, start, end, inc,)
	try:
	    if self.data_socket == -1 :
                self.data_socket = socket.socket()
                self.data_socket.connect((self.getBoardIp(), 54547))
	    self.data_socket.settimeout(10.)
            self.data_socket.send("/dev/acq200/data/%02d\n" % (chan,))
            bytes_to_read = 2*(end+pre-1)
            buf = []
            while bytes_to_read > 0:
                chunk = self.data_socket.recv(bytes_to_read, socket.MSG_WAITALL)
                buf.append(chunk)
                bytes_to_read = bytes_to_read-len(chunk)
	    if self.debugging():
		print "  asked for %d got %d in %d chunks\n" % (2*(end+pre-1), len(''.join(buf)),len(buf),) 
            binValues = array.array('h')
            binValues.fromstring(''.join(buf))
            if inc == 1:
                ans = numpy.array(binValues[pre+start:end-start-1], dtype=numpy.int16)
            else :
                ans = numpy.array(binValues[pre+start:end-start-1:inc], dtype=numpy.int16)
	except Exception, e:
	    print "ACQ error reading channel %d\n, %s\n" % (chan, e,)
	    raise e
        return ans

    def timeoutHandler(self,sig,stack):
        raise Exception("Timeout occurred")
        
    def triggered(self):
        complete = 0
        tries = 0
        while not complete and tries < 60 :
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
            print "get state sfter loop returned %s\n" % (state,)
        if state != "ACQ32:0 ST_STOP" :
            print "ACQ196 device not triggered /%s/\n"% (self.getBoardState(),)
            return 0  # should return not triggered error
        return 1

    def getBoardState(self):
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
                  
    def getMyIp(self):
        import socket
        icmp = socket.getprotobyname("icmp")

        s=socket.socket()
	try:
            s.connect((self.getBoardIp(),54545))
            hostip = s.getsockname()[0]
	except Exception,e:
	    hostip = ""
	    print "could not connect to board to verify local IP address\n%s\n" %(e,)
	return hostip

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
        import socket,signal
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
            signal.signal(signal.SIGALRM,self.timeoutHandler)
            signal.alarm(15)
            s.connect((self.getBoardIp(),54546))
            s.send("initialize")
        except Exception,e:
            status=0
            print "Error sending doInit: %s" % (str(e),)
        signal.alarm(0)
        s.close()
        if self.debugging():
            print "finishing doInit"
        return status

    def waitftp(self, arg) :
        """Wait for board to finish digitizing and ftp'ing data to host"""
        state = self.getBoardState()
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
            time.sleep(3)
            tries = tries+1
        if tries == 60:
            print "Triggered, but data not stored !"
            return 0

        return 1
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
