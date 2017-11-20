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
import ftplib
import tempfile
import socket
import json

import MDSplus
import acq200, transport

class Acq(MDSplus.Device):
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

    debug=None
    data_socket = -1

    wires = [ 'fpga','mezz','rio','pxi','lemo', 'none', 'fpga pxi', ' ']
    max_tries = 120

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)

    def getInteger(self, node, cls):
        try:
            ans = int(node.record)
        except Exception as e:
            print("ACQ error reading %s erro is\n%s" %(node, e,))
            raise cls()
        return ans

    def getPreTrig(self) :
        parts = self.settings['getNumSamples'].split('=')
        pre_trig = int(parts[2].split(' ')[0])
        return pre_trig

    def getPostTrig(self) :
        parts = self.settings['getNumSamples'].split('=')
        post_trig = int(parts[3].split(' ')[0])
        return post_trig

    def getBoardIp(self):
        from MDSplus.mdsExceptions import DevNO_NAME_SPECIFIED
        from MDSplus.mdsExceptions import TreeNODATA
        try:
            boardip=str(self.node.record)
        except Exception as e:
            raise DevNO_NAME_SPECIFIED(str(e))
        if len(boardip) == 0 :
            raise DevNO_NAME_SPECIFIED()
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
                print("flushing old data from socket")
            dummy = self.data_socket.recv(99999, socket.MSG_DONTWAIT)
#            self.data_socket.close()
#            self.data_socket = -1
            time.sleep(.05)
            self.connectAndFlushData()

    def readRawData(self, chan, pre, start, end, inc, retrying) :
        from MDSplus.mdsExceptions import DevERROR_READING_CHANNEL
        if self.debugging():
            print("starting readRawData(chan=%d, pre=%d. start=%d, end=%d, inc=%d)" %(chan,pre, start, end, inc,))
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

        except socket.timeout as e:
            if not retrying :
                print("Timeout - closing socket and retrying")
                self.data_socket.shutdown(socket.SHUT_RDWR)
                self.data_socket.close()
                self.data_socket=-1
                return self.readRawData(chan, pre, start, end, inc, True)
            else:
                 raise
        except Exception as e:
            print("ACQ error reading channel %d\n, %s" % (chan, e,))
            raise DevERROR_READING_CHANNEL(str(e))
        if self.debugging():
            print("Read Raw data pre=%d start=%d end = %d inc=%d returning len = %d" % (pre, start, end, inc, len(ans),))
        return ans

    def loadSettings(self):
        from MDSplus.mdsExceptions import DevCANNOT_LOAD_SETTINGS
        tries = 0
        self.settings = None
        last_error = None
        while self.settings == None and tries < 10 :
            try:
                tries = tries + 1
                self.settings = self.readSettings()
                complete=1
            except Exception as e:
                last_error=e
                if self.debugging():
                    print("ACQ196 Error loading settings%s" % str(e))
        if self.settings == None :
            print("after %d tries could not load settings" % (tries,))
            raise DevCANNOT_LOAD_SETTINGS(str(last_error))

    def checkTreeAndShot(self, arg1='checks', arg2='checks'):
        from MDSplus.mdsExceptions import DevWRONG_TREE
        from MDSplus.mdsExceptions import DevWRONG_SHOT
        from MDSplus.mdsExceptions import DevWRONG_PATH

        if arg1 == "checks" or arg2 == "checks":
            path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            if self.debugging() :
                print("json is loaded")
            if tree != self.settings['tree'] :
                print("ACQ Device open tree is %s board armed with tree %s" % (tree, self.settings["tree"],))
                raise DevWRONG_TREE()
            if path != self.settings['path'] :
                print("ACQ device tree path %s, board armed with path %s" % (path, self.settings["path"],))
                raise DevWRONG_PATH()
            if shot != int(self.settings['shot']) :
                print("ACQ open shot is %d, board armed with shot %d" % (shot, int(self.settings["shot"]),))
                raise DevWRONG_SHOT()

    def storeStatusCommands(self):
        status = []
        cmds = self.status_cmds.record
        for cmd in cmds:
            cmd = cmd.strip()
            if self.debugging():
                print("about to append answer for /%s/\n   which is /%s/" % (cmd,str(self.settings[cmd],)))
            status.append(self.settings[cmd])
            if self.debugging():
                print("%s returned %s\n" % (cmd, self.settings[cmd],))
        if self.debugging():
            print("about to write board_status signal")
        self.board_status.record = MDSplus.Signal(cmds, None, status)

    def checkTrigger(self, arg1, arg2):
        from time import sleep
        from MDSplus.mdsExceptions import DevNOT_TRIGGERED
        state = self.getBoardState()
        if state == "ACQ32:0 ST_STOP" or (state == "ACQ32:4 ST_POSTPROCESS" and (arg1 == "auto" or arg2 == "auto")):
            return
        if arg1 != "auto" and arg2 != "auto" :
            tries = 0
            while state == "ACQ32:4 ST_POSTPROCESS" and tries < 120:
                tries +=1
                sleep(1)
                state=self.getBoardState()
            if state != "ACQ32:0 ST_STOP" :
                raise DevNOT_TRIGGERED()

    def triggered(self):
        import time
        complete = 0
        tries = 0
        while not complete and tries < 120 :
            state = self.getBoardState()
            if self.debugging():
                print("get state returned %s" % (state,))
            if state == "ACQ32:4 ST_POSTPROCESS" :
                tries +=1
                time.sleep(1)
            else:
                complete=1
        state = self.getBoardState()
        if self.debugging():
            print("get state after loop returned %s" % (state,))
        if state != "ACQ32:0 ST_STOP" :
            print("ACQ196 device not triggered /%s/"% (self.getBoardState(),))
            return 0
        return 1

    def getBoardState(self):
        from MDSplus.mdsExceptions import DevCANNOT_GET_BOARD_STATE
        boardip = self.getBoardIp()
        last_error = None
        for t in range(10):
            try:
                UUT = acq200.ACQ200(transport.factory(boardip))
            except Exception as e:
                print("could not connect to the board %s"% (boardip,))
                last_error=e
            try:
                if not UUT == None:
                    a = UUT.uut.acqcmd('getState')
                    return "ACQ32:%s"%a
            except Exception as e:
                print("could not send getState to the board try %d"%t)
                last_error = e
        if not last_error == None:
            raise DevCANNOT_GET_BOARD_STATE(str(last_error))
        else:
            return 'unkown'

