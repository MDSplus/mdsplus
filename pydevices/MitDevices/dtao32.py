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

from MDSplus import Device,Data,Action,Dispatch,Method
from Dt200WriteMaster import Dt200WriteMaster
import os
import numpy

class DTAO32(Device):
    """
	D-Tacq AO32 Analog output module

        Methods:
		Add() - add a DTAO32 device to the tree open for edit
		Init(arg) - initialize the DTAO32 device 
			    write setup parameters and waveforms to the device
		Arm(arg)  - Send Commit to the device to arm it
		Help(arg) - Print this message

                
	Nodes:
		HOSTBOARD - the board number of the DT196 host card
		BOARD - the slot number in the crate of the AO32 card
		COMMENT - a comment
		TRIG_SRC : Where the AO32 will get its trigger -
		    trig_sources=[ 'S_PXI_0',
                		   'S_PXI_1',
              			   'S_LEMO_CLK_DIRECT',
                   		   'S_LEMO_CLK_OPTO',
                   		   'S_PXI_3',
                   		   'S_PXI_4',
                   		   'S_LEMO_TRG_DIRECT',
                   		   'S_LEMO_TRG_OPTO',
                     ]
		TRIG_EDGE - Trigger on 'RISING' or 'FALLING' edge
		CLOCK_SRC - Where the AO32 will get its clock See TRIG_SRC or 'S_INTERNAL'
                CLOCK_EDGE - See TRIG_EDGE
		CLOCK_DIV - decimation of clock for output
		MODE - operation mode  one of :
			'M_RIM',
           		'M_RTU',
			'M_AWGI',
			'M_AWGT',
			'M_LLI',
			'M_LLC'
		MAX_SAMPLES - the maximum number of samples to output
		CONTINUOUS - oneshot (0) or continuous (1)
		TRIGGER - Time the moudule was triggered (user provided)
		CLOCK - Source of clock edges (user provided)  - SHOULD BE STORED FOR INTERNAL !
		DIM   - TIMEBASE for output signals - do not change filled in at ADD time
		OUTPUT_01-OUTPUT_32 - Signal of voltage vs time for each channel (BUILD_SIGNAL(...) )
		INIT_ACTION - default initialization action
		STORE_ACTION - default store action

	Note:  In order to accomidate the shared commit operation for DTAO32 and DTDO32 the commit (arm) is a separate 
	       device method
    """

    parts=[
        {'path':':HOSTBOARD','type':'numeric','options':('noshot_write',)},
        {'path':':BOARD','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':COMMENT','type':'text'},
        {'path':':TRIG_SRC','type':'text','value':'S_LEMO_TRG_OPTO','options':('no_write_shot',)},
        {'path':':TRIG_EDGE','type':'text','value':'RISING','options':('no_write_shot',)},
        {'path':':CLOCK_SRC','type':'text','value':'S_LEMO_CLK_OPTO','options':('no_write_shot',)},
        {'path':':CLOCK_EDGE','type':'text','value':'RISING','options':('no_write_shot',)},
        {'path':':CLOCK_DIV','type':'numeric','value':1,'options':('noshot_write',)},
        {'path':':MODE','type':'text','value':'M_AWGT','options':('no_write_shot',)},
        {'path':':MAX_SAMPLES','type':'numeric','value':16384,'options':('noshot_write',)},
        {'path':':CONTINUOUS','type':'numeric','value':1,'options':('noshot_write',)},
        {'path':':TRIGGER','type':'numeric','value':0,'options':('noshot_write',)},
        {'path':':CLOCK','type':'axis','options':('noshot_write',)},
        {'path':':DIM','type':'any',
         'valueExpr':'Data.compile("build_dim(build_window(0,$1,$2),begin_of($3) : end_of($3) : (slope_of($3)/$4))",head.max_samples,head.trigger,head.clock,head.clock_div)',
         'options':('noshot_write','write_once')},
        ]
    for i in range(32):
        parts.append({'path':':OUTPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_shot',)})
        parts.append({'path':':OUTPUT_%2.2d:FIT'%(i+1,),'type':'text','value':'LINEAR', 'options':('no_write_shot',)})
    parts.append({'path':':INIT_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                 'options':('no_write_shot',)})
    parts.append({'path':':ARM_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',51,None),Method(None,'ARM',head))",
                 'options':('no_write_shot',)})
    parts.append({'path':':ZERO1_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',1,None),Method(None,'ZERO',head))",
                 'options':('no_write_shot',)})
    parts.append({'path':':ZERO2_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',1,None),Method(None,'ZERO',head))",
                 'options':('no_write_shot',)})

    clock_edges=['RISING', 'FALLING']
    trigger_edges = clock_edges
    trig_sources=[ 'S_PXI_0',
                   'S_PXI_1',
                   'S_LEMO_CLK_DIRECT',
                   'S_LEMO_CLK_OPTO',
                   'S_PXI_3',
                   'S_PXI_4',
                   'S_LEMO_TRG_DIRECT',
                   'S_LEMO_TRG_OPTO',
                   ]
    clock_sources = trig_sources
    clock_sources.append('S_INTERNAL')
    modes=['M_RIM',
           'M_RTU',
           'M_AWGI',
           'M_AWGT',
           'M_LLI',
           'M_LLC',
           ]       

    del i

    def init(self, arg):
        """Load the waveforms and other setup information for a D-Tacq AO32 board
        This device does only the analog output part of the setup.

        An arm operation must also be done to activate the device
        """

        debug=os.getenv("DEBUG_DEVICES")
        try:
            complaint = "Must specify host board number"
            hostboard=int(self.hostboard.record)
            complaint = "board must be an integer"
            board=int(self.board)
            complaint = "clock div must be an integer"
            clock_div= int(self.clock_div)
            complaint = "invalid clock source"
            clock_src=str(self.clock_src.data())
            if not clock_src in self.clock_sources:
                raise Exception, complaint
            complaint = "clock edge must be RISING or FALLING"
            clock_edge=str(self.clock_edge.data())
            if not clock_edge in self.clock_edges:
                raise Exception, complaint
            complaint = 'invalid trigger source'
            trig_src=str(self.trig_src.data())
            if not trig_src in self.trig_sources:
                raise Exception, complaint
            complaint = "trig edge must be RISING, FALLING or NONE"
            trig_edge=str(self.trig_edge.data())
            if not trig_edge in self.trigger_edges:
                raise Exception, complaint
            complaint = "invalid mode string"
            mode=str(self.mode.data())
            if not mode in self.modes:
                raise Exception, complaint
            complaint = 'Error reading dimension information'
            dim=self.dim.data()
            complaint = 'Error reading max_samples'
            max_samples=int(self.max_samples)
        except Exception,e:
            print "%s\n%s" % (complaint, str(e),)
            return 0

        try:
	    pipe = os.popen('acqcmd -b %d setAbort' % (hostboard));
            pipe.close()
	except Exception, e:
	    print "error sending abort to host board\n%s" %(str(e),)
            return 0

	hostname = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
        hostname = hostname[0].strip()
        
        self.first = True
        for i in range(32):
            ao_nid=self.__getattr__('output_%2.2d'%(i+1))
            fit='LINEAR'
            knots_y = [0.,0.]
            knots_x = [0., 1.]
           
            if ao_nid.on:
                try:
                    fit = str(self.__getattr__('ouput_%2.2d_fit'))
                except:
                    fit = 'LINEAR'
                try:
                    knots_y = ao_nid.record.data()
                    knots_x = ao_nid.record.getDimensionAt(0).data()
                except Exception,e:
                    knots_y = numpy.array([ 0.,  0.])
                    knots_x = numpy.array([ 0.,  1.])
                    print "Error reading data for channel %d - ZEROING ZEROING\n" % i
                if fit == 'SPLINE':
                    wave = Data.execute('SplineFit($,$,$)', knots_x, knots_y, dim)
                else:
                    wave = Data.execute('LinFit($,$,$)', knots_x, knots_y, dim)
                counts = wave.data() / 10.*2**15
                counts = numpy.int16(counts[:max_samples])
                self.WriteWaveform(hostname, board, i+1, counts)

        if not self.first:
            self.SendFiles(hostname, hostboard, board)
        else:
            raise Exception, 'No channels defined aborting'

        try:
            Dt200WriteMaster(hostboard, 'set.ao32 %d AO_MODE %s' % (board, mode), 1)
            Dt200WriteMaster(hostboard, 'set.ao32 %d AO_CLK  %s %d %s' % (board, clock_src, clock_div, clock_edge), 1) 
            Dt200WriteMaster(hostboard, 'set.ao32 %d AO_TRG %s %s' % (board, trig_src, trig_edge), 1)
        except Exception, e:
            print "Error sending commands to AO32 board\n%s" % (str(e),)
        return 1

    def arm(self, arg):
        try:
            complaint = 'Error reading continuos flag'
            continuous=int(self.continuous)
            complaint = "Must specify host board number"
            hostboard=int(self.hostboard.record)
            complaint = "board must be an integer"
            board=int(self.board)
        except Exception,e:
            print "%s\n%s", (complaint, str(e),)
        try:
            if (continuous):
              commit = '0x02'
            else :
              commit = '0x22'
            print "set.ao32.data %d commit %s" % (board, commit)
            Dt200WriteMaster(hostboard, 'set.ao32.data %d commit %s' % (board, commit), 1)
        except:
            raise Exception, 'error sending commands to AO32 board'
        return 1

    def help(self, arg):
 	""" Help method to describe the methods and nodes of the DTAO32 module type """
	help(DTAO32)
        return 1

    def zero(self, arg):
	""" zero method sets all of the outputs of the DTAO32  module to zero """
        self.first = True
        try:
            complaint = "host board must be an integer"
            hostboard=int(self.hostboard.record)
            complaint = "board must be an integer"
            board = int(self.board.record)
            complaint = "can not retrieve ip address for board %d" % hostboard
            hostname = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
            hostname = hostname[0].strip()
            complaint = "error putting board in immediate mode"
            Dt200WriteMaster(hostboard, 'set.ao32 %d AO_MODE M_RIM' % (board,), 1)
            for i in range(32):
                complaint = "error writing zero for channel %d to card" % (i+1,)  
                self.WriteWaveform(hostname, board, i+1, numpy.int16(numpy.array([ 0])))
            complaint, "error sending files to card"
            self.SendFiles(hostname, hostboard, board);

        except Exception, e:
            print "%s\n%s" % (complaint, str(e),)
            return 0
        return 1

    def WriteWaveform(self, host, board, chan, wave):
        if self.first:
            try:
                pipe = os.popen('mkdir -p /tmp/%s/ao32cpci.%d; chmod a+rwx /tmp/%s/ao32cpci.%d' % (host, board, host, board));
                pipe.close()
            except:
		pass
	    self.first=False

        file = '/tmp/%s/ao32cpci.%d/f.%2.2d' % (host, board, chan)
        f = open(file, 'wb')
        wave.tofile(f)
        f.close()

    def SendFiles(self, host, hostboard, board):
        cmd = '(cd /tmp/%s; tar -czf /tmp/%s.%d.tgz *)' % (host, host, board,)
        print cmd
        pipe = os.popen(cmd)
        pipe.close()
        cmd = 'curl -s -T /tmp/%s.%d.tgz -u ftp: ftp://%s/' %(host, board, host)
        print cmd
        pipe = os.popen(cmd)
        pipe.close()
        cmd = 'rm -rf /tmp/%s.%d.tgz; rm -rf /tmp/%s/' % (host, board, host,)
        print cmd
        pipe = os.popen(cmd)
        pipe.close()        
        Dt200WriteMaster(hostboard, '/ffs/unpack_waves %s %d' %(host, board), 1)
    INIT=init
    ARM=arm
    ZERO=zero

