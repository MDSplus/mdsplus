from MDSplus import Device,Data,Action,Dispatch,Method, makeArray, Range, Signal, Window, Dimension
from time import sleep
from tempfile import *
import acq200
import transport
from Dt200WriteMaster import Dt200WriteMaster

from time import sleep
import os
import numpy
import array
from xml.marshal.generic import dumps, loads, load 


class DT196(Device):
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
        parts.append({'path':':DI%1.1d:BUS'%(i,),'type':'text','options':('no_write_shot',)})
        parts.append({'path':':DI%1.1d:WIRE'%(i,),'type':'text','options':('no_write_shot',)})

    parts2=[
        {'path':':CLOCK_SRC','type':'text','value':'INT','options':('no_write_shot',)},
        {'path':':CLOCK_DIV','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':DAQ_MEM','type':'numeric','value':512,'options':('no_write_shot',)},
        {'path':':ACTIVE_CHANS','type':'numeric','value':96,'options':('no_write_shot',)},
        {'path':':TRIG_SRC','type':'text','value':'DI3','options':('no_write_shot',)},
        {'path':':POST_TRIG','type':'numeric','value':128,'options':('no_write_shot',)},
        {'path':':PRE_TRIG','type':'numeric','value':0,'options':('no_write_shot',)},
        ]
    parts.extend(parts2)
    del parts2
    for i in range(96):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%2.2d:STARTIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:ENDIDX'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:INC'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:FILTER_COEFS'%(i+1,),'type':'NUMERIC', 'options':('no_write_shot')})

    parts.append({'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
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
    
    wires = [ 'fpga','mezz','rio','pxi','lemo', 'none', 'fpga pxi']
    
    del i
    def getPreTrig(self,str) :
	parts = str.split('=')
        pre_trig = int(parts[2].split(' ')[0])
	return pre_trig

    def getPostTrig(self,str) :
        parts = str.split('=')
        post_trig = int(parts[3].split(' ')[0])
        return post_trig

    def readRawData(self, name, start, end, inc) :

        f = open(name, mode='rb')
        #try:
        f.seek(start*2)
        binValues = array.array('H')
        binValues.read(f,end-start)
        ans = numpy.array(binValues, dtype=numpy.int16)
        f.close()
# use the numpy reader
# and shape as 2d  then subscript the dim
# to get the inc

        #except Exception,e:
        #    try:
        #        f.close()
        #    except:
        #        pass
        #    raise e
        return ans

    def check(self, expression, message):
        try:
            ans = eval(expression)
            return ans
        except:
            raise Exception, message
        
    def initftp(self, arg):
        """
        Initialize the device
        Send parameters
        Arm hardware
        """
        msg=None
        debug=os.getenv("DEBUG_DEVICES")
        try:
	    boardip=str(self.node.record)
            if len(boardip) == 0 :
              raise Exception, "boardid record empty"
        except:
	    try:
		boardip = Dt200WriteMaster(int(self.board), "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)[0]
	    except:
		raise Exception, "could not get board ip from either tree or hub"

	try:
	    path = self.local_path
            tree = self.local_tree
            shot = self.tree.shot
            UUT = acq200.Acq200(transport.factory(boardip))
            msg="Must specify active chans as int in (32,64,96)"
            #active_chans = self.check("int(self.active_chans)", "Must specify active chans as int in (32,64,96)")
            active_chans = int(self.active_chans)
            msg=None
            if active_chans not in (32,64,96) :
                print "active chans must be in (32, 64, 96 )"
                active_chans = 96
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
            fd.write("#!/bin/sh\n")
	    fd.write("curl -u mdsftp:mdsftp ftp://192.168.0.254/storeftp.sh > /tmp/storeftp.sh\n")
            fd.write("chmod a+x /tmp/storeftp.sh\n")
            fd.write("mkdir -p /etc/postshot.d\n")
	    fd.write("echo /tmp/storeftp.sh %s %d %s > /etc/postshot.d/post_shot.sh\n" % (self.local_tree, self.tree.shot, self.local_path,))
	    fd.write("echo rm -f \$0 >> /etc/postshot.d/post_shot.sh\n")
            fd.write("chmod a+x /etc/postshot.d/post_shot.sh\n")
            fd.write("acqcmd setAbort\n")
            for dx in ['d0', 'd1', 'd2', 'd3', 'd4', 'd5' ] :
                fd.write("set.route " + dx + " in fpga\n")

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
                    bus = str(self.__getattr__('di%1.1d_bus' % 1).record)
                    if bus not in self.wires :
                        print "DI%d:bus must be in %s" % (i, str(self.wires),)
                        bus = ''
                except:
                    bus = ''
                fd.write("set.route %s in %s out %s\n" %(line, wire, bus,))
                fd.write("acqcmd  setChannelMask " + '1' * active_chans+"\n")

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

            fd.write("set.pre_post_mode %d %d\n" %(pre_trig,post_trig,))

            fd.write("acqcmd setArm\n")
            fd.flush()
	    fd.close()
            fname = "%s_%s_%s.sh" % (tree, shot, path)
            cmd = "curl -u mdsftp:mdsftp ftp://192.168.0.254/scratch/%(fname)s > /tmp/%(fname)s; chmod a+x /tmp/%(fname)s; /tmp/%(fname)s; rm -f /tmp/%(fname)s\n" % {'fname': fname}
            cmd = "curl -u mdsftp:mdsftp ftp://192.168.0.254/scratch/%(fname)s > /tmp/%(fname)s; chmod a+x /tmp/%(fname)s; /tmp/%(fname)s\n" % {'fname': fname}
	    UUT.uut.acq2sh(cmd)
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
        numSampsStr = settings['getNumSamples']
	preTrig = self.getPreTrig(numSampsStr)
        postTrig = self.getPostTrig(numSampsStr)
        #vins = eval('makeArray([%s])' % settings['get.vin'])
        vins = makeArray(numpy.array(settings['get.vin'].split()).astype('int'))
        chanMask = settings['getChannelMask'].split('=')[-1]
	if settings['get.extClk'] == 'none' :
	    #intClkStr=settings['getInternalClock'].split()[0].split('=')[1]
            #intClock=int(intClikStr)
	    intClock=1e6
            delta=1./float(intClock)
	else:
	    delta = 0
        
        #trigExpr = 'self.%s'% str(self.trig_src.record)
        #trig_src = eval(trigExpr.lower())
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
			#start = max(eval('int(self.input_%2.2d:start_idx)'%(chan+1,)), preTrig)
                        start = max(int(self.__getattr__('input_%2.2d:start_idx'%(chan+1,))),preTrig)
                    except:
                        start = preTrig
                    try:
                        #end = min(eval('int(self.input_%2.2d:end_idx)'%(chan+1,)), postTrig)
                        end = min(int(self.__getattr__('input_%2.2d:end_idx'%(chan+1,))),postTrig)
                    except:
                        end = postTrig
                    try:
                        #inc =  max(eval('int(self.input_%2.2d:inc)'%(chan+1,)), 1)
                        inc = max(int(self.__getattr__('input_%2.2d:inc'%(chan+1,))),1)
                    except:
                        inc = 1
#
# could do the coeffs
#
		    chanFileName="%s/%2.2d"%(dataDir, chan+1,)
                    buf = self.readRawData(chanFileName, start, end, inc)
#                    try:
#                        buf = self.readRawData(chanFileName, start, end, inc)
#                    except:
#			print "Error Reading Channel %d"%(chan+1,)
		    if delta != 0 :
			axis = Range(None, None, delta/inc)
		    else:
			#clockExpr = 'self.%s'% str(self.clock_src.record)
			#clock_src = eval(clockExpr.lower())
                        clock_src = self._getattr__(str(self.clock_src.record).lower())
                        axis = clock_src

		    if inc == 1:
			dim = Dimension(Window(start, end, trig_src ), axis)
                    else:
			dim = Data.compile('Map($,$)', Dimension(Window(start/inc, end/inc, trig_src), axis), Range(start, end, inc))
                    dat = Data.compile('_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)', vins[chan*2], vins[chan*2+1], buf,dim) 
                    exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
	return 1

    STOREFTP=storeftp

    def waitftp(self, arg) :
        try:
            boardip=str(self.node.record)
        except:
            try:
                boardip = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
            except:
                raise Exception, "could not get board ip from either tree or hub"

        UUT = acq200.Acq200(transport.factory(boardip))
        state = UUT.get_state()
	state = state[2:]
        if state == 'ST_ARM' or state == 'ST_RUN' :
            raise  Exeption, "device Not triggered"
		
	for chan in range(96):
	    chan_node = self.__getattr__('input_%2.2d' % (chan+1,))
            if chan_node.on :
		max_chan = chan_node
        tries = 0
	while tries < 60 :
	    try:
		sz = len(max_chan)
                if sz > 0:
        	  break
	    except:
		sleep(3)
		tries = tries+1
        if tries == 60:
	    raise  Exeption, "device Not triggered"

	return 1
    WAITFTP=waitftp