#    def getBoardState(self):
#        """Get the current state"""
#        import socket,time
#       for tries in range(5):
#            s=socket.socket()
#           s.settimeout(5.)
#            state="Unknown"
#            try:
#                s.connect((self.getBoardIp(),54545))
#                state=s.recv(100)[0:-1]
#               if self.debugging():
#                   print "getBoardState  returning /%s/\n" % (state,)
#                s.close()
#                return state
#            except socket.error, e:
#               print "Error getting board state - offline: %s" % (str(e),)
#               state = "off-line"
#                s.close()
#               return state
#            except Exception,e:
#                print "Error getting board state: %s" % (str(e),)
#               state = "off-line"
#                s.close()
#            time.sleep(3)
#
#
# Let this function raise an error that will be
# Thrown all the way out of the device method
#
    def getMyIp(self):
        import socket
        from MDSplus.mdsExceptions import DevOFFLINE
        try:
            s=socket.socket()
            s.connect((self.getBoardIp(),54545))
            hostip = s.getsockname()[0]
            state=s.recv(100)[0:-1]
            if self.debugging():
                print("getMyIp  read /%s/\n" % (state,))
            s.close()
        except Exception as e:
            raise DevOFFLINE(str(e))
        return hostip

    def addGenericJSON(self, fd):
        if self.debugging():
            print("starting addGenericJson")
        fd.write(r"""
begin_json() {   echo "{"; }
end_json() {   echo "\"done\" : \"done\"  }"; }
add_term() {   echo " \"$1\" : \"$2\", "; }
add_acqcmd() { add_term "$1" "`acqcmd $1`"; }
add_cmd() { add_term "$1" "`$1`"; }
settingsf=/tmp/settings.json
begin_json > $settingsf
add_term tree $tree >> $settingsf
add_term shot $shot >> $settingsf
add_term path $path >> $settingsf
""")
        cmds = self.status_cmds.record
        for cmd in cmds:
            cmd = cmd.strip()
            if self.debugging():
                print("adding cmd '%s' >> $settingsf/ to the file."%(cmd,))
            fd.write("add_cmd '%s' >> $settingsf\n"%(cmd,))
        fd.write(r"""
cat - > /etc/postshot.d/postshot.sh <<EOF
begin_json() {   echo "{"; }
end_json() {   echo "\"done\" : \"done\"  }"; }
add_term() {   echo " \"\$1\" : \"\$2\", "; }
add_acqcmd() { add_term "\$1" "\`acqcmd \$1\`"; }
add_cmd() { add_term "\$1" "\`\$1\`"; }
settingsf=/tmp/settings.json
add_acqcmd getNumSamples >> $settingsf
add_acqcmd getChannelMask >> $settingsf
add_acqcmd getInternalClock >> $settingsf
add_cmd date >> $settingsf
add_cmd hostname >> $settingsf
add_cmd 'sysmon -T 0' >> $settingsf
add_cmd 'sysmon -T 1' >> $settingsf
add_cmd get.channelMask >> $settingsf
add_cmd get.channel_mask >> $settingsf
add_cmd get.d-tacq.release >> $settingsf
add_cmd get.event0 >> $settingsf
add_cmd get.event1 >> $settingsf
add_cmd get.extClk  >> $settingsf
add_cmd get.ext_clk >> $settingsf
add_cmd get.int_clk_src >> $settingsf
add_cmd get.modelspec >> $settingsf
add_cmd get.numChannels >> $settingsf
add_cmd get.pulse_number >> $settingsf
add_cmd get.trig >> $settingsf
""")
    def finishJSON(self, fd, auto_store):
        if self.debugging():
            print("starting finishJSON")
        fd.write("end_json >> $settingsf\n")

        if auto_store != None :
            if self.debugging():
                fd.write("mdsValue 'setenv(\"\"DEBUG_DEVICES=yes\"\")'\n")
            fd.write("mdsConnect %s\n" %self.getMyIp())
            fd.write("mdsOpen %s %d\n" %(self.local_tree, self.tree.shot,))
            fd.write("mdsValue 'tcl(\"\"do /meth %s autostore\"\", _out),_out'\n" %( self.local_path, ))
            if self.debugging():
                fd.write("mdsValue 'write(*,_out)'\n")
            fd.write("mdsClose\n")
            fd.write("mdsDisconnect\n")

        fd.write("EOF\n")
        fd.write("chmod a+x /etc/postshot.d/postshot.sh\n")
        fd.flush()
        fd.seek(0,0)
        self.auto_store=auto_store

