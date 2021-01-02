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

class _ACQ2106_WRPG(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    DIO with 4 channels or 32

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
        MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug

    """
    base_parts=[
        {'path':':COMMENT',     'type':'text',     'options':('no_write_shot',)},
        {'path':':NODE',        'type':'text',     'options':('no_write_shot',)},
        {'path':':DIO_SITE',    'type':'numeric',  'value': int(4), 'options':('no_write_shot',)},
        {'path':':TRIG_TIME',   'type':'numeric',  'options':('write_shot',)},
        {'path':':RUNNING',     'type':'numeric',  'options':('no_write_model',)},
        {'path':':LOG_OUTPUT',  'type':'text',     'options':('no_write_model', 'write_once', 'write_shot',)},
        {'path':':INIT_ACTION', 'type':'action',   'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",'options':('no_write_shot',)},
        {'path':':TRIG_ACTION', 'type':'action',   'valueExpr':"Action(Dispatch('CAMAC_SERVER','TRIG',50,None),Method(None,'TRIG',head))",'options':('no_write_shot',)},
        {'path':':STOP_ACTION', 'type':'action',   'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",'options':('no_write_shot',)},
        {'path':':STL_LISTS',   'type':'text',     'options':('write_shot',)},
        {'path':':GPG_TRG_DX',  'type':'text',     'value': 'dx', 'options':('write_shot',)},
    ]


    def init(self):
        uut  = self.getUUT()
        slot = self.getSlot()

        # Setting the trigger in the PG/GPG module. These settings depends very much on what is the
        # configuration of the experiment. For example, when using one WRTT timing highway, then we can use d0, which will be
        # the same used by the digitazer module. Otherwise, we can choose a different one, to be in an independent highway from
        # the digitazer, like d1.

        slot.GPG_ENABLE = 1
        slot.GPG_MODE   = 'LOOP'

        if self.isPG():
            slot.TRG        = 'enable'
            slot.TRG_DX     = str(self.gpg_trg_dx.data())
            slot.TRG_SENSE  = 'rising'
        else:
            slot.GPG_TRG        = 'enable'
            slot.GPG_TRG_DX     = str(self.gpg_trg_dx.data())
            slot.GPG_TRG_SENSE  = 'rising'

        if self.debug >= 2:
            start_time = time.time()
            self.dprint(2, "Building STL: start")

        #Create the STL table from a series of transition times and states given in OUTPUT.
        #TIGA: PG nchans = 4, or non-TIGA PG nchans = 32
        tiga    = '7B'
        nontiga = '6B'

        site = self.dio_site.data()
        if site == 0 or slot.MTYPE in nontiga:
            nchans = 32
            if self.debug >= 2:
                self.dprint(2, 'DIO site and Number of Channels: {} {}'.format(self.dio_site.data(), nchans))
        elif slot.MTYPE in tiga:
            nchans = 4            
            if self.debug >= 2:
                self.dprint(2, 'DIO site and Number of Channels: {} {}'.format(self.dio_site.data(), nchans))
            
        # Create the STL table:
        self.set_stl(nchans)

        #Load the STL into the WRPG hardware: GPG
        traces = False  # True: shows debug information during loading
        self.load_stl_data(traces)
        
        if self.debug >= 1:
            self.dprint(1,'WRPG has loaded the STL')
      
    INIT=init

    def stop(self):
        slot = self.getSlot()
        slot.GPG_ENABLE = 0
        self.running.on = False
    STOP=stop

    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106_TIGA(self.node.data())
        return uut

    def getSlot(self):
        uut = self.getUUT()
        site_number  = int(self.dio_site.data())

        # Verify site_number is a valid int between 1 and 6
        # if site_number in range(1, 7):
        #     self.slot = uut.__getattr__('s' + self.dio_site.data())

        try:
            if site_number   == 0: 
                slot = uut.s0    # Only for a GPG system.
            elif site_number == 1:
                slot = uut.s1
            elif site_number == 2:
                slot = uut.s2
            elif site_number == 3:
                slot = uut.s3
            elif site_number == 4:
                slot = uut.s4
            elif site_number == 5:
                slot = uut.s5
            elif site_number == 6:
                slot = uut.s6
        except:
            pass
        
        return slot

    def isPG(self):
        uut  = self.getUUT()
        slot = self.getSlot()

        site = self.dio_site.data()
        
        try:
            if site == 0:
                is_pg = False
            else:
                is_pg = slot.GPG_ENABLE is not None
        except:
            is_pg = False

        return is_pg


    def load_stl_data(self,traces):
        uut = self.getUUT()
        # Pair of (transition time, 32 bit channel states):
        stl_pairs = self.stl_lists.data()
        # Change from Numpy array to List with toList()
        pairs = ''.join([ str(item) for item in stl_pairs.tolist() ])     

        #What follows checks if the system is a GPG module (site 0) or a PG module (site 1..6)
        if self.isPG():
            uut.load_dio482pg(self.dio_site.data(), pairs, traces)
        else:
            uut.load_wrpg(pairs, traces)

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
        stl_tuple = zip(times_usecs, binrows)

        #Record the list of lists into a tree node:
        stl_list  = []
        
        # Write to a list with states in HEX form.
        for s in stl_tuple:
            stl_list.append('%d,%08X\n' % (s[0], int(s[1], 2)))

        # MDSplus wants a numpy array
        self.stl_lists.putData(numpy.array(stl_list))

OUTFMT3 = ':OUTPUT_%3.3d'
ACQ2106_CHANNEL_CHOICES = [4, 32]

def create_classes(base_class, root_name, parts, channel_choices):
    my_classes = {}
    for nchan in channel_choices:
        class_name = "%s_%sCH" % (root_name, nchan)
        my_parts = list(parts)
        my_classes[class_name] = assemble(type(class_name, (base_class,), {"nchan": nchan, "parts": my_parts}))
        my_classes[class_name].__module__ = base_class.__module__
    return my_classes

def assemble(cls):
    outfmt = OUTFMT3
    for ch in range(1, cls.nchan+1):
        cls.parts.append({'path':outfmt%(ch,), 'type':'NUMERIC', 'options':('no_write_shot',)})
    return cls

class_ch_dict = create_classes(
    _ACQ2106_WRPG, "ACQ2106_WRPG",
    list(_ACQ2106_WRPG.base_parts),
    ACQ2106_CHANNEL_CHOICES
)

globals().update(class_ch_dict)

del(class_ch_dict)

# public classes created in this module
# ACQ2106_WRPG_4CH
# ACQ2106_WRPG_32CH