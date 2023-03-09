#!/usr/bin/env python
# coding=utf-8
from MDSplus import mdsExceptions, Device, Data, Dimension, Range, Window, Tree
from MDSplus import Int32, Float32, Float32Array, Float64
from ctypes import POINTER,CDLL,Structure,c_int,c_double,c_uint,c_char,c_byte,c_ubyte,c_float,byref,c_char_p,c_void_p,c_short,c_ulonglong
#from ctypes import *
from threading import Thread
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
import time
import os
import errno
import traceback

class NI6368EV(Device):
    """NI PXI-6368 X-series multi functional data acquisition card"""
    parts = [
        {'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':INPUT_MODE', 'type':'text', 'value':'RSE'},
        {'path':':FREQ1_DIV', 'type':'numeric', 'value':1},
        {'path':':FREQ2_DIV', 'type':'numeric', 'value':1},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':1000},
        {'path':':SEG_LENGTH', 'type':'numeric', 'value':10000},
        {'path':':SERIAL_NUM', 'type':'numeric'},
        {'path':':CLOCK_START', 'type':'numeric', 'value':0},
        {'path':':CLOCK_FREQ', 'type':'numeric', 'value':10000},
        {'path':':CLOCK_SOURCE', 'type':'numeric'},
        {'path':':HISTORY_LEN', 'type':'numeric', 'value':0.2},
        {'path':':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
        {'path':':CLOCK_LINE', 'type': 'text', 'value': 'TRIG1'},
        {'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)},
        
    ]

    for i in range(16):
        parts.extend([
            {'path':'.CHANNEL_%d'%(i+1), 'type':'structure'},
            {'path':'.CHANNEL_%d:STATE'%(i+1), 'type':'text', 'value':'BURST(FREQ1)'},
            {'path':'.CHANNEL_%d:RANGE'%(i+1), 'type':'numeric', 'value':10},
            {'path':'.CHANNEL_%d:EVENT_NAME'%(i+1), 'type':'text', 'value':'disabled '},
            {'path':'.CHANNEL_%d:START_TIME'%(i+1), 'type':'numeric', 'value':0},
            {'path':'.CHANNEL_%d:END_TIME'%(i+1), 'type':'numeric', 'value':2},
            {'path':'.CHANNEL_%d:CALIB_PARAM'%(i+1), 'type':'numeric', 'options':('no_write_model')},
            {'path':'.CHANNEL_%d:DATA_RAW'%(i+1), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')},
            {'path':'.CHANNEL_%d:DATA'%(i+1), 'type':'signal'},
        ])
    del(i)

    parts.extend([
        {'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','READY',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)},
        {'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)},
        {'path': ':WAIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',55,None),Method(None,'wait_store',head))",
         'options': ('no_write_shot',)}
    ])

    for i in range(0, 16):
        parts.append({'path': '.CHANNEL_%d:RES_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})

    DEV_IS_OPEN = 1
    DEV_OPEN = 1


    class XSERIES_DEV_INFO(Structure):
        _fields_ = [("product_id", c_uint),
                    ("device_name", c_char * 20),
                    ("is_simultaneous", c_ubyte),
                    ("is_pcie", c_ubyte),
                    ("adc_number", c_uint),
                    ("dac_number", c_uint),
                    ("port0_length", c_uint),
                    ("max_ai_channels", c_uint),
                    ("serial_number", c_uint),
                    ("aichan_size", c_uint),
                    ("aochan_size", c_uint),
                    ("ext_cal_time", c_float),
                    ("ext_cal_temp", c_ulonglong),
                    ("self_cal_time", c_float),
                    ("self_cal_temp", c_ulonglong),
                    ("geographical_addr", c_uint)]

    XSERIES_AI_CHANNEL_TYPE_LOOPBACK        = c_int(0)
    XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL    = c_int(1)
    XSERIES_AI_CHANNEL_TYPE_NRSE            = c_int(2)
    XSERIES_AI_CHANNEL_TYPE_RSE             = c_int(3)
    XSERIES_AI_CHANNEL_TYPE_INTERNAL        = c_int(5)

    XSERIES_SAMPLE_INTERVAL_COUNTER_SI_SRC  = c_int(0)
    XSERIES_SAMPLE_INTERVAL_COUNTER_TB3     = c_int(1)

    XSERIES_AI_START_TRIGGER_SW_PULSE       = c_int(0)
    XSERIES_AI_START_TRIGGER_PFI0           = c_int(1)
    XSERIES_AI_START_TRIGGER_PFI1           = c_int(2)
    XSERIES_AI_START_TRIGGER_RTSI0          = c_int(11)

    XSERIES_AI_START_TRIGGER_LOW            = c_int(31)

    XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE = c_int(0)
    XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE  = c_int(0)
    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_TB3 = c_int(0)

     # channel lines (16)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0 = c_int(1)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI1 = c_int(2)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI2 = c_int(3)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI3 = c_int(4)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI4 = c_int(5)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI5 = c_int(6)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI6 = c_int(7)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI7 = c_int(8)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI8 = c_int(9)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI9 = c_int(10)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI10 = c_int(21)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI11 = c_int(22)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI12 = c_int(23)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI13 = c_int(24)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI14 = c_int(25)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI15 = c_int(26)
    ##
    # backplane lines (8)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI0 = c_int(11)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI1 = c_int(12)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI2 = c_int(13)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI3 = c_int(14)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI4 = c_int(15)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI5 = c_int(16)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI6 = c_int(17)  
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI7 = c_int(27)  

    XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING  = c_int(0)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_LOW             = c_int(31)

    XSERIES_AI_EXTERNAL_GATE_DISABLED               = c_int(0)

    MAX_NUM_BOARDS = 100
    
    # File Descriptors
    ai_fd = 0
    boardId = 0
    ni6368EvFds = {}
    ni6368EvWorkers = {}
    niLib = None
    niInterfaceLib = None

    # Dictionaries
    inputModeDict = {'RSE':XSERIES_AI_CHANNEL_TYPE_RSE, 'NRSE':XSERIES_AI_CHANNEL_TYPE_NRSE, 'DIFFERENTIAL':XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL, 'LOOPBACK':XSERIES_AI_CHANNEL_TYPE_LOOPBACK, 'INTERNAL':XSERIES_AI_CHANNEL_TYPE_INTERNAL}
    gainDict = {10.:c_int(0), 5.:c_int(1), 2.:c_int(2), 1.:c_int(3), 0.5:c_int(4), 0.2:c_int(5),0.1:c_int(6)}
    clockLineDict = {'PFI0': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0, 'PFI1': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI1, 'PFI2': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI2, 'PFI3': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI3, 'PFI4': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI4, 'PFI5': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0, 'PFI6': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI6, 'PFI7': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI7, 'PFI8': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI8, 'PFI9': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI9, 'PFI10': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI10, 'PFI11': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI11, 'PFI12': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI12, 'PFI13': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI13, 'PFI14': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI14, 'PFI15': XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI15,
                     'TRIG0':XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI0, 'TRIG1': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI1, 'TRIG2': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI2, 'TRIG3': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI3, 'TRIG4': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI4, 'TRIG5': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI5, 'TRIG6': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI6, 'TRIG7': XSERIES_AI_SAMPLE_CONVERT_CLOCK_RTSI7}
    diffChanMap    = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
    nonDiffChanMap = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
    ni6368chanModes = {}
    ni6368chanEvents = {}
    ni6368chanPreTimes = {}
    ni6368chanPostTimes = {}

    currShot = 0

    def debugPrint(self, msg='', obj=''):
        print (self.name + ':' + msg, obj)

    def saveInfo(self):
        self.debugPrint('SAVE INFO')
        NI6368EV.ni6368EvFds[self.getNid()] = self.ai_fd
    
    def restoreInfo(self):
        if NI6368EV.niLib is None:
            NI6368EV.niLib = CDLL('libnixseries.so')
        if NI6368EV.niInterfaceLib is None:
            NI6368EV.niInterfaceLib = CDLL('libNiInterface.so')
        try:
            self.ai_fd = NI6368EV.ni6368EvFds[self.getNid()]
            return self.DEV_IS_OPEN # if present, already opened
        except:

            try:
                NI6368EV.currShot = self.getTree().shot
                print(NI6368EV.currShot)
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing shot number '+str(ex))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                boardId = self.board_id.data()
                #The boardId number refers to device number (/dev/pxie6368.N) 
                #if it is less than MAX_NUM_BOARDS otherwise, it refers to the board 
                #serial number and the getPxie6368BoarId function translate serial 
                #number to device number of the board.
                if boardId > self.MAX_NUM_BOARDS:
                   boardId = Data.execute('getPxie6368BoarId('+str(boardId)+')')
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing board ID '+str(ex))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                fileName = '/dev/pxie-6368.' + str(boardId) + '.ai'
                self.ai_fd = os.open(fileName, os.O_RDWR) # if not present, try to open
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device' + fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return self.DEV_OPEN # return of RestoreInfo
    
    def closeInfo(self):
        try:
            self.debugPrint('CLOSE INFO')
            os.close(NI6368EV.ni6368EvFds[self.getNid()])
            del(NI6368EV.ni6368EvFds[self.getNid()])
            self.ai_fd = -1
        except:
            pass

    def saveWorker(self):
        NI6368EV.ni6368EvWorkers[self.getNid()] = self.worker
    
    def restoreWorker(self):
        try:
            if self.getNid() in NI6368EV.ni6368EvWorkers.keys():
                self.worker = NI6368EV.ni6368EvWorkers[self.getNid()]
                return True
        except Exception as ex:
            self.device.debugPrint('Error in restoring worker : %s'%(str(ex)))
            raise mdsExceptions.TclFAILED_ESSENTIAL
    
    ####### Start Inner class AsynchStore ######
    class AsynchStore(Thread):
        ACQ_NOERROR = 0
        ACQ_ERROR = 1

        # retrieving the useful information for a correct acquisition
        def configure(self, device, ai_fd, chanMap, hwChanMap, treePtr, stopAcq):
            self.device = device
            self.ai_fd = ai_fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr
            self.stopAcq = stopAcq
            self.stopReq = False
            self.error = self.ACQ_NOERROR
            self.f1Div = device.freq1_div.data()

        # worker main 
        def run(self):

            # info
            self.device.setTree(Tree(self.device.getTree().name, self.device.getTree().shot))
            self.device = self.device.copy()

            nid = self.device.getNid()
            chanModes = NI6368EV.ni6368chanModes[nid]
            chanEvents = NI6368EV.ni6368chanEvents[nid]
            chanPreTimes = NI6368EV.ni6368chanPreTimes[nid]
            chanPostTimes = NI6368EV.ni6368chanPostTimes[nid]
            chanFd = []
            chanNid = []
            coeffsNids = []
            gainDividers = []

            f1Div = self.device.freq1_div.data()
            f2Div = self.device.freq2_div.data()
            baseFreq = self.device.clock_freq.data()
            baseStart = self.device.clock_start.data()
            coeff_array = c_float*4 # c_types array
            coeff = coeff_array()   # initialization
            maxDelay = self.device.history_len.data()
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()

            # channels arrays
            numChans = len(self.chanMap)
            isBurst_c = (c_int * numChans)()
            f1Divs_c = (c_int * numChans)()
            f2Divs_c = (c_int * numChans)()
            preTimes_c = (c_double * numChans)()
            postTimes_c = (c_double * numChans)()
            eventNames_c = (c_char_p * numChans)()
            resNids = []
            
            for chan in range(numChans): #range(numChans)
                if chanModes[chan] == 'CONTINUOUS(FREQ1)': # continuous at a single frequence Freq1
                    isBurst_c[chan] = 0 # it is not a burst
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f1Div
                    eventNames_c[chan] = c_char_p(''.encode('utf-8')) # no events
                elif chanModes[chan] == 'CONTINUOUS(FREQ2)': # continuous at a single frequence Freq2
                    isBurst_c[chan] = 0
                    f1Divs_c[chan] = f2Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p(''.encode('utf-8'))
                elif chanModes[chan] == 'BURST(FREQ1)': # burst at a single frequence Freq1
                    isBurst_c[chan] = 1
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f1Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan].encode())
                    print ("CHANEVENTS: " + eventNames_c[chan].decode())
                elif chanModes[chan] == 'BURST(FREQ2)': # burst at a single frequence Freq2
                    isBurst_c[chan] = 1
                    f1Divs_c[chan] = f2Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan].encode())
                elif chanModes[chan] == 'DUAL SPEED': # switching frequence from F1 to F2 on defined period
                    isBurst_c[chan] = 0
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan].encode())
                elif chanModes[chan] == 'DISABLED':
                    continue
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Mode for channel '+str(chan + 1))
                    raise DevBAD_PARAMETER

                preTimes_c[chan] = chanPreTimes[chan]
                postTimes_c[chan] = chanPostTimes[chan]

                try:
                    boardId = self.device.board_id.data()
                    if boardId > self.device.MAX_NUM_BOARDS:
                        boardId = Data.execute('getPxie6368BoarId('+str(boardId)+')')
                    # print('OPENING', '/dev/pxie-6368.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]))
                    currFd = os.open('/dev/pxie-6368.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    # print("FD for "+ str(chan) + " is: "+ str(currFd))
                    chanFd.append(currFd)
                    # print('APERTO')
                except Exception as exc:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open Channel ' + str(self.chanMap[chan])+" : "+str(exc))
                    self.error = self.ACQ_ERROR
                    return
                
                chanNid.append( getattr(self.device, 'channel_%d_data_raw'%(self.chanMap[chan]+1)).getNid() )
                #self.device.debugPrint ('chanFd '+'channel_%d_data_raw'%(self.chanMap[chan]+1), chanFd[chan])
                resNids.append( getattr(self.device, 'channel_%d_res_raw'%(self.chanMap[chan]+1)).getNid() )
                coeffsNids.append(getattr(self.device, 'channel_%d_calib_param' % (self.chanMap[chan]+1)).getNid())

                # getting gain
                gain = getattr(self.device, 'channel_%d_range'%(self.chanMap[chan]+1)).data()
                gain_code = self.device.gainDict[gain]

                gainDividers.append(1.)

                # reading calibration params
                status = NI6368EV.niInterfaceLib.getCalibrationParams(currFd, gain_code, coeff)
                gainValue = 0
                if( status < 0 ):
                    errno = NI6368EV.niInterfaceLib.getErrno()
                    msg = 'Error (%d) %s' % (errno, os.strerror( errno ))
                    self.device.debugPrint (msg)
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536'%(self.chanMap[chan]))
                    gainValue = self.device.gainValueDict[gain] * 2.
                    coeff[0] = coeff[2] = coeff[3] = 0
                    coeff[1] = c_float( gainValue / 32768. )
                #print('SCRIVO CALIBRAZIONE', coeff)
                getattr(self.device, 'channel_%d_calib_param'%(self.chanMap[chan]+1)).putData(Float32Array(coeff))
                #print('SCRITTO')

            # handling the buffer size for resampling
            if(bufSize > segmentSize):
                segmentSize = bufSize
            else:
                c = segmentSize/bufSize
                if (segmentSize % bufSize > 0):
                    c = c+1
                segmentSize = c*bufSize
            
            # instantiating save list for mdsplus data
            saveList = c_void_p(0)
            NI6368EV.niInterfaceLib.startSave(byref(saveList)) 

            chanNid_c = (c_int * len(chanNid) )(*chanNid)
            chanFd_c = (c_int * len(chanFd) )(*chanFd)
            chanMap_c = (c_int * len(chanFd) )(*self.chanMap)
            resNids_c = (c_int * len(chanFd) )(*resNids)
            coeffsNid_c = (c_int * len(coeffsNids))(*coeffsNids)
            gainDividers_c = (c_float * len(gainDividers))(*gainDividers)
            
            print ("STARTING READ AND SAVE...")
            print ("numChans : " + str(numChans) + " chanFd: " + str(len(chanFd_c)) + " isBurst: " + str(len(isBurst_c)) + " fiDivs: " + str(len(f1Divs_c)) + " preTimes: " + str(len(preTimes_c)) + " EventNames: " + str(len(eventNames_c)) + " chanNid: " + str(len(chanNid_c)))
            while not self.stopReq:
                try:
                    status = NI6368EV.niInterfaceLib.pxi6368EV_readAndSaveAllChannels(     # main function to handle data
                    self.ai_fd, c_int(numChans), chanMap_c, chanFd_c, isBurst_c, f1Divs_c, f2Divs_c,
                    c_double(maxDelay), c_double(baseFreq),
                    preTimes_c, postTimes_c, c_double(baseStart),
                    c_int(bufSize), c_int(int(segmentSize)), eventNames_c,
                    chanNid_c, self.treePtr, saveList, self.device.clock_source.getNid(), NI6368EV.currShot, resNids_c, coeffsNid_c, gainDividers_c, self.stopAcq)

                except Exception as ex:
                    self.device.debugPrint('Acquisition thread start error : %s'%(str(ex)))
                    self.stopReq = True
                    status = -3

                if( status < 0 ):
                    if( status == -1 ):
                       Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PXI 6368 Module is not triggered')
                    if( status == -2 ):
                       Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PXI 6368 DMA overflow')
                    if( status == -3 ):
                       Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PXI 6368 Exception on acquisition function')
                    self.error = self.ACQ_ERROR
                    self.stopReq = True

            # once stopReq received, stop the acquisition
            status = NI6368EV.niLib.xseries_stop_ai(c_int(self.ai_fd))
            self.closeAll(chanFd, saveList)


        def stop(self):
            self.device.debugPrint('Stop ASYNCH WORKER TERMINATED')
            self.stopReq = True
            NI6368EV.niInterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return ( self.error != self.ACQ_NOERROR)
        
        def closeAll(self, chanFd, saveList):
            for chan in range(len(self.chanMap)):
                try:
                    os.close(chanFd[chan])
                except:
                    self.device.debugPrint('Exception')
                    pass
            self.device.debugPrint('ASYNCH WORKER TERMINATED')
            NI6368EV.niInterfaceLib.stopSave(saveList)
            NI6368EV.niInterfaceLib.freeStopAcqFlag(self.stopAcq)

            self.device.closeInfo()

        def closeTree(self):
            #On first test dosen't work   
            NI6368EV.niInterfaceLib.closeTree(self.treePtr)
            self.device.debugPrint('CLOSE TREE')

############# End Inner class AsynchStore ######## 

####################################################################################
####################################### INIT #######################################
####################################################################################

    def init(self):
        self.debugPrint('================= PXI 6368 EV Init =================')
        
        # checking if in acquisition 
        if self.restoreInfo() == self.DEV_IS_OPEN:
            try:
                if self.restoreWorker():
                    if self.worker.isAlive():
                        print ('stopping Store...')
                        self.stop_store()
                        self.restoreInfo()
            except:
                pass
        dai_fd = 0

        # defining inputMode
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Input Mode')
            raise DevBAD_PARAMETER
        
        # opening device descriptor
        fileName = '/dev/pxie-6368.'+str(self.boardId)
        dai_fd = os.open(fileName, os.O_RDWR)

        device_info = self.XSERIES_DEV_INFO(0,"".encode('utf-8'),0,0,0,0,0,0,0,0,0,0,0,0,0,0)

        # getting card info
        NI6368EV.niInterfaceLib.xseries_reset(self.ai_fd)
        status = NI6368EV.niInterfaceLib._xseries_get_device_info(c_int(dai_fd), byref(device_info))
        if status:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error reading card information')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        os.close(dai_fd)

        # saving the card serial number in MDSPLUS 
        try:
            self.serial_num.putData(device_info.serial_number)
        except:
            pass

        # saving the device configuration in aiConf
        aiConf = c_void_p(0)
        NI6368EV.niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)

        # Channel configuration
        numChannels = 16
        chanModes = []
        chanEvents = []
        chanPreTimes = []
        chanPostTimes = []

        for chan in range(1, numChannels+1):
            getattr(self, 'channel_%d_data_raw'%(chan)).deleteData()
            getattr(self, 'channel_%d_data_raw'%(chan)).setCompressOnPut(False)
            mode = getattr(self, 'channel_%d_state'%(chan)).data()
            enabled = (mode != 'DISABLED')
            if not enabled:
                continue

            chanModes.append(mode)
            # reading channels events and adding to queue    
            try:
                event = getattr(self, 'channel_%d_event_name'%(chan)).data()
                chanEvents.append(event)
            except:
                chanEvents.append('')

            # handling time period definition (preTime and postTime define the bursts beginning and ending)
            try:
                preTime = getattr(self, 'channel_%d_start_time'%(chan)).data()
                if preTime < 0:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Pre time for channel %d must be geater of equal to 0 '+str(chan))
                    raise DevBAD_PARAMETER

                postTime = getattr(self, 'channel_%d_end_time'%(chan)).data()
                if postTime < 0:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Post time for channel %d must be geater of equal to 0 '+str(chan))
                    raise DevBAD_PARAMETER
            except:
                if mode == 'BURST(FREQ1)' or mode == 'BURST(FREQ2)' or mode == ('DUAL SPEED'): # it may be a continuous acquisition
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Missing pre or post time for channel '+str(chan))
                    raise DevBAD_PARAMETER
                else:
                    preTime = 0
                    postTime = 0
            if mode != "DISABLED":
                chanPreTimes.append(preTime)
                chanPostTimes.append(postTime)
    
            try:
                gain = self.gainDict[getattr(self, 'channel_%d_range'%(chan)).data()] # retrieving the gain data for channel conf
                data = self.getTree().tdiCompile("NIanalogInputScaled( build_path($), build_path($) )", getattr(self, 'channel_%d_data_raw'%(chan)).getPath(),  getattr(self, 'channel_%d_calib_param'%(chan)).getPath() )
                data.setUnits("Volts") 
                getattr(self, 'channel_%d_data'%(chan)).putData(data)
            except:
                self.debugPrint(traceback.format_exc())
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid Configuration for channel '+str(chan))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            if(enabled):
                status = NI6368EV.niLib.xseries_add_ai_channel(aiConf, c_short(chan-1), gain, inputMode, c_byte(1))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot add channel '+str(chan))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                activeChan = chan

        # saving info for AsynchStore
        nid = self.getNid()
        NI6368EV.ni6368chanModes[nid] = chanModes
        NI6368EV.ni6368chanEvents[nid] = chanEvents
        NI6368EV.ni6368chanPreTimes[nid] = chanPreTimes
        NI6368EV.ni6368chanPostTimes[nid] = chanPostTimes
        
        # stops analog input segment, necessary since the reset is next
        status = NI6368EV.niLib.xseries_stop_ai(c_int(self.ai_fd))
        # if status:
        #     errno = NI6368EV.niInterfaceLib.getErrno()
        #     Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot stop AI: (%d) %s' % (errno, os.strerror( errno )) )
        #     raise mdsExceptions.TclFAILED_ESSENTIAL
        # reset analog input segment
        status = NI6368EV.niLib.xseries_reset_ai(c_int(self.ai_fd))
        if status:
            errno = NI6368EV.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset AI segment: (%d) %s' % (errno, os.strerror( errno )) )
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
        # check Acquisition Configuration
        try:
            bufSize = self.buf_size.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve acquisition buffer size')
            raise DevBAD_PARAMETER

        try:
            segmentSize = self.seg_length.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve acquisition segment size')
            raise DevBAD_PARAMETER

        # DMA buffer setting
        status = NI6368EV.niLib.xseries_set_ai_attribute(aiConf, c_int(0) , c_int(80))
        if(status != 0):
            errno = NI6368EV.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Set DMA buffer size : (%d) %s' % (errno, os.strerror( errno )) )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # disabling external gate 
        status = NI6368EV.niLib.xseries_set_ai_external_gate(aiConf, self.XSERIES_AI_EXTERNAL_GATE_DISABLED, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE)
        if( status != 0 ):
            errno = NI6368EV.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot disable external gate!: (%d) %s' % (errno, os.strerror( errno )) )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # setting the trigger behaviour
        status = NI6368EV.niLib.xseries_set_ai_start_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_LOW, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE, 0)
        if(status != 0):
            errno = NI6368EV.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set auto start trigger: (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
        # managing clock mode
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if(frequency > 2000000.):
                    self.debugPrint('Frequency out of limits')
                    frequency = 2000000.

                clockSource = Range(None, None, Float64(1./frequency))
                self.clock_source.putData(clockSource)
            else:
                # clock line definition (backplane or channel lines)
                clockLineKey = self.clock_line.data()
                clockLine = self.clockLineDict[clockLineKey]
                # clockSource = self.clock_source.evaluate()
                frequency = self.clock_freq.data()
                clockSource = Range(None, None, Float64(1./frequency))
                self.clock_source.putData(clockSource)
                # self.debugPrint('PXI 6368 External CLOCK: ', clockSource)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),'Invalid clock definition')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
        # clock setting
        if (clockMode == 'INTERNAL'):
            frequency = self.clock_freq.data()
            period = int(100000000/frequency)  # TB3 clock 100MHz
            self.debugPrint("PXI 6368 Internal CLOCK TB3 period ", period)
            status = NI6368EV.niLib.xseries_set_ai_scan_interval_counter(aiConf, self.XSERIES_SCAN_INTERVAL_COUNTER_TB3, self.XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE, c_int(period), c_int(2))
            if(status != 0):
                errno = NI6368EV.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Set internal sample clock: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            frequency = self.clock_freq.data()
            self.debugPrint("PXI 6368 Program the sample clock (START signal) to start on a rising edge")
            status = NI6368EV.niLib.xseries_set_ai_sample_clock(aiConf, clockLine, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE, c_int(1))
            if(status == 0):
                # Program the convert to be the same as START.
                status = NI6368EV.niLib.xseries_set_ai_convert_clock(aiConf, clockLine, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)
                self.debugPrint("xseries_set_ai_convert_clock ", clockLine)
            
            if(status != 0):
                errno = NI6368EV.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot configure external device clock: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        
        NI6368EV.niLib.xseries_stop_ai(c_int(self.ai_fd))
        
        # setting the current configuration on the module
        try:
            status = NI6368EV.niInterfaceLib.xseries_set_ai_conf_ptr(c_int(self.ai_fd), aiConf)
            if(status != 0):
                errno = NI6368EV.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot load ai configuration : (%d) %s' % (errno, os.strerror( errno )) )
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except IOError:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Exception: cannot load ai configuration: (%d) %s' % (errno, os.strerror( errno )) )
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        self.debugPrint("===============================================")


####################################################################################
################################### START STORE ####################################
####################################################################################
    def start_store(self):
        self.debugPrint('================= PXI 6368 EV ===============')

        # retrieving the opened device info
        if self.restoreInfo() != self.DEV_IS_OPEN: 
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # check module in acquisition state
        try:
            if self.restoreWorker():
                if self.worker.isAlive():
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module is in acquisition')
                    return
        except:
            pass
        
        # worker initialization
        self.worker = self.AsynchStore() 
        self.worker.daemon = True
        self.worker.stopReq = False
        NI6368EV.ni6368EvWorkers[self.nid] = self.worker

        chanMap = []
        stopAcq = c_void_p(0)
        NI6368EV.niInterfaceLib.getStopAcqFlag(byref(stopAcq)) # getting the pointer to the acquisition stop request

        for chan in range(16):
            try:
                mode = getattr(self, 'channel_%d_state'%(chan+1)).data()
                if mode != 'DISABLED':
                    chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid mode for channel '+str(chan + 1))
                raise DevBAD_PARAMETER

        print ('Tree opening')
        treePtr = c_void_p(0)
        NI6368EV.niInterfaceLib.openTree(c_char_p(self.getTree().name.encode('utf-8')), c_int(self.getTree().shot), byref(treePtr))
        print ('Tree %s opened'%self.getTree().name)

        self.worker.configure(self.copy(), self.ai_fd, chanMap, self.diffChanMap, treePtr, stopAcq)

        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        if self.worker.hasError():
            self.worker.error = self.worker.ACQ_NOERROR 
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if not self.worker.isAlive():
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition thread not started')
            raise mdsExceptions.TclFAILED_ESSENTIAL
 
        self.debugPrint("===============================================")


####################################################################################
################################### STOP STORE ####################################
####################################################################################
    def stop_store(self):

        self.debugPrint('================= PXI 6368 EV stop store ================')
        error = False

        if not self.restoreWorker():
            print("Try to restoring...")
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition thread not created')
            return

        """
        if self.worker.isAlive():
            print("Try to stopping...")
            self.debugPrint("PXI 6368 stop_worker")
            self.worker.stop()
            self.worker.join()
            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition thread stopped')
        """

        if self.worker.isAlive():
            print("Try to stopping...")
            self.worker.stop()
            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Acquisition thread stopped')

        if error:
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL
 



####################################################################################
################################### WAIT STORE ####################################
####################################################################################
    def wait_store(self):

        error = False
        self.debugPrint("=============== PXI 6368 EV wait_store ===========")

        try:
            self.restoreWorker()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not started')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        if self.worker.isAlive():
            self.worker.stop()
            self.worker.join()
            self.debugPrint("PXI 6368 EV worker thread stopped")

            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Acquisition thread stopped')

        if error:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread exit with fault')
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # self.worker.closeTree()

        self.debugPrint("===============================================")


    def readConfig(self):

        self.restoreInfo()

        try:
            NI6368EV.niInterfaceLib.readAiConfiguration(c_int(self.fd))
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read board configuration')
            raise DevBAD_PARAMETER
