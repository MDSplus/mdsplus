from MDSplus import Device,Data,Action,Dispatch,Method
from Dt200WriteMaster import Dt200WriteMaster
import os
import numpy

class DTAO32(Device):
    """D-Tacq AO32 Analog output module"""

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
                 'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                 'options':('no_write_shot',)})
    parts.append({'path':':ARM_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','STORE',50,None),Method(None,'ARM',head))",
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

        hostname = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
        hostname = hostname[0].strip()
        
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

                self.SendFiles(hostname, hostboard, board)

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
            continuous=nids[self.part_names['CONTINUOUS']].record.data()
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


    def WriteWaveform(self, host, board, chan, wave):
        if chan == 1:
            pipe = os.popen('mkdir -p /tmp/%s/ao32cpci.%d' % (host, board));
            pipe.close()
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
        cmd = 'rm -rf /tmp/%s.%d.tgz; rm -rf /tmp/%s/ao32cpci.%d/' % (host, board, host, board,)
        print cmd
        pipe = os.popen(cmd)
        pipe.close()        
        Dt200WriteMaster(hostboard, '/ffs/unpack_waves %s %d' %(host, board), 1)

