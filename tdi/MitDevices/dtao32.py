class DTAO32(object):
    """D-Tacq AO32 Analog out"""
    def __init__(self,node):
        self.node=node
        self.part_names = {'':0,
                           'HOSTBOARD':1,
                           'BOARD':2,
                           'COMMENT':3,
                           'TRIG_SRC':4,
                           'TRIG_EDGE':5,
                           'CLOCK_SRC':6,
                           'CLOCK_EDGE':7,
                           'TRIGGER':8,
                           'CLOCK':9,
                           'CLOCK_DIV':10,
                           'MODE':11,
                           'MAX_SAMPLES':12,
                           'CONTINUOUS':13,
                           'DIM':14,
                           }
        for i in range(32) :
            self.part_names['OUTPUT_%2.2d' % (i+1)] = len(self.part_names)
            self.part_names['OUTPUT_%2.2d:FIT' % (i+1)] = len(self.part_names)
        self.part_names['INIT_ACTION'] = len(self.part_names)

        self.clock_edges=['RISING', 'FALLING']
        self.trigger_edges = self.clock_edges
        self.trig_sources=[ 'S_PXI_0',
                            'S_PXI_1',
                            'S_LEMO_CLK_DIRECT',
                            'S_LEMO_CLK_OPTO',
                            'S_PXI_3',
                            'S_PXI_4',
                            'S_LEMO_TRG_DIRECT',
                            'S_LEMO_TRG_OPTO',
                            ]
        self.clock_sources = self.trig_sources
        self.clock_sources.append('S_INTERNAL')
        self.modes=['M_RIM',
                    'M_RTU',
                    'M_AWGI',
                    'M_AWGT',
                    'M_LLI',
                    'M_LLC',
                    ]       
        return

    def init(self,arg):
        from MDSplus import Data
        from MitDevices import Dt200WriteMaster
        import numpy
        
        nids=self.node.conglomerate_nids
        try:
            complaint = "Must specify host board number"
            hostboard=int(nids[self.part_names['HOSTBOARD']].record.data())
            complaint = "board must be an integer"
            board=int(nids[self.part_names['BOARD']].record)
            devroot='/dev/ao32cpci/data/ao32cpci.'+str(board)+'/'
            complaint = "clock div must be an integer"
            clock_div= int(nids[self.part_names['CLOCK_DIV']].record)
            complaint = "invalid clock source"
            clock_src=str(nids[self.part_names['CLOCK_SRC']].record.data())
            if not clock_src in self.clock_sources:
                raise Exception, complaint
            complaint = "clock edge must be RISING or FALLING"
            clock_edge=str(nids[self.part_names['CLOCK_EDGE']].record.data())
            if not clock_edge in self.clock_edges:
                raise Exception, complaint
            complaint = 'invalid trigger source'
            trig_src=str(nids[self.part_names['TRIG_SRC']].record.data())
            if not trig_src in self.trig_sources:
                raise Exception, complaint
            complaint = "trig edge must be RISING, FALLING or NONE"
            trig_edge=str(nids[self.part_names['TRIG_EDGE']].record.data())
            if not trig_edge in self.trigger_edges:
                raise Exception, complaint
            complaint = "invalid mode string"
            mode=str(nids[self.part_names['MODE']].record.data())
            if not mode in self.modes:
                raise Exception, complaint
            complaint = 'Error reading dimension information'
            dim=nids[self.part_names['DIM']].record.data()
            complaint = 'Error reading max_samples'
            max_samples=nids[self.part_names['MAX_SAMPLES']].record.data()
            complaint = 'Error reading coninuos flag'
            continuous=nids[self.part_names['CONTINUOUS']].record.data()
        except:
            raise Exception, complaint

        hostname = Dt200WriteMaster(hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)
        hostname = hostname[0].strip()
        
        for i in range(32):
            ao_nid=nids[self.part_names['OUTPUT_01']+i*2]
            fit='LINEAR'
            knots_y = [0.,0.]
            knots_x = [0., 1.]
           
            if ao_nid.on:
                fit_nid = nids[self.part_names['OUTPUT_01']+i*2+1]
                try:
                    fit = str(fit_nid.record.data())
                except:
                    fit = 'LINEAR'
                try:
                    knots_y = ao_nid.record.data()
                    knots_x = ao_nid.record.getDimensionAt(0).data()
                except:
                    knots_y = numpy.array([ 0.,  0.])
                    knots_x = numpy.array([ 0.,  1.])
                    print "Error reading data for channel %d - ZEROING\n" % i
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
            if (continuous):
              commit = '0x02'
            else :
              commit = '0x22' 
            print "set.ao32.data %d commit %s" % (board, commit)        
            Dt200WriteMaster(hostboard, 'set.ao32.data %d commit %s' % (board, commit), 1)        
        except:
            raise Exception, 'error sending commands to AO32 board'

        return 1

    def WriteWaveform(self, host, board, chan, wave):
        from os import popen
        if chan == 1:
            pipe = popen('mkdir -p /tmp/%s/ao32cpci.%d' % (host, board));
            pipe.close()
        file = '/tmp/%s/ao32cpci.%d/f.%2.2d' % (host, board, chan)
        f = open(file, 'wb')
        wave.tofile(f)
        f.close()

    def SendFiles(self, host, hostboard, board):
        from os import popen
        from MitDevices import Dt200WriteMaster
        cmd = '(cd /tmp/%s; tar -czf /tmp/%s.%d.tgz *)' % (host, host, board,)
        print cmd
        pipe = popen(cmd)
        pipe.close()
        cmd = 'curl -s -T /tmp/%s.%d.tgz -u ftp: ftp://%s/' %(host, board, host)
        print cmd
        pipe = popen(cmd)
        pipe.close()
        cmd = 'rm -rf /tmp/%s.%d.tgz; rm -rf /tmp/%s/ao32cpci.%d/' % (host, board, host, board,)
        print cmd
        pipe = os.popen(cmd)
        pipe.close()        
        Dt200WriteMaster(hostboard, '/ffs/unpack_waves %s %d' %(host, board), 1)

    def ORIGINAL_PART_NAME(self,arg):
        return ':'+self.part_names.keys[self.node.conglomerate_elt-1]
    
