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
import MDSplus
import threading
from queue import Queue, Empty
import time
import socket
import math
import numpy as np
import csv
import copy

try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

class ACQ2106_WRPG(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    32 Channels * number of slots
    Minimum 2Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    Software sample decimation

    Settable segment length in number of samples

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    parts=[
        {'path':':NODE',        'type':'text',                     'options':('no_write_shot',)},
        {'path':':COMMENT',     'type':'text',                     'options':('no_write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric',                  'options':('write_shot',)},
        {'path':':RUNNING',     'type':'numeric',                  'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',   'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",      'options':('no_write_shot',)},
        {'path':':STL_FILE',    'type':'TEXT'},
    ]

    for j in range(32):
        parts.append({'path':':OUTPUT_%3.3d' % (j+1,), 'type':'NUMERIC', 'options':('no_write_shot',)})

    def init(self):
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)

        # #Setting the trigger in the GPG module
        uut.s0.GPG_ENABLE    ='enable'
        uut.s0.GPG_TRG       ='1'    #external=1, internal=0
        uut.s0.GPG_TRG_DX    ='d0'
        uut.s0.GPG_TRG_SENSE ='rising'
        uut.s0.GPG_MODE      ='ONCE'

        #Create the STL table from a series of transition times and states given in OUTPUT.
        print("Building STL: start")
        self.set_stl()
        print("Building STL: end")

        #Load the STL into the WRPG hardware: GPG
        traces = False  # True: shows debugging information during loading
        self.load_stl_file(traces)
        print('WRPG has loaded the STL')
      
    INIT=init


    def load_stl_file(self,traces):
        example_stl=self.stl_file.data()    

        print('Path to State Table: ', example_stl)
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.trace = traces
        
        print('Loading STL table into WRPG')
        with open(example_stl, 'r') as fp:
            uut.load_wrpg(fp.read(), uut.s0.trace)

    def set_stl(self):
        nchan = 32

        states_hex    = []
        states_bits   = []
        all_t_times   = []
        all_t_times_states = []

        for i in range(nchan):
            chan_t_times = self.__getattr__('OUTPUT_%3.3d' % (i+1))

            # Pair of (transition time, state) for each channel:
            chan_t_states = chan_t_times.data()

            # Creation of an array that contains as EVERY OTHER element all the transition times in it, adding them
            # for each channel:
            for x in np.nditer(chan_t_states):
                all_t_times_states.append(x) #Appends arrays made of one element,

        # Choosing only the transition times:
        all_t_times = all_t_times_states[0::2]

        # Removing duplicates and then sorting in ascending manner:
        t_times = []
        for i in all_t_times:
            if i not in t_times:
                t_times.append(i)

        # t_times contains the unique set of transitions times used in the experiment:
        t_times = sorted(np.float64(t_times))
        print(t_times)

        # initialize the state matrix
        cols = nchan
        state = [[0]*cols]

        # Building the state matrix. For each transition times given by t_times, we look for those times that
        # appear in the channel. If a transition time does not appear in that channel, then the state
        # doesn't change.

        for i in range(len(t_times)):
            for j in range(nchan):
                chan_t_states = self.__getattr__('OUTPUT_%3.3d' % (j+1))
                
                # chan_t_states its elements are pairs of [ttimes, state]. e.g [[0.0, 0],[1.0, 1],...]
                # chan_t_states[0] are all the first elements of those pairs, i.e the trans. times: e.g [[1D0], [2D0], [3D0], [4D0] ... ]
                # chan_t_states[1] are all the second elements of those pairs, the states: .e.g [[0],[1],...]
                for s in range(len(chan_t_states[0])):
                    
                    #Check if the transition time is one of the times that belongs to this channel:
                    if t_times[i] == chan_t_states[0][s][0]:
                        print("inside ", int(chan_t_states[1][s][0]))
                        state[i][j] = int(chan_t_states[1][s][0]) 

            # Building the string of 1s and 0s for each transition time:
            binstr = ''
            for element in np.flip(state[i]):
                binstr += str(element)
            states_bits.append(binstr)
            
            # If the transition time is not in that channel, then keep a copy of the state of the previous channel
            state.append(copy.deepcopy(state[i]))

        # Converting the original units of the transtion times in seconds, to micro-seconts:
        times_usecs = []
        for elements in t_times:
            times_usecs.append(int(elements * 1E6)) #in micro-seconds
        # Building a pair between the t_times and hex states:
        state_list = zip(times_usecs, states_bits)

        f=open(self.stl_file.data(), 'w')

        for s in state_list:
            f.write('%d,%08X\n' % (s[0], int(s[1], 2)))

        f.close()
