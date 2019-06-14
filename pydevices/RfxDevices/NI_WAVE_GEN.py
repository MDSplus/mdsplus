from MDSplus import mdsExceptions, Device, Data, Dimension, Range, Window
from MDSplus import Int32, Float32, Float32Array, Float64
from ctypes import *
from threading import Thread
import time
import os
import errno
import traceback
import numpy as np

class NI_WAVE_GEN(Device):
    """NI PXI-6259 / 6368 Analog Output Waveform Generation"""
    parts=[{'path':':COMMENT',      'type':'text'},
        {'path':':BOARD_ID',  'type':'numeric', 'value':0},
        {'path':':BOARD_TYPE',   'type':'text', 'value':'NI6259'},
        {'path':':WAVE_POINT',   'type':'numeric', 'value':1000},
        {'path':':TRIG_MODE',    'type':'text','value':'EXTERNAL'},
        {'path':':TRIG_SOURCE',  'type':'numeric','value':0},
        {'path':':SERIAL_NUM',   'type':'numeric'}]


    for i in range(0,4):
        parts.append({'path':'.AO_%d'%(i+1),      'type':'structure'})
        parts.append({'path':'.AO_%d:MAX'%(i+1),  'type':'numeric', 'value':1.})
        parts.append({'path':'.AO_%d:MIN'%(i+1),  'type':'numeric', 'value':0.})
        parts.append({'path':'.AO_%d:X'%(i+1),    'type':'numeric'})
        parts.append({'path':'.AO_%d:Y'%(i+1),    'type':'numeric'})
        parts.append({'path':'.AO_%d:TYPE'%(i+1), 'type':'text','value':'SIN'})
        parts.append({'path':'.AO_%d:FREQ'%(i+1), 'type':'numeric'})
    del (i)

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_gen',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_gen',head))",
        'options':('no_write_shot',)})

    AO_UPDATE_SOURCE_SELECT_UI_TC 		= c_int(0)
    AO_UPDATE_SOURCE_POLARITY_RISING_EDGE	= c_int(0)

    AO_DAC_POLARITY_UNIPOLAR			= c_int(0)
    AO_DAC_POLARITY_BIPOLAR			= c_int(1)

    #File descriptor
    #ao_fd = 0
    boardId = 0
    waveAOFds = {}
    waveAOChFds = {}
    waveWorkers = {}
    niLib6259 = None
    niLib6368 = None
    niInterfaceLib = None 

#Dictionaries and maps
    boardTypeDict = {'NI6259':'/dev/pxi6259.', 'NI6368':'/dev/pxie6368.'}
    waveTypeDict = {'AS_IS':1 , 'SIN':2 , 'COS':3 , 'TRIANGULAR':4 , 'SQUARE':5 , 'SAWTOOTH':6}

    HANDLE_FOUND = 2
    NO_ERROR = 1


    def saveInfo(self):
        NI_WAVE_GEN.waveAOFds[self.getNid()] = self.ao_fd
	#NI_WAVE_GEN.waveAOChFds.update({self.getNid():self.currFd})


    def restoreInfo(self):
        if NI_WAVE_GEN.niLib6259 is None:
	    NI_WAVE_GEN.niLib6259 = CDLL("libpxi6259.so")
        if NI_WAVE_GEN.niInterfaceLib is None:
            NI_WAVE_GEN.niInterfaceLib = CDLL("libNiInterface.so")

        """
        try:
            self.currFd = NI_WAVE_GEN.waveAOChFds[self.getNid()]
        except:
            pass
        """

        try:
            self.ao_fd = NI_WAVE_GEN.waveAOFds[self.getNid()]
            print('RESTORE INFO HANDLE FOUND')
            return NI_WAVE_GEN.HANDLE_FOUND
        except:
            print('RESTORE INFO HANDLE NOT FOUND')
            try:
                boardId = self.board_id.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board Id' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                boardType = self.board_type.data();
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing Board type' )
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                fileName = NI_WAVE_GEN.boardTypeDict[boardType] + str(boardId)+'.ao'
                self.ao_fd = os.open(fileName, os.O_RDWR)
                print('Open fileName : ', fileName)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), '--Cannot open device '+ fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return NI_WAVE_GEN.NO_ERROR

    def closeInfo(self):
        try:
            os.close(NI_WAVE_GEN.waveAOFds[self.getNid()])
            del(NI_WAVE_GEN.waveAOFds[self.getNid()])
            self.ao_fd = -1
        except:
            pass
            #print('CLOSE INFO: HANDLE NOT FOUND')
        return 1 

