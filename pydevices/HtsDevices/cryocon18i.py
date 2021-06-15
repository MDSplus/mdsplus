#
# Copyright (c) 2018, Massachusetts Institute of Technology All rights reserved.
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
import MDSplus
import threading
import socket
import time
import datetime
import numpy as np

if MDSplus.version.ispy2:
    def tostr(x):
        return x

    def tobytes(x):
        return x
else:
    def tostr(x):
        return b if isinstance(b, str) else b.decode('utf-8')

    def tobytes(x):
        return s if isinstance(s, bytes) else s.encode('utf-8')


class CRYOCON18I(MDSplus.Device):
    """
    8 channel Cryocon temperature monitor

    Each channel (A-H):
      channel_[a-h] - measured resistence
                     (for trend timestamp record, for daq segmented record)
      serial_no - serial number of the sensor
      calibration - 2d array of calibration values read from device
      temperature - measured temperature
                   (for trend timestamp record, for daq segmented record)

    Methods:
       init - start daq loop
       stop - stop daq loop
       trend - store one trend sample for each channel

    debugging() - is debugging enabled.
                  Controlled by environment variable DEBUG_DEVICES

    """

    inputs = "abcdefgh"

    parts = [
        {'path': ':COMMENT', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':RUNNING', 'type': 'numeric', 'options': ('no_write_model')},
        {'path': ':INIT_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))", 'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('S','STOP',50,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)},
    ]
    for i in inputs.upper():
        parts.extend([
            {'path': ':INPUT_%c' % (i,), 'type': 'signal', 'options': (
                'no_write_model', 'write_once',)},
            {'path': ':INPUT_%c:RESISTENCE' % (i,), 'type': 'SIGNAL', 'options': (
                'no_write_model', 'write_once',)},
            {'path': ':INPUT_%c:CALIBRATION' % (i,), 'type': 'TEXT', 'options': (
                'no_write_model', 'write_once',)},
        ])
    del(i)
    debug = None

    def debugging(self):
        import os
        if self.debug == None:
            self.debug = os.getenv("DEBUG_DEVICES")
        return(self.debug)


class CRYOCON18I_TREND(CRYOCON18I):

    parts = CRYOCON18I.parts + [
        {'path': ':NODE', 'type': 'text', 'value': '',
            'options': ('no_write_shot')},
        {'path': ':DATA_EVENT', 'type': 'text',
            'value': 'CRYOCON18I_TREND', 'options': ('no_write_shot')},
        {'path': ':STATUS_CMDS', 'type': 'text', 'value': MDSplus.makeArray(
            ['*IDN?', 'SYSTem:HWRev?', 'SYSTem:FWREV?', 'SYSTem:AMBient?']), 'options':('no_write_shot',)},
        {'path': ':STATUS_OUT', 'type': 'any', 'options': (
            'write_shot', 'write_once', 'no_write_model')},
    ]

    def sendCommand(self, s, cmd):
        s.send(tobytes(cmd + "\r\n"))

    def recvResponse(self, s):
        msg = ""
        while True:
            c = tostr(s.recv(1))
            if c == "\r":
                continue
            if c == "\n":
                break
            msg += c
        return msg

    def queryCommand(self, s, cmd):
        self.sendCommand(s, cmd)
        return self.recvResponse(s)

    def init(self):
        '''
        init method for cryocon 18i
        '''
        self.trend()
        return 1
    INIT = init

    def trend(self):
        '''
        trend method for cryocon18i

        record the values of the status commands
        start the stream

        STILL NEEDS TO STORE Calibration CURVES !!

        Store one sample for each of the channels that is on using
        putRow.  The timestamp will be time since the unix EPOCH in msec
        '''

        # start it trending
        self.running.on = True

        event_name = self.data_event.data()

        if self.debugging():
            print("About to open cryocon device %s" % str(self.node.data()))

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((str(self.node.data()), 5000))

        try:
            self.status_out.getData()
        except:
            # read and save the status commands
            status_out = {}
            for cmd in self.status_cmds:
                if self.debugging():
                    print('about to send %s' % cmd)
                status_out[str(cmd)] = self.queryCommand(s, str(cmd))
                if self.debugging():
                    print('  got back %s' % status_out[str(cmd)])
            self.status_out.record = status_out

            # for i in range(ord('a'), ord('h')+1+1):
            #    chan = self.__getattr__('input_%c'%(chr(i),))
            #    if chan.on:
            #      cal = self.__getattr__('input_%c_calibration'%(chr(i),))
            #      ans = self.queryCommand(s, 'CALCUR %c?'%(chr(i)))
            #      print(ans)
            #      cal.record = ans
            #print(self.queryCommand(s, 'CALCUR'))

        query_cmd = ''
        ansQuery = []
        for i in self.inputs:
            t_chan = self.__getattr__('input_%c' % (i,))
            if t_chan.on:
                query_cmd = 'INP %c?;INP %c:SENP?;' % (i, i)
                ansQuery = self.queryCommand(s, query_cmd)[:-1].split(';')
                t_time = time.time()

                try:
                    temp = float(ansQuery[0])
                except:
                    if self.debugging():
                        print("Could not parse temperature /%s/" % ansQuery[0])
                    temp = 0.0

                t_chan.putRow(1000, MDSplus.Float32(temp),
                              MDSplus.Int64(t_time*1000.))

                r_chan = self.__getattr__('input_%c_resistence' % (i,))
                try:
                    resist = float(ansQuery[1])
                except:
                    if self.debugging():
                        print("Could not parse resist /%s/" % ansQuery[1])
                    resist = 0.0

                r_chan.putRow(1000, MDSplus.Float32(resist),
                              MDSplus.Int64(t_time*1000.))

        MDSplus.Event.setevent(event_name)
        s.close()
    TREND = trend

    def stop(self):
        '''
        stop method for the cryocon 18i
        set the stop flag by turning off the 'running' node
        '''
        self.running.on = False
        return 1
    STOP = stop


