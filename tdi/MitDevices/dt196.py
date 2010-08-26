from MDSplus import Device,Data,Action,Dispatch,Method, makeArray, Range, Signal, Window, Dimension
from tempfile import *
import acq200
import transport

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
#ACQ32:getNumSamples=65541 pre=0 post=65541 elapsed=61445
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

        debug=os.getenv("DEBUG_DEVICES")
        try:
	    boardip=str(self.node.record)
        except:
	    try:
		boardip = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
	    except:
		raise Exception, "could not get board ip from either tree or hub"

	try:
            UUT = acq200.Acq200(transport.factory(boardip))
            active_chans = self.check("int(self.active_chans)", "Must specify active chans as int in (32,64,96)")
            if active_chans not in (32,64,96) :
                print "active chans must be in (32, 64, 96 )"
                active_chans = 96
            trig_src=self.check("str(self.trig_src.record)", "Could not read trigger source")
            if not trig_src in self.trig_sources:
                raise Exception, "Trig_src must be in %s" % str(self.trig_sources)
            clock_src=self.check("str(self.clock_src.record)", "Could not read clock source")
            if not clock_src in self.clock_sources:
                raise Exception, "clock_src must be in %s" % str(self.clock_sources)
            pre_trig=self.check('int(self.pre_trig.data()*1024)', "Must specify pre trigger samples")
            post_trig=self.check('int(self.post_trig.data()*1024)', "Must specify post trigger samples")
            if clock_src == "INT" or clock_src == "MASTER":
		clock_freq = self.check('int(self.clock_div)', "Must specify clock frequency in clock_div node for internal clock")
            else :
		try:
		    clock_div = int(self.clock_div)
		except:
		    clock_div = 1

            UUT.set_abort()
            UUT.clear_routes()
            
            for i in range(6):
                line = 'd%1.1d' % i
                try:
                    wire = eval('str(self.di%1.1d_wire.record)' %i)
                    if wire not in self.wires :
                        print "DI%d:wire must be in %s" % (i, str(self.wires), )
                        wire = 'fpga'
                except:
                    wire = 'fpga'
                try:
                    bus = eval('str(self.di%1.1d_bus.record)' % i)
                    if bus not in self.wires :
                        print "DI%d:bus must be in %s" % (i, str(self.wires),)
                        bus = ''
                except:
                    bus = ''
                UUT.set_route(line, 'in %s out %s' % (wire, bus,))
            UUT.setChannelCount(active_chans)

            if clock_src == 'INT' or clock_src == 'MASTER' :
                UUT.uut.acqcmd("setInternalClock %d" % clock_freq)
		if clock_src == 'MASTER' :
		    UUT.uut.acqcmd('-- setDIO -1-----')
            else:
		if (clock_div != 1) :        
		    UUT.uut.acqcmd("setExternalClock %s %d D02" % (clock_src, clock_div,))
#
# the following is not generic
# the clock is output on d2 and comes from DI0
#
		    UUT.set_route('d2', 'in fpga out pxi')
		    UUT.uut.acqcmd('-- setDIO --1-----')
                    UUT.uut.acq2sh('set.ext_clk DI0')

		else :
                    UUT.uut.acqcmd("setExternalClock %s" % clock_src)

            UUT.setPrePostMode(pre_trig, post_trig)
#            mask = UUT.uut.acqcmd('getChannelMask').split('=')[-1]
#
# now create the post_shot ftp command file
#
            (fd,fname) = mkstemp('.sh')
            f=open(fname, 'w')
            f.write("#!/bin/sh\n")
	    f.write("storeftp.sh %s %d %s\n" % (self.local_tree, self.tree.shot, self.local_path,))
	    f.write("rm -f $0\n")
            f.flush()
	    f.close()
            cmd = 'curl -s -T %s ftp://%s/%s' %( fname, boardip, 'post_shot.sh')
            print cmd
            pipe = os.popen(cmd)
            pipe.close()
	    UUT.uut.acq2sh('mv /home/ftp/post_shot.sh /etc/postshot.d/')
            UUT.uut.acq2sh('chmod a+x /etc/postshot.d/post_shot.sh')
            UUT.set_arm() 
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
        vins = eval('makeArray([%s])' % settings['get.vin'])
        chanMask = settings['getChannelMask'].split('=')[-1]
	if settings['get.extClk'] == 'none' :
	    #intClkStr=settings['getInternalClock'].split()[0].split('=')[1]
            #intClock=int(intClikStr)
	    intClock=1e6
            delta=1./float(intClock)
	else:
	    delta = 0
        
        trigExpr = 'self.%s'% str(self.trig_src.record)
        trig_src = eval(trigExpr.lower())
#
# now store each channel
#
	for chan in range(96):
	    if debug:
		print "working on channel %d" % chan
	    chan_node = eval('self.input_%2.2d' % (chan+1,))
            if chan_node.on :
                if debug:
                    print "it is on so ..."
                if chanMask[chan:chan+1] == '1' :
                    try:
			start = max(eval('int(self.input_%2.2d:start_idx)'%(chan+1,)), preTrig)
                    except:
                        start = preTrig
                    try:
                        end = min(eval('int(self.input_%2.2d:end_idx)'%(chan+1,)), postTrig)
                    except:
                        end = postTrig
                    try:
                        inc =  max(eval('int(self.input_%2.2d:inc)'%(chan+1,)), 1)
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
			clockExpr = 'self.%s'% str(self.clock_src.record)
			clock_src = eval(clockExpr.lower())
                        axis = clock_src

		    if inc == 1:
			dim = Dimension(Window(start, end, trig_src ), axis)
                    else:
			dim = Data.compile('Map($,$)', Dimension(Window(start/inc, end/inc, trig_src), axis), Range(start, end, inc))
                    dat = Data.compile('_v0=$, _v1=$, build_signal(build_with_units(( _v0+ (_v1-_v0)*($value - -32768)/(32767 - -32768 )), "V") ,build_with_units($,"Counts"),$)', vins[chan*2], vins[chan*2+1], buf,dim) 
                    exec('c=self.input_'+'%02d'%(chan+1,)+'.record=dat')
