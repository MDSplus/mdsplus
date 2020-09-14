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
import numpy as np
import MDSplus
import threading
import sys
if sys.version_info < (3,):
    from Queue import Queue, Empty
else:
    from queue import Queue, Empty


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

    debug - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    parts=[
        {'path':':NODE','type':'text','value':'192.168.0.254', 'options':('no_write_shot',)},
        {'path':':SITE','type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':COMMENT','type':'text', 'options':('no_write_shot',)},
        {'path':':TRIGGER','type':'numeric', 'value': 0.0, 'options':('no_write_shot',)},
        {'path':':TRIG_MODE','type':'text', 'value': 'hard', 'options':('no_write_shot',)},
        {'path':':EXT_CLOCK','type':'axis', 'options':('no_write_shot',)},
        {'path':':FREQ','type':'numeric', 'value': 16000, 'options':('no_write_shot',)},
        {'path':':HW_FILTER','type':'numeric', 'value':0, 'options':('no_write_shot',)},
        {'path':':DEF_DCIM','type':'numeric', 'value': 1, 'options':('no_write_shot',)},
        {'path':':SEG_LENGTH','type':'numeric', 'value': 8000, 'options':('no_write_shot',)},
        {'path':':MAX_SEGMENTS','type':'numeric', 'value': 1000, 'options':('no_write_shot',)},
        {'path':':SEG_EVENT','type':'text', 'value': 'STREAM', 'options':('no_write_shot',)},
        {'path':':STATUS_CMDS','type':'text','value':MDSplus.makeArray(['cat /proc/cmdline', 'get.d-tacq.release']),'options':('no_write_shot',)},
        {'path':':STATUS_OUT','type':'signal','options':('write_shot',)},
        {'path':':TRIG_TIME','type':'numeric', 'options':('write_shot',)},
        {'path':':TRIG_STR','type':'text', 'options':('nowrite_shot',),'valueExpr':"EXT_FUNCTION(None,'ctime',head.TRIG_TIME)"},
        {'path':':RUNNING','type':'any', 'options':('no_write_model',)},
        {'path':':LOG_OUTPUT','type':'text', 'options':('no_write_model','write_once','write_shot')},
        {'path':':GIVEUP_TIME', 'type': 'numeric', 'value': 180.0, 'options': ('no_write_shot',)},
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

    trig_types=[ 'hard', 'soft', 'automatic']

    class MDSWorker(threading.Thread):
        NUM_BUFFERS = 20

        def __init__(self,dev):
            super(ACQ435ST.MDSWorker,self).__init__(name=dev.path)
            threading.Thread.__init__(self)

            self.dev = dev.copy()

            self.chans = []
            self.decim = []
            self.nchans = 32

            for i in range(self.nchans):
                self.chans.append(getattr(self.dev, 'input_%2.2d'%(i+1)))
                self.decim.append(getattr(self.dev, 'input_%2.2d_decimate' %(i+1)).data())

            self.seg_length = self.dev.seg_length.data()
            self.segment_bytes = self.seg_length*self.nchans*np.int32(0).nbytes

            self.empty_buffers = Queue()
            self.full_buffers = Queue()

            for i in range(self.NUM_BUFFERS):
                self.empty_buffers.put(bytearray(self.segment_bytes))

            self.device_thread = self.DeviceWorker(self)

        def run(self):
            import acq400_hapi
            import ast
            uut = acq400_hapi.Acq400(self.dev.node.data(), monitor=False)

            def lcm(a,b):
                from fractions import gcd
                return (a * b / gcd(int(a), int(b)))

            def lcma(arr):
                ans = 1.
                for e in arr:
                    ans = lcm(ans, e)
                return int(ans)

            if self.dev.debug:
                print("MDSWorker running")

            event_name = self.dev.seg_event.data()

            trig = self.dev.trigger.data()
            
            # Retrive the actual value of NACC (samples) already set in the ACQ box
            nacc_str   = uut.s1.get_knob('nacc')
            if nacc_str == '0,0,0':
                nacc_sample = 1
            else:
                nacc_tuple  = ast.literal_eval(nacc_str)
                nacc_sample = nacc_tuple[0]
                
            if self.dev.debug:
                print("The ACQ NACC sample value was set to {}".format(nacc_sample))

            # nacc_sample values are always between 1 and 32, set in the ACQ box by the device INIT() function, therefore:
            dt = 1./self.dev.freq.data() * nacc_sample

            decimator = lcma(self.decim)

            if self.seg_length % decimator:
                 self.seg_length = (self.seg_length // decimator + 1) * decimator

            self.device_thread.start()

            segment = 0
            first = True
            running = self.dev.running
            max_segments = self.dev.max_segments.data()
            while running.on and segment < max_segments:
                try:
                    buf = self.full_buffers.get(block=True, timeout=1)
                except Empty:
                    continue

                buffer = np.right_shift(np.frombuffer(buf, dtype='int32') , 8)
                i = 0
                for c in self.chans:
                    slength = self.seg_length/self.decim[i]
                    deltat  = dt * self.decim[i]
                    if c.on:
                        b = buffer[i::self.nchans*self.decim[i]]
                        begin = segment * slength * deltat
                        end   = begin + (slength - 1) * deltat
                        dim   = MDSplus.Range(begin, end, deltat)
                        c.makeSegment(begin, end, dim, b)
                    i += 1
                segment += 1
                MDSplus.Event.setevent(event_name)

                self.empty_buffers.put(buf)

            self.dev.trig_time.record = self.device_thread.trig_time - ((self.device_thread.io_buffer_size / np.int32(0).nbytes) * dt)
            self.device_thread.stop()

        class DeviceWorker(threading.Thread):
            running = False

            def __init__(self,mds):
                threading.Thread.__init__(self)
                self.debug = mds.dev.debug
                self.node_addr = mds.dev.node.data()
                self.seg_length = mds.dev.seg_length.data()
                self.segment_bytes = mds.segment_bytes
                self.freq = mds.dev.freq.data()
                self.nchans = mds.nchans
                self.empty_buffers = mds.empty_buffers
                self.full_buffers = mds.full_buffers
                self.trig_time = 0
                self.io_buffer_size = 4096

            def stop(self):
                self.running = False

            def run(self):
                import socket
                import time
                if self.debug:
                    print("DeviceWorker running")

                self.running = True
                first = True
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((self.node_addr,4210))
                s.settimeout(6)

                # trigger time out count initialization:
                while self.running:
                    try:
                        buf = self.empty_buffers.get(block=False)
                    except Empty:
                        print("NO BUFFERS AVAILABLE. MAKING NEW ONE")
                        buf = bytearray(self.segment_bytes)

                    toread = self.segment_bytes
                    try:
                        view = memoryview(buf)
                        while toread:
                            nbytes = s.recv_into(view, min(self.io_buffer_size,toread))
                            if first:
                                self.trig_time = time.time()
                                first = False
                            view = view[nbytes:] # slicing views is cheap
                            toread -= nbytes

                    except socket.timeout as e:
                        print("We have Got a timeout.")
                        err = e.args[0]
                        # this next if/else is a bit redundant, but illustrates how the
                        # timeout exception is setup

                        if err == 'timed out':
                            time.sleep(1)
                            print (' received timed out, retry later')
                            continue
                        else:
                            print (e)
                            break
                    except socket.error as e:
                        # Something else happened, handle error, exit, etc.
                        print("socket error", e)
                        self.full_buffers.put(buf[:self.segment_bytes-toread])
                        break
                    else:
                        if toread != 0:
                            print ('orderly shutdown on server end')
                            break
                        else:
                            self.full_buffers.put(buf)

    def setChanScale(self,num):
        chan=self.__getattr__('INPUT_%2.2d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT,MDSplus.dVALUE()),chan.OFFSET))

    def init(self):
        import os
        import sys
        from threading import Thread
        import tempfile
        import subprocess
        import acq400_hapi

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('set_abort', '1')

        if self.ext_clock.length > 0:
            raise Exception('External Clock is not supported')

        trg = self.trig_mode.data()

        if trg == 'hard':
            trg_dx = 'd0'
        elif trg == 'automatic':
            trg_dx = 'd1'
        elif trg == 'soft':
            trg_dx = 'd1'

        # USAGE sync_role {fpmaster|rpmaster|master|slave|solo} [CLKHZ] [FIN]
        # modifiers [CLK|TRG:SENSE=falling|rising] [CLK|TRG:DX=d0|d1]
        # modifiers [TRG=int|ext]
        # modifiers [CLKDIV=div]  
        # If the user has specified a trigger.
        uut.s0.sync_role = '%s %s TRG:DX=%s' % ('master', self.freq.data(), trg_dx)

        if self.debug:
            print("Hardware Filter (NACC) from tree node is {}".format(int(self.hw_filter.data())))

        # Hardware Filter: Accumulate/Decimate filter. Accumulate nacc_samp samples, then output one value.
        nacc_samp = int(self.hw_filter.data())
        if 1 <= nacc_samp <= 32:
            uut.s1.nacc = ('%d'%nacc_samp).strip()
        else:
            print("WARNING: Hardware Filter samples must be in the range [0,32]. 0 => Disabled == 1")
            uut.s1.nacc = '1'
#
#  Read the coeffients and offsets
#  for each channel
#    store its' coeff and offset
#
#        for chan in range(32):
#
        coeffs  =  map(float, uut.s1.AI_CAL_ESLO.split(" ")[3:] )
        offsets =  map(float, uut.s1.AI_CAL_EOFF.split(" ")[3:] )

        for i in range(32):
            coeff = self.__getattr__('input_%2.2d_coefficient'%(i+1))
            coeff.record = coeffs[i]
            offset = self.__getattr__('input_%2.2d_offset'%(i+1))
            offset.record = offsets[i]
        self.running.on=True
        thread = self.MDSWorker(self)
        thread.start()
    INIT=init

    def stop(self):
        self.running.on = False
    STOP=stop

    def trig(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.set_knob('soft_trigger','1')
    TRIG=trig
