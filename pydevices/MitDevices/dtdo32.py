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

from MDSplus import Device
from Dt200WriteMaster import Dt200WriteMaster
from os import popen
import numpy

class DTDO32(Device):
    """D-Tacq DO32 Digitial out"""
    parts = [{'path': ':HOSTBOARD',  'type': 'NUMERIC', 'options':('no_write_shot',)},
             {'path': ':BOARD',      'type': 'NUMERIC', 'options':('no_write_shot',)},
             {'path': ':COMMENT',    'type': 'TEXT'},
             {'path': ':TRIG_SRC',   'type': 'TEXT',    'options':('no_write_shot',)},
             {'path': ':TRIG_EDGE',  'type': 'TEXT',    'options':('no_write_shot',)},
             {'path': ':CLOCK_SRC',  'type': 'TEXT',    'options':('no_write_shot',)},
             {'path': ':CLOCK_EDGE', 'type': 'TEXT',    'options':('no_write_shot',)},
             {'path': ':TRIGGER',    'type': 'NUMERIC', 'options':('no_write_shot',)},
             {'path': ':CLOCK',      'type': 'AXIS',    'options':('no_write_shot',)},
             {'path': ':CLOCK_DIV',  'type': 'NUMERIC', 'options':('no_write_shot',)},
             {'path': ':MODE',       'type': 'TEXT',    'options':('no_write_shot',)}]
    for i in range(64):
        parts += [{'path': ":OUTPUT_%2.2d" % (i+1), 'type': 'NUMERIC', 'options':('no_write_model','write_once')}]
    parts += [{'path': ':INIT_ACTION', 'type': 'ACTION', 'options':('no_write_shot','write_once')}]
    clock_edges=('RISING',
                 'FALLING',)
    trigger_edges = clock_edges
    trig_sources=('S_PXI_0',
                  'S_PXI_1',
                  'S_LEMO_CLK_DIRECT',
                  'S_LEMO_CLK_OPTO',
                  'S_PXI_3',
                  'S_PXI_4',
                  'S_LEMO_TRG_DIRECT',
                  'S_LEMO_TRG_OPTO',)
    clock_sources = trig_sources
    clock_sources += ('S_INTERNAL',)
    modes=('M_RIM',
           'M_RTU',
           'M_AWGI',
           'M_AWGT',
           'M_LLI',
           'M_LLC',)

    def init(self,arg):
        try:
            self.hostboard= int(self.HOSTBOARD.data())
            hostname = Dt200WriteMaster(self.hostboard, "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'", 1)[0].strip()
        except: raise Exception("Must specify host board number")
        try:    self.board = int(self.BOARD.data())
        except: raise Exception("board must be an integer")
        try:    clock_div = int(self.CLOCK_DIV.data())
        except: raise Exception("clock div must be an integer")
        try:    clock_src = str(self.CLOCK_SRC.data()).upper()
        except: clock_src = None
        if  not clock_src in self.clock_sources:
            raise Exception("invalid clock source")
        try:    clock_edge = str(self.CLOCK_EDGE.data()).upper()
        except: clock_edge = None
        if  not clock_edge in self.clock_edges:
            raise Exception("clock edge must be RISING or FALLING")
        try:    trig_src = str(self.TRIG_SRC.data()).upper()
        except: trig_src = None
        if  not trig_src in self.trig_sources:
            raise Exception("invalid trigger source")
        try:    trig_edge = str(self.TRIG_EDGE.data()).upper()
        except: trig_edge = None
        if  not trig_edge in self.trigger_edges:
            raise Exception("trig edge must be RISING or FALLING")
        try:    mode = str(self.MODE.data()).upper()
        except: mode = None
        if  not mode in self.modes:
            raise Exception("invalid mode string")
        try:    trigger_time = float(self.TRIGGER.data())
        except: raise Exception("Error reading trigger time")
        try:    clock_rate = 1./float(self.CLOCK.record.delta)
        except: raise Exception("Error reading clock rate - it Must be a simple range")

        for i in range(64):
            do_name = 'OUTPUT_%2.2d' % (i+1)
            do_node = self.getNode(do_name)
            if do_node.on:
                try:
                    times=numpy.float32(do_node.data())
                    times -= trigger_time
                    times *= clock_rate
                    if len(times) < 2 :
                        raise Exception("at least 2 times must be specified for channel %d" % (i+1))
                    self.WriteWaveform(hostname, self.board, i, times)
                except:
                    print "Zeroing DO waveform channel %d" % (i+1)
        try:
            self.SendFiles(hostname, self.hostboard, self.board)
            Dt200WriteMaster(self.hostboard, 'set.ao32 %d DO_CLK  %s %d %s' % (self.board, clock_src, clock_div, clock_edge), 1)
            Dt200WriteMaster(self.hostboard, 'set.ao32 %d DO_TRG %s %s' % (self.board, trig_src, trig_edge), 1)
            Dt200WriteMaster(self.hostboard, 'set.ao32 %d DO_MODE %s' % (self.board, mode), 1)
        except:
            raise Exception, "error sending commands to board"

        return 1

    def arm(self,arg):
        commit = '0x22'
        try:
            Dt200WriteMaster(self.hostboard, 'set.ao32.data %d commit %s' % (self.board, commit), 1)
        except:
            raise Exception("error sending commit to board")


    def WriteWaveform(self, host, board, chan, wave):
        if chan == 0:
            print "comand is mkdir -p /tmp/%s/do32cpci.%d\n" % (host, board)
            pipe = popen('mkdir -p /tmp/%s/do32cpci.%d' % (host, board));
            pipe.close()
        file = '/tmp/%s/do32cpci.%d/d%2.2d' % (host, board, chan)
        f = open(file, 'w')
        if min(wave) < 0:
            print "Channel %d contains negative times - disabling\n" % chan
            raise Exception("Channel %d contains negative times - disabling\n" % chan)
        if min(wave[1:] -wave[:-1]) < 0 :
            print "Channel %d is not strictly increasing - disabling\n" % chan
            raise Exception("Channel %d is not strictly increasing - disabling\n" % chan)

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
    INIT=init

