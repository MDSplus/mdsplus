from tempfile import *
from time import sleep, time
import os
import numpy
import array
from xml.marshal.generic import dumps, loads, load 

from MDSplus import Device,Data,Action,Dispatch,Method,makeArray, Range, Signal, Window, Dimension


class ACQ216_FTP(Device):
    """
    D-Tacq ACQ216  16 channel transient recorder
    
    """
    
    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254','options':('no_write_shot',)},
        {'path':':COMMENT','type':'text'},
        ]

    for i in range(6):
        parts.append({'path':':DI%1.1d'%(i,),'type':'axis','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:WIRE'%(i,),'type':'text','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:BUS'%(i,),'type':'text','options':('no_write_shot',)})


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
        {'path':':STATUS_CMDS','type':'text','value':makeArray(['cat /proc/cmdline', 'get.d-tacq.release']),'options':('no_write_shot',)},
        {'path':':BOARD_STATUS','type':'signal','options':('write_shot',)},
        {'path':':CLOCK','type':'axis','options':('no_write_model','write_once')},
        ]
    parts.extend(parts2)
    del parts2
    for i in range(16):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:VIN'%(i+1,),'type':'NUMERIC', 'value':10, 'options':('no_write_shot')})
    del i

    parts.append({'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INITFTP',head))",
                  'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'WAITFTP',head))",
                  'options':('no_write_shot',)})
        
    wires = [ 'fpga','mezz','rio','pxi','lemo', 'none', 'fpga pxi', ' ']
    
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
        """Tell the board to arm"""
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
                fd.write("acqcmd setInternalClock %d\n" % clock_freq)
                if clock_out != None:
                    fd.write("acqcmd '-- setDIO -1-----'\n")
                    fd.write("set.route d2 in fpga out pxi\n")
                    fd.write("acqcmd '-- setDIO --1-----'\n")
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
        if debug:
            print "got preTrig %d and postTrig %d\n" % (preTrig, postTrig,)
        vins = makeArray(numpy.array(settings['get.vin'].split(',')).astype('float'))
        if debug:
            print "got the vins "
            print vins
	self.ranges.record = makeArray(numpy.array(settings['get.vin'].split(',')).astype('float'))
        chanMask = settings['getChannelMask'].split('=')[-1]
        if debug:
            print "chan_mask = %s\n" % (chanMask,)
        clock_src=self.clock_src.record.getOriginalPartName().getString()[1:]
        if debug:
            print "clock_src = %s\n" % (clock_src,)
        if clock_src == 'INT_CLOCK' :
	    intClock = float(settings['getInternalClock'].split()[1])
            delta=1./float(intClock)
            self.clock.record = Range(None, None, delta)
        else:
            self.clock.record = self.clock_src

        clock = self.clock.record
#
# now store each channel
#
	for chan in range(16):
	    if debug:
		print "working on channel %d" % chan
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
                    if debug:
			print "about to readRawData(%s, preTrig=%d, start=%d, end=%d, inc=%d)" % (chanFileName, preTrig, start, end, inc)
                    buf = self.readRawData(chanFileName, preTrig, start, end, inc)
		    if inc == 1:
			dim = Dimension(Window(start, end, self.trig_src ), clock)
                    else:
			dim = Data.compile('Map($,$)', Dimension(Window(start/inc, end/inc, self.trig_src), clock), Range(start, end, inc))
		    dat = Data.compile(
                        '_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)',
                        vins[chan*2], vins[chan*2+1], buf,dim) 
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
