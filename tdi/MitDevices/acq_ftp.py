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

import time
import os
import numpy
import array

import MDSplus
from MitDevices import acq200, transport


class ACQ_FTP(MDSplus.Device):
    """
    Abstract class to subclass the d-tacq acqxxx_ftp device types.  Contains
    members and methods that all of the acqxxx_ftp  devices share:


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

    clock_parts=[
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INITFTP',head))",
         'options':('no_write_shot',)},
        {'path':':STORE_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'WAITFTP',head))",
         'options':('no_write_shot',)}]


    debug=None

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

    def readRawData(self, name, pre, start, end, inc) :
        f = open(name, mode='rb')
        try:
            f.seek((pre+start)*2)
            binValues = array.array('H')
	    binValues.read(f,end-start+1)
            ans = numpy.array(binValues, dtype=numpy.int16)
	    if inc > 1 :
		ans = ans[::inc]
            f.close()
        except Exception,e :
	   print "readRawData - %s" % e
           raise e
        return ans

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
        """Tell the board to arm"""
        import socket,signal
        if self.debugging():
	    print "starting doInit"
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
        if self.debugging():
            print "finishing doInit"

        return status

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
            time.sleep(3)
            tries = tries+1
        if tries == 60:
            print "Triggered, but data not stored !"
            return 0

        return 1
    WAITFTP=waitftp

    def trigger(self, arg):
        boardip = self.getBoardIp()
        try:
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
        except:
            print "could not read trigger source"
            return 0
        if self.debugging() :
            print "executing trigger on board %s, trig_src is %s."% (boardip, trig_src,)
        trig_src = trig_src[2:]
        cmd = "set.dtacq dio_bit %s P" % (trig_src,)
        if self.debugging() :
            print "sending command %s."%(cmd,)
        try:
            UUT = acq200.ACQ200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            a = UUT.uut.acq2sh(cmd)
            b = UUT.uut.acqcmd('-- setDIO -------')
        except:
            print "could not send %s to the board" %(cmd,)
        if self.debugging():
            print "got back: %s\n" % (a,)
	    print "     and: %s\n" % (b,)
        return 1
    TRIGGER=trigger

    def acqcmd(self, arg):
        boardip = self.getBoardIp()

        try:
            UUT = acq200.ACQ200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            a = UUT.uut.acqcmd(str(arg))
        except:
            print "could not send %s to the board" %(str(arg),)
            return 0
        print "%s  %s -> %s"%(boardip, arg, a,)
        return 1

    def acq2sh(self, arg):
        boardip = self.getBoardIp()

        try:
            UUT = acq200.ACQ200(transport.factory(boardip))
        except:
            print "could not connect to the board %s"% (boardip,)
        try:
            a = UUT.uut.acq2sh(str(arg))
        except:
            print "could not send %s to the board" %(str(arg),)
            return 0
        print "%s  %s -> %s"%(boardip, arg, a,)
        return 1