######################## Worker Management
    def saveWorker(self):
      NI_WAVE_GEN.waveWorkers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        if self.getNid() in NI_WAVE_GEN.waveWorkers.keys():
           self.worker = NI_WAVE_GEN.waveWorkers[self.nid]
      except:
        print('Cannot restore worker!!')


    def configChannels(self, reset):

        boardId = self.board_id.data();
        boardType = self.board_type.data();
        wavePoint = self.wave_point.data();
         

        for ch in range(4) :

            # open file descriptor for each AO channel
	    aoChFd = os.open(NI_WAVE_GEN.boardTypeDict[boardType] + str(boardId) + '.ao.' + str(ch), os.O_RDWR | os.O_NONBLOCK)        
            if aoChFd < 0 :
                errno = NI_WAVE_GEN.niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to open channel: " + os.strerror(errno))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            if reset == 1 :
                chMax = getattr(self, 'ao_%d_max'%(ch+1)).data()
                chMin = getattr(self, 'ao_%d_min'%(ch+1)).data()
                chType = getattr(self, 'ao_%d_type'%(ch+1)).data()

                offset = ( chMax + chMin ) / 2
                level = ( chMax - chMin ) / 2

                print "ch type ", chType

                # create one complete sine period in volts
            
                ix = np.arange(wavePoint)

                if chType == 'SIN':
                    sigData = offset + level * np.sin(2*np.pi*ix/float(wavePoint))

                elif chType == 'SAWTOOTH':
                    sigData = (offset + 2 * level * ix/float(wavePoint) - level );

                elif chType == "COS":
                    sigData = offset + level * np.cos(2*np.pi*ix/float(wavePoint))

                elif chType == "TRIANGULAR":
                     ix = np.arange(wavePoint/2)
                     ix1 = wavePoint/2 + np.arange(wavePoint/2)

                     sigData = ( offset + 2*level * ix/float(wavePoint/2) - level );
                     sigData = np.append(sigData, ( offset - 2 * level * ix1/float(wavePoint/2) + 3 * level ) )

                elif chType == "SQUARE":
                     ix = np.zeros(wavePoint/2) + 1

                     sigData = ( offset +  level * ix );
                     sigData = np.append( sigData, ( offset  - level * ix ) );

                elif chType == "AS_IS":
                     aoX = getattr(self, 'ao_%d_x'%(ch+1)).data()
                     aoY = getattr(self, 'ao_%d_y'%(ch+1)).data()

                     xi = np.linspace(0,1,wavePoint)
                     sigData = np.interp(xi, aoX, aoY, wavePoint)
 
                scaledWriteArray = ( c_float * len(sigData) )(*sigData)  

            else:
                zero_arr = np.zeros(wavePoint)
                scaledWriteArray = ( c_float * len(zero_arr) )(*zero_arr)

            # write sine to AO channel
            retval = NI_WAVE_GEN.niLib6259.pxi6259_write_ao(aoChFd, scaledWriteArray, c_uint(wavePoint))
            if retval != wavePoint : 
                errno = NI_WAVE_GEN.niInterfaceLib.getErrno();
                Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed while writing! Written samples.: "+ os.strerror(errno))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            os.close(aoChFd)
 

    def init(self):

        print '================= NI Wavefor Generation Init ==============='

        if self.restoreInfo() == mdsExceptions.TclFAILED_ESSENTIAL :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
            raise mdsExceptions.TclFAILED_ESSENTIAL


	# Stop the segment
        """ 
	NI_WAVE_GEN.niLib6259.xseries_stop_ao(c_int(self.ao_fd))

	retval = NI_WAVE_GEN.niLib6259.xseries_reset_ao(self.ao_fd);
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetiing device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
	retval = NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(c_int(self.ao_fd))
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting device')
            raise mdsExceptions.TclFAILED_ESSENTIAL


	retval = NI_WAVE_GEN.niLib6259.pxi6259_reset_ao(c_int(self.ao_fd));
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        #  initialize AO configuration
        aoConfig = c_void_p(0)
        NI_WAVE_GEN.niInterfaceLib.pxi6259_create_ao_conf_ptr(byref(aoConfig))

        # enable waveform generation
        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_waveform_generation(aoConfig, c_int(1)) :
            Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to set AO waveform generation!");
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
        for ch in range(4) :
            # configure AO channel
            if NI_WAVE_GEN.niLib6259.pxi6259_add_ao_channel(aoConfig, c_uint(ch), self.AO_DAC_POLARITY_BIPOLAR) :
                Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to configure channel!");
                raise mdsExceptions.TclFAILED_ESSENTIAL

        wavePoint = self.wave_point.data();
 
        # set AO count
        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_count(aoConfig, c_uint(wavePoint), c_uint(1), c_ubyte(1)) :
             Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to configure AO count!");
             raise mdsExceptions.TclFAILED_ESSENTIAL

        # configure AO update clock
        try:
            waveFreq = self.wave_freq.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid waveform frequency')
            raise mdsExceptions.TclFAILED_ESSENTIAL


	sampleRate = wavePoint * waveFreq;
	periodDivisor = (20000000 / sampleRate);
        print 'periodDivisor ', periodDivisor, aoConfig

        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_update_clk(aoConfig, self.AO_UPDATE_SOURCE_SELECT_UI_TC, self.AO_UPDATE_SOURCE_POLARITY_RISING_EDGE, c_uint(periodDivisor)) != 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to configure AO update clock!")
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # load AO configuration and let it apply
        print "ao_fd " , self.ao_fd
        if NI_WAVE_GEN.niLib6259.pxi6259_load_ao_conf(c_int(self.ao_fd), aoConfig) != 0:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno();
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Failed to load configuration! (%d) %s' % (errno, os.strerror( errno )) )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.configChannels(1)

        NI_WAVE_GEN.niInterfaceLib.pxi6259_free_ao_conf_ptr(aoConfig)

        self.saveInfo()

        return NI_WAVE_GEN.NO_ERROR


##########Start generation
    def start_gen(self):
        print "================ NI Waveform Geneation Start Store ============="

        if self.restoreInfo() != NI_WAVE_GEN.HANDLE_FOUND  :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # start AO segment (signal generation)
        if NI_WAVE_GEN.niLib6259.pxi6259_start_ao(c_int(self.ao_fd)) != 0 : 
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno();
            Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to start AO!: %s" %(os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        return NI_WAVE_GEN.NO_ERROR

##########Stop generation
    def stop_gen(self):
        print "================ NI Waveform Geneation Stop Store ============="

        if self.restoreInfo() != NI_WAVE_GEN.HANDLE_FOUND :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        # stop AO segment
        if NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(c_int(self.ao_fd)) != 0 :
           errno = NI_WAVE_GEN.niInterfaceLib.getErrno();os,
           Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to stop AO!: %s" %( os.strerror(errno)) )
           raise mdsExceptions.TclFAILED_ESSENTIAL

        self.configChannels(0)

        # start AO segment (signal generation)
        if NI_WAVE_GEN.niLib6259.pxi6259_start_ao(self.ao_fd) : 
            Data.execute('DevLogErr($1,$2)', self.getNid(), "Failed to start AO!")
            raise mdsExceptions.TclFAILED_ESSENTIAL

        time.sleep(2)

        if NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(self.ao_fd) : 
            Data.execute('DevLogErr($1,$2)', self.getNid(), "Error stopping AO generation")
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.closeInfo()

        return NI_WAVE_GEN.NO_ERROR