#
#  storeChannel method, stores the data for one channel of an ACQxxx device.
#    allows caller to catch any errors raised by MDSplus construction or IO
#
    def storeChannel(self, chan, chanMask, preTrig, postTrig, clock, vins):
        if self.debugging():
            print("working on channel %d" % chan)
        chan_node = self.__getattr__('input_%2.2d' % (chan+1,))
        if chan_node.on :
            if self.debugging():
                print("it is on so ...")
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
                    print("about to readRawData(%d, preTrig=%d, start=%d, end=%d, inc=%d)" % (chan+1, preTrig, start, end, inc))
                buf = self.readRawData(chan+1, preTrig, start, end, inc, False)
                if self.debugging():
                    print("readRawData returned %s" % (type(buf),))
                if inc == 1:
                    dim = MDSplus.Dimension(MDSplus.Window(start, end, self.trig_src ), clock)
                else:
                    dim = MDSplus.Dimension(MDSplus.Window(start/inc, end/inc, self.trig_src ), MDSplus.Range(None, None, clock.evaluate().getDelta()*inc))
                dat = MDSplus.Data.compile(
                        'build_signal(build_with_units((($1+ ($2-$1)*($value - -32768)/(32767 - -32768 ))), "V") ,build_with_units($3,"Counts"),$4)',
                        vins[chan*2], vins[chan*2+1], buf,dim)
                exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')

    def startInitializationFile(self, fd, trig_src, pre_trig, post_trig):
        if self.debugging():
            print("starting startInitialization")
        host = self.getMyIp()
        fd.write("acqcmd setAbort\n")
        fd.write("host=%s\n"%(host,))
        fd.write("tree=%s\n"%(self.local_tree,))
        fd.write("shot=%s\n"%(self.tree.shot,))
        fd.write("path='%s'\n"%(self.local_path,))

        for i in range(6):
            line = 'D%1.1d' % i
            try:
                wire = str(self.__getattr__('di%1.1d_wire' %i).record)
                if self.debugging():
                    print("wire is %s" % (wire,))
                if wire not in self.wires :
                    wire = 'fpga'
            except Exception as e:
                if self.debugging():
                    print("error retrieving wire %s" %str(e))
                wire = 'fpga'
            try:
                bus = str(self.__getattr__('di%1.1d_bus' % i).record)
                if self.debugging():
                    print("bus is %s" % (bus,))
                if bus not in self.wires :
                    print("DI%d:bus must be in %s" % (i, str(self.wires)))
                    bus = ''
            except Exception as e:
                if self.debugging():
                    print("error retrieving bus %s" %(e,))
                bus = ''
            if bus != '' :
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))
                if self.debugging():
                    print("set.route %s in %s out %s" %(line, wire, bus,))

    def readSettings(self):
        settingsfd = tempfile.TemporaryFile()
        ftp = ftplib.FTP(self.getBoardIp())
        ftp.login('dt100', 'dt100')
        ftp.retrlines("RETR /tmp/settings.json", lambda s, w=settingsfd.write: w(s+"\n"))
        settingsfd.seek(0,0)
        if self.debugging():
            print("got the settings")

        try :
            settings = json.load(settingsfd)
        except Exception as e:
            settingsfd.close()
            raise
        settingsfd.close()
        if self.debugging():
            print("The settings are loaded")
        return settings

