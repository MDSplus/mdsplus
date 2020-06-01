#
# Copyright (c) 2020, Massachusetts Institute of Technology All rights reserved.
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
import time
import numpy
import os
from tempfile import mkstemp

class ACQ2106_WRPG(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    DIO with 32 channels

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
		MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug

    """

    parts=[
        {'path':':NODE',        'type':'text',                     'options':('no_write_shot',)},
        {'path':':COMMENT',     'type':'text',                     'options':('no_write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric',                  'options':('write_shot',)},
        {'path':':TRIG_DX',     'type':'text',    'value': 'dx',   'options':('no_write_shot',)},
        {'path':':RUNNING',     'type':'numeric',                  'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',   'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action', 'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",      'options':('no_write_shot',)},
    ]


    for j in range(32):
        parts.append({'path':':OUTPUT_%3.3d' % (j+1,), 'type':'NUMERIC', 'options':('no_write_shot',)})

    def init(self):
        import acq400_hapi
        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        
        #Number of channels of the DIO482
        nchans = 32

        # Setting the trigger in the GPG module. These settings depends very much on what is the
        # configuration of the experiment. For example, when using one WRTT timing highway, the we can use d0, which will be
        # the same used by the digitazer module. Otherwise, we can choose a different one, to be a independent highway from
        # the digitazer, like d1.
       
        uut.s0.SIG_EVENT_SRC_0 = 'GPG'
        uut.s0.GPG_ENABLE      = 'enable'
        uut.s0.GPG_TRG         = '1'    #external=1, internal=0
        uut.s0.GPG_TRG_DX      =  self.TRIG_DX   #d1 for WRTT1. d0 for WRTT0 or EXT.
        uut.s0.GPG_TRG_SENSE   = 'rising'
        uut.s0.GPG_MODE        = 'ONCE'

        if self.debug >= 2:
            start_time = time.time()
            self.dprint(2, "Building STL: start")

        #Create the STL table from a series of transition times and states given in OUTPUT.
        self.set_stl(nchans)
        
        if self.debug >= 2:
            self.dprint(2, "Building STL: end --- %s seconds ---", (time.time() - start_time))

        #Load the STL into the WRPG hardware: GPG
        traces = False  # True: shows debug information during loading
        self.load_stl_file(traces)
        self.dprint(1,'WRPG has loaded the STL')
      
    INIT=init


    def load_stl_file(self,traces):
        import acq400_hapi

        stl_table = self.stl_file
        self.dprint(1, 'Path to State Table: %s', stl_table)

        uut = acq400_hapi.Acq400(self.node.data(), monitor=False)
        uut.s0.trace = traces
        
        with open(stl_table, 'r') as fp:
            uut.load_wrpg(fp.read(), uut.s0.trace)


    def set_stl(self, nchan):

        all_t_times   = []
        all_t_times_states = []

        for i in range(nchan):
            chan_t_times = self.__getattr__('OUTPUT_%3.3d' % (i+1))

            # Pair of (transition time, state) for each channel:
            chan_t_states = chan_t_times.data()

            # Creation of an array that contains, as EVERY OTHER element, all the transition times in it, appending them
            # for each channel:
            for x in numpy.nditer(chan_t_states):
                all_t_times_states.append(x) #Appends arrays made of one element,

        # Choosing only the transition times:
        all_t_times = all_t_times_states[0::2]

        # Removing duplicates and then sorting in ascending manner:
        t_times = []
        for i in all_t_times:
            if i not in t_times:
                t_times.append(i)

        # t_times contains the unique set of transitions times used in the experiment:
        t_times = sorted(numpy.float64(t_times))

        # initialize the state matrix
        rows, cols = (len(t_times), nchan)
        state = [[0 for i in range(cols)] for j in range(rows)]

        # Building the state matrix. For each channel, we traverse all the transition times to find those who are 
        # in the particular channel. 
        # If the transition time is in the channel, we copied its state into the state[i][j] element. 
        # If a transition time does not appear in that channel, we keep the previous state for, i.e. the state doesn't change.
        for j in range(nchan):
            chan_t_states = self.__getattr__('OUTPUT_%3.3d' % (j+1))

            for i in range(len(t_times)):

                if i == 0:
                    state[i][j] = 0
                else:
                    state[i][j] = state[i-1][j]
                    
                    # chan_t_states its elements are pairs of [ttimes, state]. e.g [[0.0, 0],[1.0, 1],...]
                    # chan_t_states[0] are all the first elements of those pairs, i.e the trans. times: 
                    # e.g [[1D0], [2D0], [3D0], [4D0] ... ]
                    # chan_t_states[1] are all the second elements of those pairs, i.e. the states: 
                    # e.g [[0],[1],...]
                    for t in range(len(chan_t_states[0])):
                        #Check if the transition time is one of the times that belongs to this channel:
                        if t_times[i] == chan_t_states[0][t][0]:
                            state[i][j] = int(chan_t_states[1][t][0])


        # Building the string of 1s and 0s for each transition time:
        binrows = []
        for row in state:
            rowstr = [str(i) for i in numpy.flip(row)]  # flipping the bits so that chan 1 is in the far right position
            binrows.append(''.join(rowstr))

        # Converting the original units of the transtion times in seconds, to micro-seconts:
        times_usecs = []
        for elements in t_times:
            times_usecs.append(int(elements * 1E6)) #in micro-seconds
        # Building a pair between the t_times and bin states:
        stl_list = zip(times_usecs, binrows)

        # Creating the STL file with the path and file name given in self.stl_file.data()
        fd, temp_path = mkstemp(prefix='acq2106-dio482-', suffix='.stl')

        self.dprint(1, 'STL temp file: %s', temp_path)

        self.stl_file = temp_path

        # f=open(self.stl_file.data(), 'w')
        f = open(temp_path, 'w')

        # Write to file with states in HEX form.
        for s in stl_list:
            f.write('%d,%08X\n' % (s[0], int(s[1], 2)))

        f.close()
        os.close(fd)
