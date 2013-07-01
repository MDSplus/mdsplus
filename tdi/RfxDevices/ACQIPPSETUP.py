from MDSplus import *
from numpy import *
from threading import *
import time
import os
import errno
from ctypes import *

class ACQIPPSETUP(Device):
    print 'PROBEPSETUP'
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """IPP probe & thermocoupels acquisition setup"""

    parts=[ {'path':':COMMENT', 'type':'text'}]
    parts.append({'path':'.PROBE', 'type':'structure'})
    parts.append({'path':'.PROBE:START_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.PROBE:END_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.PROBE:CLOCK_FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.PROBE:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':'.SWEEP_WAVE', 'type':'structure'})
    parts.append({'path':'.SWEEP_WAVE:DEV_TYPE', 'type':'text','value':'NI6368'})
    parts.append({'path':'.SWEEP_WAVE:BOARD_ID', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:AO_CHAN', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:MIN', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:MAX', 'type':'numeric', 'value':0})
    parts.append({'path':'.SWEEP_WAVE:FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.SWEEP_WAVE:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':'.TC', 'type':'structure'})
    parts.append({'path':'.TC:START_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.TC:END_TIME', 'type':'numeric', 'value':0})
    parts.append({'path':'.TC:CLOCK_FREQ', 'type':'numeric', 'value':1000})
    parts.append({'path':'.TC:TRIG_MODE', 'type':'text','value':'EXTERNAL'})

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_wave_gen',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_wave_gen',head))",
        'options':('no_write_shot',)})

    isRunning = False

    class AsynchWaveGen(Thread):            

        def configure(self, device, niInterfaceLib):
            self.device = device
            self.niInterfaceLib = niInterfaceLib

        #      set tree ipp_probes/shot=2
        #  do/method :ACQIPPSETUP start_wave_gen

        def run(self):

            if niInterfaceLib == 0 :
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load libNiInterface.so')
                return 0
        
            try: 
                board_id = self.device.sweep_wave_board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                return 0

            try: 
                channel = self.device.sweep_wave_ao_chan.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing output channel number 0..3' )
                return 0

            try: 
                minValue = self.device.sweep_wave_min.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing min sweep value' )
                return 0

            try: 
                maxValue = self.device.sweep_wave_max.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing max sweep value' )
                return 0

            try: 
                waverate = self.device.sweep_wave_freq.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing frequency sweep value' )
                return 0

            try: 
                trigMode = self.device.sweep_wave_trig_mode.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing trig mode sweep value' )
                return 0

            level  = ( maxValue - minValue ) /2.;
            offset = ( maxValue + minValue ) / 2.;
    
            print "Offset  ", offset, " Vpp level ", level * 2

            offset = offset / 10.;
            level = level / 10.;

            print "Reference Offset  ", offset, " Vpp level ", level * 2

            if(trigMode == 'EXTERNAL'):
                 softwareTrigger = 0;
            else:
                 softwareTrigger = 1;

            self.niInterfaceLib.generateWaveformOnOneChannel_6368(c_int(board_id), c_int(channel), c_double(offset), c_double(level), c_int(waverate), c_int(softwareTrigger) );

	    """	
	    count = 0
	    delay = 2;
            while count < 5:
                time.sleep(delay)
                count += 1
                print " %s" % (  time.ctime(time.time()) )
 	    """

            #print "Thread STOP"

            return


    def restoreInfo(self):

        global niInterfaceLib

        try:
            niInterfaceLib
        except:
            niInterfaceLib = CDLL("libNiInterface.so")


    def start_wave_gen(self, arg):

        print '======= Initialize waveform generation ========'

        global niInterfaceLib
 	global isRunning

        try:
           isRunning
        except:
           isRunning = False

	if isRunning :
	    print "Is running stop thread"
	    niInterfaceLib.stopWaveGeneration()
            time.sleep(2);

        self.worker = self.AsynchWaveGen()
        self.worker.daemon = True        


	#print "Is running ", isRunning



        self.restoreInfo()

        #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(nSamples), (numTrigger))
        #niInterfaceLib.generateWaveformOnOneChannel_6368(uint8_t selectedCard, uint8_t channel, double offset, double level, uint32_t waverate, char softwareTrigger);
        #niInterfaceLib.generateWaveformOnOneChannel_6368(c_int(board_id), c_int(channel), c_double(offset), c_double(level), c_int(waverate), c_int(softwareTrigger) );
     
	#print "Start new thread"
	isRunning = True
	self.worker.configure(self, niInterfaceLib);
	self.worker.start()

        #print "End Initialization"
        print "==============================================="

        return 1  


    def stop_wave_gen(self, arg):

        print '========= Stop waveform generation ============'
        global niInterfaceLib
 	global isRunning

        self.restoreInfo()
        isRunning = False
        niInterfaceLib.stopWaveGeneration()
        time.sleep(2)
        print "==============================================="

        return 1        