#
# Do not return status let caller catch (or not)
# exceptions
#
    def doInit(self,fd):
        """Tell the board to arm"""
        import socket
        from MDSplus.mdsExceptions import DevERROR_DOING_INIT

        if self.debugging():
            print("starting doInit")
        status=1
        try:
            ftp = ftplib.FTP(self.getBoardIp())
            ftp.login('dt100','dt100')
            ftp.storlines("STOR /tmp/initialize",fd)
        except Exception as e:
            print("Error sending arm commands via ftp to %s\n%s" % (self.getBoardIp(), e,))
            raise DevERROR_DOING_INIT(str(e))
        s=socket.socket()
        try:
            s.settimeout(10.)
            s.connect((self.getBoardIp(),54546))
            s.send("initialize")
        except Exception as e:
            status=0
            s.close()
            print("Error sending doInit: %s" % (str(e),))
            raise DevERROR_DOING_INIT(str(e))
        s.close()
        if self.debugging():
            print("finishing doInit")

    def storeClock(self):
        clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        if self.debugging():
            print("clock_src = %s" % (clock_src,))
        try:
            clock_div = int(self.clock_div)
        except:
            clock_div = 1
        if self.debugging():
           print("clock div is %d" % (clock_div,))

        if clock_src == 'INT_CLOCK' :
            intClock = float(self.settings['getInternalClock'].split()[1])
            delta=1.0/float(intClock)
            self.clock.record = MDSplus.Range(None, None, delta)
        else:
            if self.debugging():
                print("it is external clock")
            if clock_div == 1 :
                self.clock.record = self.clock_src
            else:
                if self.debugging():
                    print("external clock with divider %d  clock source is %s" % ( clock_div, clock_src,))
                clk = self.clock_src
                try :
                    while type(clk) != MDSplus.compound.Range :
                        clk = clk.record
                    if self.debugging():
                        print("I found the Range record - now writing the clock with the divide")
                    self.clock.record = MDSplus.Range(clk.getBegin(), clk.getEnding(), clk.getDelta()*clock_div)
                except:
                    print("could not find Range record for clock to construct divided clock storing it as undivided")
                    self.clock.record  = clock_src
                if self.debugging():
                    print("divided clock stored")

    def waitftp(self) :
        import time
        from MDSplus.mdsExceptions import DevOFFLINE
        from MDSplus.mdsExceptions import DevNOT_TRIGGERED
        from MDSplus.mdsExceptions import DevIO_STUCK
        from MDSplus.mdsExceptions import DevTRIGGERED_NOT_STORED
        from MDSplus.mdsExceptions import DevUNKOWN_STATE

        """Wait for board to finish digitizing and storing the data"""
        state = self.getBoardState()
        tries = 0
        while (state == "ACQ32:4 ST_POSTPROCESS") and tries < self.max_tries :
            tries = tries + 1
            state = self.getBoardState()
            time.sleep(2)

        if state == 'off-line' :
            raise DevOFFLINE()
        if state == "ACQ32:1 ST_ARM" or state == "ACQ32:2 ST_RUN" :
            raise DevNOT_TRIGGERED()
        if state == "ACQ32:4 ST_POSTPROCESS" :
            raise DevIO_STUCK()
        if state == "ACQ32:0 ST_STOP" or state == "Ready":
            for chan in range(int(self.active_chan.record), 0, -1):
                chan_node = self.__getattr__('input_%2.2d' % (chan,))
                if chan_node.on :
                    max_chan = chan_node
                    break

            if max_chan.rlength > 0:
                return 1
            else:
                print("Triggered, but data not stored !")
                raise DevTRIGGERED_NOT_STORED()
        else:
            print("ACQxxx UNKNOWN BOARD state /%s/" % (state,))
            raise DevUNKOWN_STATE()

    WAITFTP=waitftp

    def trigger(self):
        from MDSplus.mdsExceptions import DevTRIGGER_FAILED

        if self.debugging():
            print("starting trigger")
        try:
            boardip = self.getBoardIp()
            trig_src=self.trig_src.record.getOriginalPartName().getString()[1:]
            if self.debugging() :
                print("executing trigger on board %s, trig_src is %s."% (boardip, trig_src,))
            trig_src = trig_src[2:]

            UUT = acq200.ACQ200(transport.factory(boardip))
            route = UUT.uut.acq2sh('get.route D%s'%(trig_src,))

            d1 = UUT.uut.acq2sh('set.route d%s in fpga out'%(trig_src,))
            d2 = UUT.uut.acq2sh('set.dtacq dio_bit %s P'%(trig_src,))
            d3 = UUT.uut.acq2sh('set.route %s' %(route,))
            d4 = UUT.uut.acq2sh('set.dtacq dio_bit %s -'%(trig_src,))

            if self.debugging():
                print("got back: %s" % (route,))
                print("     and: %s" % (d1,))
                print("     and: %s" % (d2,))
                print("     and: %s" % (d3,))
                print("     and: %s" % (d4,))
        except Exception as e:
            print("Error doing Trigger method")
            raise DevTRIGGER_FAILED(str(e))
        return 1
    TRIGGER=trigger

    def acqcmd(self, arg):
        from MDSplus.mdsExceptions import DevACQCMD_FAILED

        boardip = self.getBoardIp()

        try:
            UUT = acq200.ACQ200(transport.factory(boardip))
            a = UUT.uut.acqcmd(str(arg))
        except Exception as e:
            print("could not send %s to the board" %(str(arg),))
            raise DevACQCMD_FAILED(str(e))
        print("%s  %s -> %s"%(boardip, arg, a,))
        return 1
    ACQCMD=acqcmd

    def acq2sh(self, arg):
        from MDSplus.mdsExceptions import DevACQ2SH_FAILED
        boardip = self.getBoardIp()

        try:
            UUT = acq200.ACQ200(transport.factory(boardip))
            a = UUT.uut.acq2sh(str(arg))
        except Exception as e:
            print("could not connect to the board %s"% (boardip,))
            raise DevACQ2SH_FAILED(str(e))
        print("%s  %s -> %s"%(boardip, arg, a,))
        return 1
    ACQ2SH=acq2sh

    def getnumsamples(self):
        self.acqcmd('getNumSamples')
        return 1
    GETNUMSAMPLES=getnumsamples

    def getstate(self):
        self.acqcmd('getState')
        return 1
    GETSTATE=getstate

    def autostore(self):
        self.store("auto")
        return 1
    AUTOSTORE=autostore
