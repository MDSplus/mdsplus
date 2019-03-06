#
# Copyright (c) 2019, Massachusetts Institute of Technology All rights reserved.
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

class CRYOCON24C(MDSplus.Device):
    """
    8 channel Cryocon temperature monitor

    Each channel (A-D):
      channel_[a-d] - measured resistence
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

    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254',
         'options':('no_write_shot')},
        {'path':':COMMENT','type':'text', 'options':('no_write_shot')},
        {'path':':TREND_EVENT','type':'text', 'value': 'CRYOCON_TREND',
         'options':('no_write_shot')},
        {'path':':DATA_EVENT','type':'text', 'value': 'CRYOCCON_STREAM',
         'options':('no_write_shot')},
        {'path':':STATUS_CMDS','type':'text',
         'value':MDSplus.makeArray(['IDN?',
                                    'SYSTem:HWRev?',
                                    'SYSTem:FWREV?',
                                    'SYSTem:AMBient?']),
         'options':('no_write_shot',)},
        {'path':':STATUS_OUT',
         'type':'any',
         'options':('write_shot','write_once','no_write_model')},
        {'path':':SEG_LENGTH',
         'type':'numeric',
         'value':5,
         'options':('no_write_shot')},
        {'path':':MAX_SEGMENTS',
         'type':'numeric',
         'value':200,
         'options':('no_write_shot')},
        {'path':':RATE',
         'type':'numeric',
         'value':.25,
         'options':('no_write_shot')},
        {'path':':TRIG_TIME','type':'numeric', 'options':('write_shot')},
        {'path':':TRIG_STR','type':'text', 'options':('nowrite_shot'),
         'valueExpr':"EXT_FUNCTION(None,'ctime',head.TRIG_TIME)"},
        {'path':':RUNNING','type':'numeric', 'options':('no_write_model')},
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':
             "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))",
         'options':('no_write_shot',)},
        {'path':':STOP_ACTION','type':'action',
         'valueExpr':
             "Action(Dispatch('S','STORE',50,None),Method(None,'STOP',head))",
         'options':('no_write_shot',)},
        ]

    for c in range(ord('A'), ord('E')):
        parts.append({'path':':INPUT_%c'%(c,),
                      'type':'signal',
                      'options':('no_write_model','write_once',)})
        parts.append({'path':':INPUT_%c:SERIAL_NO'%(c,),
                      'type':'TEXT',
                      'options':('no_write_shot')})
        parts.append({'path':':INPUT_%c:CALIBRATION'%(c,),
                      'type':'TEXT',
                      'options':('no_write_model', 'write_once',)})
        parts.append({'path':':INPUT_%c:TEMPERATURE'%(c,),
                      'type':'SIGNAL',
                      'options':('no_write_model', 'write_once',)})
    del c

    debug=None

    def query(self, cmd):
        import pyvisa
        rm = pyvisa.ResourceManager('@py')
        instrument = rm.open_resource('TCPIP::%s'% str(self.node.data()))
        answer = instrument.query(cmd)
        print("cmd:%s\nans:%s"%(cmd,answer))
        return 1
    QUERY=query

    class Worker(threading.Thread):
        """An async worker should be a proper class
           This ensures that the methods remian in memory
           It should at least take one argument: teh device node
        """
        def __init__(self,dev):
            print(MDSplus.__file__)
            print(MDSplus.__version__)
            super(CRYOCON24C.Worker,self).__init__(name=dev.path)
            # make a thread safe copy of the device node with a non-global context
            self.dev = MDSplus.TreeNode.copy(dev)
        def run(self):
            # self.dev.stream()
            self.dev.trend()
    def init(self):
        '''
        init method for cryocon 24c

        record the values of the status commands
        start the stream

        STILL NEEDS TO STORE Calibration CURVES !!
        '''
        import pyvisa
        # open the instrument
        if self.debugging():
            print("about to open cryocon device %s" % str(self.node.data()))
        rm = pyvisa.ResourceManager('@py')
        instrument = rm.open_resource('TCPIP::%s'% str(self.node.data()))
        # read and save the status commands
        status_out={}
        for cmd in self.status_cmds:
            if self.debugging():
                print('about to send %s' % cmd)
            status_out[str(cmd)] = instrument.query(str(cmd))
            if self.debugging():
                print('  got back %s' % status_out[str(cmd)])
        self.status_out.record = status_out

        for i in range(ord('a'), ord('e')):
            chan = self.__getattr__('input_%c'%(chr(i),))
            if chan.on:
              cal = self.__getattr__('input_%c_calibration'%(chr(i),))
              ans = instrument.query('CALCUR %c?'%(chr(i)))
              print(ans)
              cal.record = ans
        print(instrument.query('CALCUR'))
        # start it streaming
        self.running.on=True
        # self.stream()
        thread = self.Worker(self)
        thread.start()
        return 1
    INIT=init

    def trend(self):
        '''
        trend method for cryocon 24c

        Store one sample for each of the channels that is on using
        putRow.  The timestamp will be time since the unix EPOCH in msec
        '''

        import pyvisa
        import MDSplus
        import time

        # open the instrument
        if self.debugging():
            print("about to open cryocon device %s" % str(self.node.data()))
        event_name = self.data_event.data()
        rm = pyvisa.ResourceManager('@py')
        instrument = rm.open_resource('TCPIP::%s'% str(self.node.data()))
        chans = []
        t_chans = []
        resists = []
        temps = []
        query_cmd = ''
        for i in range(ord('a'), ord('e')):
            chan = self.__getattr__('input_%c'%(chr(i),))
            if chan.on:
                t_chan=self.__getattr__('input_%c_temperature' % (chr(i)))
                query_cmd = 'INP %c?;INP %c:SENP?;'%(chr(i), chr(i),)
                ans = instrument.query(query_cmd).split(';')
                t_time=time.time()
                try:
                    print("Parsing temperature")
                    temp = float(ans[0].split('\x00')[0])
                except:
                    if self.debugging():
                        print("Could not parse temperature /%s/"%
                           ans[0].split('\x00')[0])
                    temp = 0.0

                t_chan.putRow(1000,
                              MDSplus.Float32(temp),
                               MDSplus.Int64(t_time*1000.))

                try:
                    resist = float(ans[1].split('\x00')[0])
                except:
                    if self.debugging():
                        print("Could not parse resist /%s/"%
                               ans[1].split('\x00')[0])
                    resist = 0.0

                chan.putRow(1000,
                          MDSplus.Float32(resist),
                          MDSplus.Int64(t_time*1000.))

        MDSplus.Event.setevent(event_name)
    TREND=trend

    def stop(self):
        '''
        stop method for the cryocon 24c

        set the stop flag by turning off the 'running' node
        '''
        self.running.on = False
        return 1
    STOP=stop

    def stream(self):
        import pyvisa
        import datetime
        import time
        import numpy as np
        import MDSplus

        try:
            dt = 1./float(self.rate)
        except:
            dt = 1
        print("starting streamer for %s %s %s\nat: %s"%
              (self.tree, self.tree.shot, self.path, datetime.datetime.now()))
        event_name = self.data_event.data()
        seg_length = self.seg_length.data()

        # open the instrument
        rm = pyvisa.ResourceManager('@py')
        instrument = rm.open_resource('TCPIP::%s'% str(self.node.data()))

        # set up arrays of data and nodes to use in the loop
        seg_length = int(self.seg_length.data())
        max_segments = self.max_segments.data()
        chans = []
        t_chans = []
        resists = []
        temps = []
        times = np.zeros(seg_length)
        query_cmd = ''
        for i in range(ord('a'), ord('e')):
            chan = self.__getattr__('input_%c'%(chr(i),))
            if chan.on:
                temps.append(np.zeros(seg_length))
                resists.append(np.zeros(seg_length))
                chans.append(chan)
                t_chans.append(self.__getattr__('input_%c_temperature' % (chr(i))))
                query_cmd = query_cmd+'INP %c?;INP %c:SENP?;'%(chr(i), chr(i),)
        # note the time
        # while not stopped and not done
        #    for each sample in segment
        #        read all temps and resists
        #        for each channel
        #            make the temp and resist a float
        #    trim segment if necessary
        #    write the segment
        segment = 0
        start_time = time.time()
        previous_time = 0
        self.trig_time.record = start_time
        while self.running.on and segment < max_segments:
            if self.debugging():
                print ("starting on segment %d" % segment)
            for sample in range(seg_length):
                if not self.running.on:
                    break
                if previous_time != 0:
                    time.sleep(dt - (time.time()-previous_time))
                ans = instrument.query(query_cmd).split(';')
                previous_time = time.time()
                times[sample] = previous_time - start_time
                for i in range(len(temps)):
                    try:
                        temps[i][sample] = float(ans[2*i].split('\x00')[0])
                    except:
                        if self.debugging():
                            print("Could not parse temperature /%s/"%
                                   ans[2*i].split('\x00')[0])
                        temps[i][sample] = 0.0
                    try:
                        resists[i][sample] = float(ans[2*i+1].split('\x00')[0])
                    except:
                        if self.debugging():
                            print("Could not parse resist /%s/"%
                                   ans[2*i+1].split('\x00')[0])
                        resists[i][sample] = 0.0

            if sample != seg_length-1:
                for i in range(len(temps)):
                    times = times[0:sample]
                    temps[i] = temps[i][0:sample]
                    resists[i] = resists[i][0:sample]
            for i in range(len(temps)):
                chans[i].makeSegment(times[0],
                                     times[-1],
                                     times,
                                     resists[i])
                t_chans[i].makeSegment(times[0],
                                       times[-1],
                                       times,
                                       temps[i])
                MDSplus.Event.setevent(event_name)
            segment += 1
        return 1

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)
