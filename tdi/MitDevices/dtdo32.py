from MDSplus import Device
from Dt200WriteMaster import Dt200WriteMaster
from os import popen
import numpy

class DTDO32(Device):
    """D-Tacq DO32 Digitial out"""
    part_names = (':HOSTBOARD',
                  ':BOARD',
                  ':COMMENT',
                  ':TRIG_SRC',
                  ':TRIG_EDGE',
                  ':CLOCK_SRC',
                  ':CLOCK_EDGE',
                  ':TRIGGER',
                  ':CLOCK',
                  ':CLOCK_DIV',
                  ':MODE',
                 )
    for i in range(64):
         part_names += (":OUTPUT_%2.2d" % (i+1),)

    part_names += (':INIT_ACTION',)

    clock_edges=['RISING', 'FALLING']
    trigger_edges = clock_edges
    trig_sources=('S_PXI_0',
                   'S_PXI_1',
                   'S_LEMO_CLK_DIRECT',
                   'S_LEMO_CLK_OPTO',
                   'S_PXI_3',
                   'S_PXI_4',
                   'S_LEMO_TRG_DIRECT',
                   'S_LEMO_TRG_OPTO',
                   )
    clock_sources = trig_sources
    clock_sources += ('S_INTERNAL',)
    modes=('M_RIM',
            'M_RTU',
            'M_AWGI',
            'M_AWGT',
            'M_LLI',
            'M_LLC',
          )

    def init(self,arg):
        try:
            complaint = "Must specify host board number"
            hostboard=int(self.hostboard.data())
            complaint = "board must be an integer"
            board = int(self.board.data())
            complaint = "clock div must be an integer"
            clock_div = int(self.clock_div.data())
            complaint = "invalid clock source"
            clock_src = str(self.clock_src.data())
            if not clock_src in self.clock_sources:
                raise Exception, complaint
            complaint = "clock edge must be RISING or FALLING"
            clock_edge = str(self.clock_edge.data())
            if not clock_edge in self.clock_edges:
                raise Exception, complaint
            complaint = 'invalid trigger source'
            trig_src = str(self.trig_src.data())
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
            complaint = "Error reading trigger time"
            trigger_time = float(self.trigger.data())
            complaint = "Error reading clock rate - it Must be a simple range"
            clock_rate = 1./self.clock.record.delta
        except:
            raise Exception, complaint

        hostname = Dt200WriteMaster(hostboard, 
                                    "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 
                                    1)
        hostname = hostname[0].strip()
        
        for i in range(64):
            do_name = 'self.output_%2.2d' % (i+1)
            do_node = eval('self.output_%2.2d' % (i+1))
            if do_node.on:
                try:
                    times=numpy.float32(do_node.data())
                    times -= trigger_time
                    times *= clock_rate
                    if len(times) < 2 :
                        raise Exception, "at least 2 times must be specified for channel %d" % (i+1)
                    self.WriteWaveform(hostname, board, i, times)
                except:
                    print "Zeroing DO waveform channel %d" % (i+1)
        try:
            self.SendFiles(hostname, hostboard, board)
            Dt200WriteMaster(hostboard, 'set.ao32 %d DO_CLK  %s %d %s' % (board, clock_src, clock_div, clock_edge), 1) 
            Dt200WriteMaster(hostboard, 'set.ao32 %d DO_TRG %s %s' % (board, trig_src, trig_edge), 1)
            Dt200WriteMaster(hostboard, 'set.ao32 %d DO_MODE %s' % (board, mode), 1)
        except:
            raise Exception, "error sending commands to board"

        return 1
                        
    def arm(self,arg):
        commit = '0x22'
        try:
            Dt200WriteMaster(hostboard, 'set.ao32.data %d commit %s' % (board, commit), 1)
        except:
            raise Exception, "error sending commit to board"


    def WriteWaveform(self, host, board, chan, wave):
        if chan == 0:
            print "comand is mkdir -p /tmp/%s/do32cpci.%d\n" % (host, board)
            pipe = popen('mkdir -p /tmp/%s/do32cpci.%d' % (host, board));
            pipe.close()
        file = '/tmp/%s/do32cpci.%d/d%2.2d' % (host, board, chan)
        f = open(file, 'w')
        if min(wave) < 0:
            print "Channel %d contains negative times - disabling\n" % chan
            raise Execption, "Channel %d contains negative times - disabling\n" % chan
        if min(wave[1:] -wave[:-1]) < 0 :
            print "Channel %d is not strictly increasing - disabling\n" % chan
            raise Execption, "Channel %d is not strictly increasing - disabling\n" % chan

        num = len(wave)/2
        if num*2 != len(wave) :
            print "Channel %d has odd number of times, ignoring last value\n" % chan

        if chan == 1:
            pipe = popen('mkdir -p /tmp/%s/do32cpci.%d' % (host, board));
            pipe.close()
        file = '/tmp/%s/do32cpci.%d/d%2.2d' % (host, board, chan)
        f = open(file, 'w')

        for i in range(num) :
            f.write("%d, %d\n" % (int(wave[i*2]), int(wave[i*2+1]-wave[i*2])))

        f.close()

    def SendFiles(self, host, hostboard, board):
        print "sending files\n"
        cmd = '(cd /tmp/%s; tar -czf /tmp/%s.%d.tgz do32cpci.%d)' % (host, host, board, board,)
        print cmd
        pipe = popen(cmd)
        pipe.close()
        cmd = 'curl -s -T /tmp/%s.%d.tgz -u ftp: ftp://%s/' %(host, board, host)
        print cmd
        pipe = popen(cmd)
        pipe.close()
        cmd = 'rm -rf /tmp/%s.%d.tgz; rm -rf /tmp/%s/do32cpci.%d/' % (host, board, host, board,)
        print cmd
        pipe = popen(cmd)
        pipe.close()        
        Dt200WriteMaster(hostboard, '/ffs/unpack_d_waves %s %d' %(host, board), 1)

