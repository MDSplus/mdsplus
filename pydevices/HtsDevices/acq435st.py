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
import numpy
import array
import MDSplus
import tempfile
import threading

def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

class ACQ435ST(MDSplus.Device):
    """
    D-Tacq ACQ435 Digitizer real time streaming support.

    32 Channels
    Minimum 2Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    Hardware averaging / decimation by power of 2 samples
      1,2,4,8,16,32

    Software sample decimation

    Settable segment length in number of samples

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254', 'options':('no_write_shot')},
        {'path':':SITE','type':'numeric', 'value': 1, 'options':('no_write_shot')},
        {'path':':COMMENT','type':'text', 'options':('no_write_shot')},
        {'path':':TRIGGER','type':'numeric', 'value': 0.0, 'options':('no_write_shot')},
        {'path':':TRIG_MODE','type':'text', 'value': 'hard', 'options':('no_write_shot')},
        {'path':':EXT_CLOCK','type':'axis', 'options':('no_write_shot')},
        {'path':':FREQ','type':'numeric', 'value': 16000, 'options':('no_write_shot')},        
        {'path':':HW_FILTER','type':'numeric', 'value':4, 'options':('no_write_shot')},
        {'path':':DEF_DCIM','type':'numeric', 'value': 1, 'options':('no_write_shot')},
        {'path':':SEG_LENGTH','type':'numeric', 'value': 8000, 'options':('no_write_shot')},
        {'path':':MAX_SEGMENTS','type':'numeric', 'value': 1000, 'options':('no_write_shot')},
        {'path':':SEG_EVENT','type':'text', 'value': 'STREAM', 'options':('no_write_shot')},
        {'path':':STATUS_CMDS','type':'text','value':MDSplus.makeArray(['cat /proc/cmdline', 'get.d-tacq.release']),'options':('no_write_shot',)},
        {'path':':STATUS_OUT','type':'signal','options':('write_shot',)},
        {'path':':TRIG_TIME','type':'numeric', 'options':('write_shot')},
        {'path':':TRIG_STR','type':'text', 'options':('nowrite_shot'),
         'valueExpr':"EXT_FUNCTION(None,'ctime',head.TRIG_TIME)"},
        {'path':':RUNNING','type':'numeric', 'options':('no_write_model')},
        {'path':':LOG_FILE','type':'text', 'options':('write_once')},
        {'path':':LOG_OUTPUT','type':'text', 'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head,'auto'))",
         'options':('no_write_shot',)},
        {'path':':STOP_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",
         'options':('no_write_shot',)},
        ]

    for i in range(32):
        parts.append({'path':':INPUT_%2.2d'%(i+1,),'type':'signal','options':('no_write_model','write_once',),
                      'valueExpr':'head.setChanScale(%d)' %(i+1,)})
        parts.append({'path':':INPUT_%2.2d:DECIMATE'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:COEFFICIENT'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        parts.append({'path':':INPUT_%2.2d:OFFSET'%(i+1,),'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
    del i

    debug=None
    data_socket = -1

    trig_types=[ 'hard', 'soft', 'automatic']

    def setChanScale(self,num):
        chan=self.__getattr__('INPUT_%2.2d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT,MDSplus.dVALUE()),chan.OFFSET))

    def init(self):
        acq400_hapi=self.importPyDeviceModule('acq400_hapi')
        from threading import Thread

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('set_abort', '1')
        if self.ext_clock.length > 0:
            uut.s0.set_knob('SYS_CLK_FPMUX', 'FPCLK')
            uut.s0.set_knob('SIG_CLK_MB_FIN', '1000000')
        else:
            uut.s0.set_knob('SYS_CLK_FPMUX', 'ZCLK')
        freq = int(self.freq.data())
        uut.s1.set_knob('ACQ43X_SAMPLE_RATE', "%d"%freq)
        if self.hw_filter.length > 0:
            nacc = int(self.hw_filter.data())
            nacc_samp = 2**nacc
            nacc=('%d'%nacc).strip()
            nacc_samp = ('%d'%nacc_samp).strip()
            uut.s1.set_knob('nacc', '%s,%s'%(nacc_samp, nacc,))
        else :
            uut.s1.set_knob('nacc', '0,0')
        if self.trig_mode.data() == 'hard':
            uut.s1.set_knob('trg', '1,0,1')
        else:
            uut.s1.set_knob('trg', '1,1,1')
#
#  Read the coeffients and offsets
#  for each channel
#    store its' coeff and offset
#       
#        for chan in range(32):
#
        coeffs =  map(float, uut.s1.AI_CAL_ESLO.split(" ")[3:] )
        offsets =  map(float, uut.s1.AI_CAL_EOFF.split(" ")[3:] )       
        for i in range(32):
            coeff = self.__getattr__('input_%2.2d_coefficient'%(i+1))
            coeff.record = coeffs[i]
            offset = self.__getattr__('input_%2.2d_offset'%(i+1))
            offset.record = offsets[i]
        self.running.on=True
        self.stream()
        return 1
    INIT=init

    def stop(self):
        self.running.on = False
        return 1
    STOP=stop

    def trig(self):
        acq400_hapi=self.importPyDeviceModule('acq400_hapi')
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.so.set_knob('soft_trigger','1')
        return 1
    TRIG=trig

    @threaded
    def stream(self):
        import socket
        import numpy as np
        import datetime
        import time
        import os
        import sys
        from MDSplus import Tree,Event,Range

        def lcm(a,b):
            from fractions import gcd
            return (a * b / gcd(int(a), int(b)))

        def lcma(arr):
            ans = 1.
            for e in arr:
                ans = lcm(ans, e)
            return int(ans)

#        os.dup2(self.outputfd.fileno(), sys.stdout.fileno())
#        os.dup2(self.outputfd.fileno(), sys.stderr.fileno())

        print("starting streamer for %s %s %s\nat: %s"%(self.tree, self.tree.shot, self.path, datetime.datetime.now()))

        event_name = self.seg_event.data()
        trig = self.trigger.data()
        if self.hw_filter.length > 0:
            dt = 1./self.freq.data()*2**self.hw_filter.data()
        else:
            dt = 1./self.freq.data()

        chans = []
        decim = []
        for i in range(32):
            chans.append(getattr(self, 'input_%2.2d'%(i+1)))
            decim.append(getattr(self, 'input_%2.2d_decimate' %(i+1)).data())

        decimator = lcma(decim)

        seg_length = self.seg_length.data()
        if seg_length % decimator:
            seg_length = (seg_length // decimator + 1) * decimator

        segment_bytes = seg_length*32*4

        dims=[]
        for i in range(32):
            dims.append(Range(0., (seg_length-1)*dt, dt*decim[i]))

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.node.data(),4210))
        s.settimeout(6)

        chunk = 0
        segment = 0
        running = self.running
        max_segments = self.max_segments.data()
        first = True
        buf = bytearray(segment_bytes)
        while running.on and segment < max_segments:
            toread=segment_bytes 
            try:
                view = memoryview(buf)
                while toread:
                    nbytes = s.recv_into(view, min(4096,toread))
                    if first:
                        self.trig_time.record=time.time()
                        first = False
                    view = view[nbytes:] # slicing views is cheap
                    toread -= nbytes
            except socket.timeout as e:
                print("got a timeout")
                err = e.args[0]
        # this next if/else is a bit redundant, but illustrates how the
        # timeout exception is setup
                if err == 'timed out':
                    time.sleep(1)
                    print ('recv timed out, retry later')
                    if not running.on:
                        break
                    else:
                        continue
                else:
                    print (e)
                    break
            except socket.error as e:
        # Something else happened, handle error, exit, etc.
                print("socket error", e)
                break
            else:
                if toread != 0:
                    print ('orderly shutdown on server end')
                    break
                else:
                    buffer = np.right_shift(np.frombuffer(buf, dtype='int32') , 8)
                    i = 0
#                    coeff = (0.00000119209/2.0)*0.00137093615
#                    coeff = 0.00000119209/2.0
                    for c in chans:
                        if c.on:
                            b = buffer[i::32*decim[i]]
#                            b = np.where(b & 0x400000, b | 0xFF000000, b)
                            c.makeSegment(dims[i].begin, dims[i].ending, dims[i], b)
                            dims[i] = Range(dims[i].begin + seg_length*dt, dims[i].ending + seg_length*dt, dt*decim[i])
                        i += 1
                    segment += 1
                    segment += 1
                    Event.setevent(event_name)
        if self.log_output.on:
            print("%s\tAll Done"%(datetime.datetime.now(),))
            sys.stdout.flush()
#            with  open(self.log_file.data(), 'r') as fd:
#                self.log_output.record = fd.read()

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)
