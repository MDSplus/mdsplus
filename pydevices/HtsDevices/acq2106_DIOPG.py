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

class _ACQ2106_DIOPG(MDSplus.Device):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    DIO with 4 channels or 32

    MDSplus.Device.debug - Controlled by environment variable DEBUG_DEVICES
        MDSplus.Device.dprint(debuglevel, fmt, args)
         - print if debuglevel >= MDSplus.Device.debug

    """
    base_parts=[
        {
            'path':':COMMENT',
            'type':'text',     
            'options':('no_write_shot',)
        },
        {
            'path':':NODE',        
            'type':'text',
            'value': '192.168.0.254',   
            'options':('no_write_shot',)
        },
        {
            'path':':DIO_SITE',    
            'type':'numeric',  
            'value': int(5), 
            'options':('no_write_shot',)
        },
        {
            'path':':TRIG_TIME',   
            'type':'numeric',  
            'options':('write_shot',)
        },
        {
            'path':':RUNNING',     
            'type':'numeric',  
            'options':('no_write_model',)
        },
        {
            'path':':LOG_OUTPUT',  
            'type':'text',     
            'options':('no_write_model', 'write_once', 'write_shot',)
        },
        {
            'path':':INIT_ACTION', 
            'type':'action',   
            'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
            'options':('no_write_shot',)
        },
        {
            'path':':TRIG_ACTION', 
            'type':'action',   
            'valueExpr':"Action(Dispatch('CAMAC_SERVER','TRIG',50,None),Method(None,'TRIG',head))",
            'options':('no_write_shot',)    
        },
        {
            'path':':STOP_ACTION', 
            'type':'action',   
            'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STOP',head))",
            'options':('no_write_shot',)
        },
        {
            'path':':STL',         
            'type':'text',     
            'options':('write_shot',)
        },
        {
            'path':':GPG_TRG_DX',  
            'type':'text',     
            'value': 'dx', 
            'options':('write_shot',)
        },
    ]


    def init(self):
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


        self.dprint(2, "Building STL: start")

        # Create the STL table from a series of transition times and states given in the OUTPUT node in the tree.
        # TIGA: PG nchans = 4, or non-TIGA PG nchans = 32
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

        #Load the STL into the DIO or GPG module.
        self.load_stl_data()
        
        self.dprint(1, 'WRPG has loaded the STL')
      
    INIT=init

    def stop(self):
        slot = self.getSlot()
        slot.GPG_ENABLE = 0
        self.running.on = False
    STOP=stop

    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.factory(self.node.data())
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
    

    def load_stl_data(self):
        uut = self.getUUT()
        # Pair of (transition time, 32 bit channel states):
        stl = str(self.stl.data())
        
        is_debug = (self.debug > 0)
        
        #What follows checks if the system is a GPG module (site 0) or a PG module (site 1..6)
        if self.isPG():
            uut.load_dio482pg(self.dio_site.data(), stl, is_debug)
        else:
            uut.load_wrpg(stl, is_debug)


    def set_stl(self, nchan):

        data_by_chan = []
        all_times = []

        for i in range(nchan):
            c = self.__getattr__('OUTPUT_%3.3d' % (i + 1))

            times = c.dim_of().data()
            data = c.data()

            # Build dictionary of times -> states
            data_dict = { k: v for k, v in zip(times, data) }

            data_by_chan.append(data_dict)
            all_times.extend(times)

        all_times = sorted(list(set(all_times)))

        # initialize the state matrix
        state_matrix = numpy.zeros((len(all_times), nchan), dtype='int')
                
        for c, data in enumerate(data_by_chan):
            for t, time in enumerate(all_times):
                if time in data:
                    state_matrix[t][c] = data[time]
                else:
                    state_matrix[t][c] = state_matrix[t - 1][c]

        # Building the string of 1s and 0s for each transition time:
        binary_rows = []
        times_usecs = []
        last_row = None
        for time, row in zip(all_times, state_matrix):
            if not numpy.array_equal(row, last_row):
                rowstr = [ str(i) for i in numpy.flip(row) ]  # flipping the bits so that chan 1 is in the far right position
                binary_rows.append(''.join(rowstr))
                times_usecs.append(int(time * 1E7)) # Converting the original units of the transtion times in seconds, to 1/10th micro-seconds
                last_row = row

        # TODO: depending on the hardware there is a limit number of states allowed. The below lines limited the number of the CMOD's 1800 states table to 510:
        # binary_rows = binary_rows[0:510]
        # times_usecs = times_usecs[0:510]
        
        # Write to a list with states in HEX form.
        stl  = ''
        for time, state in zip(times_usecs, binary_rows):
            stl += '%d,%08X\n' % (time, int(state, 2))
        
        self.stl.record = stl



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
        cls.parts.append(
            {'path':outfmt % (ch,), 
             'type':'SIGNAL', 
             'options':('no_write_shot',)}
            )
    return cls

class_ch_dict = create_classes(
    _ACQ2106_DIOPG, "ACQ2106_DIOPG",
    list(_ACQ2106_DIOPG.base_parts),
    ACQ2106_CHANNEL_CHOICES
)

globals().update(class_ch_dict)

del(class_ch_dict)

# public classes created in this module
# ACQ2106_DIOPG_4CH
# ACQ2106_DIOPG_32CH