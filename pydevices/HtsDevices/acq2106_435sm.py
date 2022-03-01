#
# Copyright (c) 2022, Massachusetts Institute of Technology All rights reserved.
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
import socket
import threading
import time
import numpy as np

class _ACQ2106_435SM(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ435 Digitizers (up to 6) opperating as Slow Monitors.

    32 Channels * number of slots
    Minimum 10Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    carrier_parts = [
        {
            'path': ':NODE',        
            'type': 'text',
            'value': '192.168.0.254',
            'options': ('no_write_shot',)
        },
        {
            'path': ':COMMENT',     
            'type': 'text',
            'options': ('no_write_shot',)
        },
        {
            'path': ':DATA_EVENT',   
            'type': 'text',
            'value': 'SMDATA', 
            'options': ('no_write_shot',)
        },
        {
            'path': ':RUNNING',     
            'type': 'numeric',
            'options': ('no_write_model',)
        },
        {
            'path': ':INIT_ACTION', 
            'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head,'auto'))", 
            'options': ('no_write_shot',)
        },
        {
            'path': ':STOP_ACTION', 
            'type': 'action',
            'valueExpr': "Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",      
            'options': ('no_write_shot',)
        },
    ]

    class Worker(threading.Thread):
        def __init__(self, dev):
            super(_ACQ2106_435SM.Worker, self).__init__(name=dev.path)
            
            self.dev = dev


        def run(self):

            event_name = self.dev.data_event.data()

            nchans = self.dev.sites * 32

            chans = []
            for i in range(nchans):
                chans.append(getattr(self.dev, 'input_%3.3d' % (i + 1)))

            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.dev.node.data(), 53666))
            s.settimeout(6)

            buffer = bytearray(nchans * np.int32(0).nbytes)

            while self.dev.running.on:
                try:
                    nbytes = s.recv_into(buffer, len(buffer))
                    if nbytes != len(buffer):
                        print("Unable to read bytes.")

                    now = time.time()
                    samples = np.frombuffer(buffer, dtype='int32')
                    for i, c in enumerate(chans):
                        if c.on:
                            c.putRow(1000, samples[i], now)

                    MDSplus.Event.setevent(event_name)

                except socket.timeout as e:
                    print("Got a timeout.")
                    err = e.args[0]
                    # this next if/else is a bit redundant, but illustrates how the
                    # timeout exception is setup

                    if err == 'timed out':
                        time.sleep(1)
                        print (' recv timed out, retry later')
                        continue
                    else:
                        print (e)
                        break
                except socket.error as e:
                    # Something else happened, handle error, exit, etc.
                    print("socket error", e)
                    break
    
    def init(self):
        uut = self.getUUT()

        # Fetching all calibration information from every channel.
        uut.fetch_all_calibration()
        coeffs = uut.cal_eslo[1:]
        eoff   = uut.cal_eoff[1:]

        chans = []
        nchans = self.sites * 32
        
        if nchans != uut.nchan():
            print("Number of channels mismatch %d != %d" % (nchans, uut.nchan()))

        for i in range(nchans):
            chans.append(getattr(self, 'INPUT_%3.3d' % (i + 1)))

        for i, ch in enumerate(chans):
            if ch.on:
                ch.OFFSET.putData(float(eoff[i]))
                ch.COEFFICIENT.putData(float(coeffs[i]))

        self.running.on = True

        thread = self.Worker(self)
        thread.start()

    INIT = init

    def stop(self):
        self.running.on = False
    STOP = stop

    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        return uut

    def setChanScale(self, num):
        chan = self.__getattr__('INPUT_%3.3d' % num)
        chan.setSegmentScale(MDSplus.ADD(MDSplus.MULTIPLY(chan.COEFFICIENT, MDSplus.dVALUE()), chan.OFFSET))

def assemble(cls):
    cls.parts = list(_ACQ2106_435SM.carrier_parts)
    for i in range(cls.sites * 32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i + 1,),            
                'type': 'SIGNAL', 'valueExpr': 'head.setChanScale(%d)' % (i + 1,), 
                'options': ('no_write_model',)
            },
            {
                'path': ':INPUT_%3.3d:COEFFICIENT' % (i + 1,), 
                'type': 'NUMERIC',
                'options': ('no_write_model', 'write_once',)
            },
            {
                'path': ':INPUT_%3.3d:OFFSET' % (i + 1,),    
                'type': 'NUMERIC',
                'options': ('no_write_model', 'write_once',)
            },
        ]


class ACQ2106_435SM_1ST(_ACQ2106_435SM):
    sites = 1


assemble(ACQ2106_435SM_1ST)


class ACQ2106_435SM_2ST(_ACQ2106_435SM):
    sites = 2


assemble(ACQ2106_435SM_2ST)


class ACQ2106_435SM_3ST(_ACQ2106_435SM):
    sites = 3


assemble(ACQ2106_435SM_3ST)


class ACQ2106_435SM_4ST(_ACQ2106_435SM):
    sites = 4


assemble(ACQ2106_435SM_4ST)


class ACQ2106_435SM_5ST(_ACQ2106_435SM):
    sites = 5


assemble(ACQ2106_435SM_5ST)


class ACQ2106_435SM_6ST(_ACQ2106_435SM):
    sites = 6


assemble(ACQ2106_435SM_6ST)

del(assemble)