class CRYOCON18I_SHOT(CRYOCON18I):

    parts = CRYOCON18I.parts + [
        {'path': ':DATA_EVENT', 'type': 'text',
            'value': 'CRYOCON18I_DATA', 'options': ('no_write_shot')},
        {'path': ':T1', 'type': 'numeric', 'options': (
            'no_write_shot'), 'help': 'The time in seconds that the shot began taking data'},
        {'path': ':T2', 'type': 'numeric',
            'value': 0, 'options': ('write_shot')},

        {'path': ':TREND_TREE', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':TREND_DEVICE', 'type': 'text',
            'options': ('no_write_shot')},
        {'path': ':TREND_SHOT', 'type': 'numeric', 'value': 0, 'options': (
            'write_shot'), 'help': 'The record of the shot number of the trend this data came from'},
    ]

    def getTrendTree(self):
        tree_name = self.trend_tree.data()
        shot_number = self.trend_shot.data()
        trend_tree = MDSplus.Tree(tree_name, shot_number, 'NORMAL')
        return trend_tree

    def init(self):
        # Place holder
        pass
    INIT = init

    def store(self):
        event_name = self.data_event.data()
        now = time.time()
        if self.t2.rlength == 0:
            self.t2.record = MDSplus.Int64(now)
        trend_tree = self.getTrendTree()
        trend_dev = trend_tree.getNode(self.trend_device.data())

        # Getting T1 from TREND:
        t1 = MDSplus.Int64(self.t1.data()*1000.)

        # Saving TREND shot number information into the tree:
        self.trend_shot.record = trend_tree.getCurrent(self.trend_tree.data())

        # Set Time Context
        trend_tree.setTimeContext(t1, MDSplus.Int64(self.t2.data()*1000.))

        print('Writing data into shot node')
        for i in self.inputs:
            trend_temp = trend_dev.__getattr__('input_%c' % (i,))
            trend_resis = trend_dev.__getattr__('input_%c_resistence' % (i,))

            times = trend_temp.dim_of().data()
            temps = trend_temp.data()
            resists = trend_resis.data()

            start_time = times[0]
            for j in range(len(times)):
                times[j] -= start_time
                times[j] = float(times[j]) / 1000.

            shot_temp = self.__getattr__('input_%c' % (i,))
            shot_resis = self.__getattr__('input_%c_resistence' % (i,))

            shot_temp.record = MDSplus.Signal(temps, None, times)
            shot_resis.record = MDSplus.Signal(resists, None, times)

        MDSplus.Event.setevent(event_name)
    STORE = store
