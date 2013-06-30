from MDSplus import *
from numpy import *
from threading import *
import time
import os
import errno
from ctypes import *

class CONTIPPSETUP(Device):
    print 'PROBEPSETUP'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Probe temperature control setup"""

    parts=[ {'path':':COMMENT', 'type':'text'},
    {'path':':BOARD_ID', 'type':'numeric', 'value':0},
    {'path':':AI_CHAN_LIST', 'type':'numeric'},
    {'path':':AI_FDCH_IDX', 'type':'numeric'},
    {'path':':CLOCK_FREQ', 'type':'numeric', 'value':10},
    {'path':':AOCH_ID', 'type':'numeric', 'value':0},
    {'path':':DOCH_ID', 'type':'numeric', 'value':0},
    {'path':':TEMP_REF', 'type':'numeric', 'value':50}]

    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start',head))",
        'options':('no_write_shot',)})
        
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop',head))",
        'options':('no_write_shot',)})

    parts.append({'path':':WAIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'exit',head))",
        'options':('no_write_shot',)})
        

    isRunning = False

    class AsynchWaveGen(Thread):            

        def configure(self, device, niInterfaceLib, board_id, ai_chan_list, ai_fdch_idx,  clock_freq, aoch_id, doch_id, temp_ref  ):
            self.device = device
            self.niInterfaceLib = niInterfaceLib
            self.board_id = board_id
            self.ai_chan_list = ai_chan_list
            self.ai_fdch_idx = ai_fdch_idx
            self.clock_freq = clock_freq
            self.aoch_id = aoch_id
            self.doch_id = doch_id
            self.temp_ref = temp_ref

        #  set tree ipp_probes/shot=2
        #  do/method :ACQIPPSETUP start_wave_gen

        def run(self):
		
            ai_num_chan = len(ai_chan_list)
            ai_chan_list_c = (c_uint * len(ai_chan_list) )(*ai_chan_list)

            niInterfaceLib.temperatureProbeControl(c_uint(board_id), ai_chan_list_c, c_int(ai_num_chan), c_int(ai_fdch_idx), c_double(clock_freq), c_int(aoch_id), c_int(doch_id), c_float(temp_ref) );

            print "Thread STOP"

            return


    def restoreInfo(self):

        global niInterfaceLib

        try:
            niInterfaceLib
        except:
            niInterfaceLib = CDLL("libNiInterface.so")


    def start(self, arg):

        print 'OK Init'

        global niInterfaceLib
        global isRunning

        try:
           isRunning
        except:
           isRunning = False

        if isRunning :
            print "Probs temperature control process is running"
            return 1

        self.worker = self.AsynchWaveGen()
        self.worker.daemon = True        

        print "Is running ", isRunning

        self.restoreInfo()

    
        if niInterfaceLib == 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load libNiInterface.so')
            return 0
    
        try: 
            board_id = self.board_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
            return 0

        try: 
            ai_chans_id = self.ai_chans_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing analog input channels list' )
            return 0

        try: 
            ai_fdch_idx = self.ai_fdch_idx.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing feddbach channel reference index in the channel list' )
            return 0

        try: 
            clock_freq = self.clock_freq.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing control loop frequency value' )
            return 0

        try: 
            aoch_id = self.aoch_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing analog output channel. Refereence signal to power supply' )
            return 0

        try: 
            doch_id = self.doch_id.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing digital output channel. Digital signal to heating cable rele\' ' )
            return 0

        try: 
            temp_ref = self.temp_ref.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing digital temperature set point' )
            return 0
                 
        print "Start new thread"
        isRunning = True
                
        self.worker.configure(self, niInterfaceLib, board_id, ai_chan_list, ai_num_chan, ai_fdch_idx, ai_fdch_idx, aoch_id, doch_id, temp_ref);
        self.worker.start()

        print "End Initialization"

        return 1  


    def stop(self, arg):

        global niInterfaceLib
        global isRunning

        try:
           isRunning
        except:
           isRunning = False

        if( isRunning == False ) :
            print "Probe temperature control process is NOT running"
            return 1
        
        self.restoreInfo()
        isRunning = False
        niInterfaceLib.temperatureCtrlCommand(c_char_p("stop"))
        time.sleep(2)

        return 1        

    def exit(self, arg):

        global niInterfaceLib
        global isRunning

        self.restoreInfo()
        isRunning = False
        niInterfaceLib.temperatureCtrlCommand(c_char_p("exit"))
        time.sleep(2)
 
        return 1        

