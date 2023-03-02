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

from MDSplus import mdsExceptions, Device, Data, Dimension, Range, Window, Tree
from MDSplus import Int32, Float32, Float32Array, Float64
from ctypes import CDLL, Structure, c_int, c_uint, c_char, c_byte, c_ubyte
from ctypes import c_float, byref, c_char_p, c_void_p, c_short, c_ulonglong
from threading import Thread
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
import time
import os
import errno
import traceback


class NI6368AI(Device):
    """NI PXI-6368 X-series multi functional data acquisition card"""

    parts = [{'path': ':BOARD_ID', 'type': 'numeric', 'value': 0},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':INPUT_MODE', 'type': 'text', 'value': 'RSE'},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 1000},
             {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 1000},
             {'path': ':SEG_LENGTH', 'type': 'numeric', 'value': 10000},
             {'path': ':CLOCK_SOURCE', 'type': 'numeric'},
             {'path': ':ACQ_MODE', 'type': 'text', 'value': 'TRANSIENT REC.'},
             {'path': ':TRIG_MODE', 'type': 'text', 'value': 'EXTERNAL'},
             {'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0},
             {'path': ':USE_TIME', 'type': 'text', 'value': 'YES'},
             {'path': ':START_TIME', 'type': 'numeric', 'value': 0},
             {'path': ':END_TIME', 'type': 'numeric', 'value': 1},
             {'path': ':START_IDX', 'type': 'numeric', 'value': 0},
             {'path': ':END_IDX', 'type': 'numeric'}]

    for i in range(0, 16):
        parts.append({'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d:STATE' % (
            i+1), 'type': 'text', 'value': 'ENABLED'})
        parts.append({'path': '.CHANNEL_%d:RANGE' % (
            i+1), 'type': 'numeric', 'value': 10.})
        parts.append({'path': '.CHANNEL_%d:DATA' % (i+1), 'type': 'signal'})
        parts.append(
            {'path': '.CHANNEL_%d:DATA_RAW' % (i+1), 'type': 'signal'})
        parts.append(
            {'path': '.CHANNEL_%d:CALIB_PARAM' % (i+1), 'type': 'numeric'})
    del (i)

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':SERIAL_NUM', 'type': 'numeric'})
    for i in range(0, 16):
        parts.append({'path': '.CHANNEL_%d:RES_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
    parts.append({'path': ':WAIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',55,None),Method(None,'wait_store',head))",
                  'options': ('no_write_shot',)})


# File descriptor
    ai_fd = 0
    boardId = 0
    ni6368AiFds = {}
    ni6368DevFds = {}
    ni6368Workers = {}
    niLib = None
    niInterfaceLib = None

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


# Driver constants

    XSERIES_AI_CHANNEL_TYPE_LOOPBACK = c_int(0)
    XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL = c_int(1)
    XSERIES_AI_CHANNEL_TYPE_NRSE = c_int(2)
    XSERIES_AI_CHANNEL_TYPE_RSE = c_int(3)
    XSERIES_AI_CHANNEL_TYPE_INTERNAL = c_int(5)

    XSERIES_SAMPLE_INTERVAL_COUNTER_SI_SRC = c_int(0)
    XSERIES_SAMPLE_INTERVAL_COUNTER_TB3 = c_int(1)

    XSERIES_AI_START_TRIGGER_SW_PULSE = c_int(0)
    XSERIES_AI_START_TRIGGER_PFI0 = c_int(1)
    XSERIES_AI_START_TRIGGER_PFI1 = c_int(2)

    XSERIES_AI_START_TRIGGER_LOW = c_int(31)

    XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE = c_int(0)
    XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE = c_int(0)
    XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_FALLING_EDGE = c_int(1)

    XSERIES_SCAN_INTERVAL_COUNTER_TB3 = c_int(0)

    XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING = c_int(0)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0 = c_int(1)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI1 = c_int(2)
    XSERIES_AI_SAMPLE_CONVERT_CLOCK_LOW = c_int(31)

    XSERIES_AI_EXTERNAL_GATE_DISABLED = c_int(0)

    DEV_IS_OPEN = 1
    DEV_OPEN = 2

#Dictionaries and maps
    inputModeDict = {'RSE': XSERIES_AI_CHANNEL_TYPE_RSE, 'NRSE': XSERIES_AI_CHANNEL_TYPE_NRSE, 'DIFFERENTIAL': XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL,
                     'LOOPBACK': XSERIES_AI_CHANNEL_TYPE_LOOPBACK, 'INTERNAL': XSERIES_AI_CHANNEL_TYPE_INTERNAL}
    enableDict = {'ENABLED': True, 'DISABLED': False}
    gainDict = {10.: c_int(0), 5.: c_int(1), 2.: c_int(2), 1.: c_int(
        3), 0.5: c_int(4), 0.2: c_int(5), 0.1: c_int(6)}
    diffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
    nonDiffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]

    def debugPrint(self, msg="", obj=""):
        print(self.name + ":" + msg, obj)

# saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        NI6368AI.ni6368AiFds[self.getNid()] = self.ai_fd

    def restoreInfo(self):
        if NI6368AI.niLib is None:
            NI6368AI.niLib = CDLL("libnixseries.so")
        if NI6368AI.niInterfaceLib is None:
            NI6368AI.niInterfaceLib = CDLL("libNiInterface.so")

        try:
            self.ai_fd = NI6368AI.ni6368AiFds[self.getNid()]
            return self.DEV_IS_OPEN
            #self.debugPrint('RESTORE INFO HANDLE FOUND')
        except:
            #self.debugPrint('RESTORE INFO HANDLE NOT FOUND')
            try:
                boardId = self.board_id.data()
            except:
                Data.execute('DevLogErr($1,$2)',
                             self.getNid(), 'Missing Board Id')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                fileName = '/dev/pxie-6368.'+str(boardId)+'.ai'
                self.ai_fd = os.open(fileName, os.O_RDWR)
                #self.debugPrint('Open fileName : ', fileName)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot open device ' + fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return self.DEV_OPEN

    def closeInfo(self):
        try:
            self.debugPrint('CLOSE INFO')
            os.close(NI6368AI.ni6368AiFds[self.getNid()])
            del(NI6368AI.ni6368AiFds[self.getNid()])
            self.ai_fd = -1
        except:
            pass
            #self.debugPrint('CLOSE INFO: HANDLE NOT FOUND')

# Worker Management
    def saveWorker(self):
        NI6368AI.ni6368Workers[self.getNid()] = self.worker

    def restoreWorker(self):
        try:
            if self.getNid() in NI6368AI.ni6368Workers.keys():
                self.worker = NI6368AI.ni6368Workers[self.nid]
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL

# AsynchStore class
    class AsynchStore(Thread):
        #chanFd = []
        #chanNid = []
        #saveList = c_void_p(0)
        stopReq = False
        #stopAcq = c_void_p(0)

        ACQ_NOERROR = 0
        ACQ_ERROR = 1

        def configure(self, device, ai_fd, chanMap, hwChanMap, treePtr, stopAcq):
            #self.debugPrint("=========== Async Store initialization ==========")
            self.device = device
            #self.niLib = device.niLib
            #self.niInterfaceLib = device.niInterfaceLib
            self.ai_fd = ai_fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr
            self.stopAcq = stopAcq
            self.error = self.ACQ_NOERROR
            # self.debugPrint(self.stopAcq)
            #self.debugPrint("=========== Fine ==========")

        def run(self):
            import os

            self.device = self.device.copy()

            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            #counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data()
            acqMode = self.device.acq_mode.data()
            sampleToSkip = self.device.start_idx.data()
            transientRec = False
            startTime = float(self.device.start_time.data())
            trigSource = self.device.trig_source.data()
            clockSource = self.device.clock_source.evaluate()

            frequency = float(clockSource.getDelta())

            try:
                # In multi trigger acquisition acquired data from trigger_time[i]+start_time up to trigger_time[i] + end_time
                # Time value to be associted at the first sample of the i-esima acquisition must be trigger_time[i]+start_time
                numTrigger = len(trigSource)
                #trigTime = 0.
                timesIdx0 = trigSource
            except:
                # Single trigger acquire data from start time up to end time
                # Time value to be associted at the first sample must be start time
                numTrigger = 1
                timesIdx0 = []
                timesIdx0.append(0)

            if(acqMode == 'TRANSIENT REC.'):
                transientRec = True

            chanFd = []
            chanNid = []
            resNid = []
            coeffsNid = []
            gainDividers = []
            saveList = c_void_p(0)

            # NI6368AI.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));

            coeff_array = c_float*4
            coeff = coeff_array()

            for chan in range(len(self.chanMap)):
                try:
                    # self.device.debugPrint('CHANNEL', self.chanMap[chan]+1

                    currFd = os.open('/dev/pxie-6368.'+str(boardId)+'.ai.'+str(
                        self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)
                    chanNid.append(getattr(self.device, 'channel_%d_data_raw' % (
                        self.chanMap[chan]+1)).getNid())
                    # self.device.debugPrint("chanFd "+'channel_%d_data_raw'%(self.chanMap[chan]+1), chanFd[chan], " chanNid ", chanNid[chan]
                    resNid.append(getattr(self.device, 'channel_%d_res_raw' % (
                        self.chanMap[chan]+1)).getNid())
                    coeffsNid.append(getattr(self.device, 'channel_%d_calib_param' % (
                        self.chanMap[chan]+1)).getNid())
                    gain = getattr(self.device, 'channel_%d_range' %
                                   (self.chanMap[chan]+1)).data()
                    gain_code = self.device.gainDict[gain]
                    # Gains nou used in calibration for 6368
                    gainDividers.append(1.)
                    status = NI6368AI.niInterfaceLib.getCalibrationParams(
                        currFd, gain_code, coeff)
                    if(status < 0):
                        Data.execute('DevLogErr($1,$2)', self.device.getNid(
                        ), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/32768' % (str(self.chanMap[chan])))
                        coeff[0] = coeff[2] = coeff[3] = 0
                        coeff[1] = c_float(gain / 32768.)

                    getattr(self.device, 'channel_%d_calib_param' %
                            (self.chanMap[chan]+1)).putData(Float32Array(coeff))

                except:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'Cannot open Channel ' + str(self.chanMap[chan] + 1))
                    self.closeAll(chanFd, saveList)
                    self.error = self.ACQ_ERROR
                    return

            # Make sure hbuf size is a multiple of 100 not to break putSegmentResampled
            if bufSize % 100 != 0:
                bufSize = 100 * (bufSize/100 + 1)

            if(bufSize > segmentSize):
                segmentSize = bufSize
            else:
                c = segmentSize/bufSize
                if (segmentSize % bufSize > 0):
                    c = c + 1
                segmentSize = c * bufSize

            if(not transientRec):
                numSamples = -1
                self.device.debugPrint("PXI 6368 CONTINUOUS ", numSamples)

            else:
                NI6368AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)
                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data()
                except:
                    numSamples = 0

            #self.device.debugPrint("--+++ NUM SAMPLES ", numSamples)
            #self.device.debugPrint("--+++ Segment Size ", segmentSize)
            #self.device.debugPrint("--+++ sample to skip ", sampleToSkip)

            NI6368AI.niInterfaceLib.startSave(byref(saveList))

            chanNid_c = (c_int * len(chanNid))(*chanNid)
            chanFd_c = (c_int * len(chanFd))(*chanFd)
            resNid_c = (c_int * len(resNid))(*resNid)
            coeffsNid_c = (c_int * len(coeffsNid))(*coeffsNid)
            gainDividers_c = (c_float * len(gainDividers))(*gainDividers)

            trigCount = 0

            time.sleep(1)
            timeAt0 = startTime

            # self.device.debugPrint(self.stopAcq)

            while not self.stopReq:
                try:
                    status = NI6368AI.niInterfaceLib.xseriesReadAndSaveAllChannels(c_int(self.ai_fd), c_int(len(self.chanMap)), chanFd_c, c_int(int(round(bufSize))), c_int(int(round(segmentSize))), c_int(sampleToSkip), c_int(numSamples), c_float(timeAt0), c_float(frequency), chanNid_c, gainDividers_c, coeffsNid_c, self.device.clock_source.getNid(), self.treePtr, saveList, self.stopAcq,
                                                                                   c_int(self.device.getTree().shot), resNid_c)
                except Exception as ex:
                    self.device.debugPrint(
                        'Acquisition thread start error : %s' % (str(ex)))
                    self.stopReq = True
                    status = -3

                # Check termination
                trigCount += 1
                #self.device.debugPrint("PXI 6368 Trigger count %d num %d num smp %d status %d " %(trigCount , numTrigger, numSamples, status) )
                if ((numSamples > 0 and trigCount == numTrigger) or (status < 0)):
                    self.stopReq = True

                if(transientRec):
                    NI6368AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)

            if(status < 0):
                if(status == -1):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PXI 6368 Module is not triggered')
                if(status == -2):
                    Data.execute('DevLogErr($1,$2)',
                                 self.device.getNid(), 'PXI 6368 DMA overflow')
                if(status == -3):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PXI 6368 Exception on acquisition function')
                self.error = self.ACQ_ERROR

            status = NI6368AI.niLib.xseries_stop_ai(c_int(self.ai_fd))
            self.closeAll(chanFd, saveList)

        def stop(self):
            self.device.debugPrint('Stop ASYNCH WORKER TERMINATED')
            self.stopReq = True
            NI6368AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)

        def closeAll(self, chanFd, saveList):
            for chan in range(len(self.chanMap)):
                try:
                    os.close(chanFd[chan])
                except:
                    self.device.debugPrint('Exception')
                    pass
            self.device.debugPrint('ASYNCH WORKER TERMINATED')
            NI6368AI.niInterfaceLib.stopSave(saveList)
            NI6368AI.niInterfaceLib.freeStopAcqFlag(self.stopAcq)

            self.device.closeInfo()

        def closeTree(self):
            # On first test dosen't work
            NI6368AI.niInterfaceLib.closeTree(self.treePtr)
            self.device.debugPrint('CLOSE TREE')

    # End Inner class AsynchStore

    def init(self):

        self.debugPrint('=================  PXI 6368 Init ===============')

        # self.restoreInfo()

        # Acquisition in progress module check
        if self.restoreInfo() == self.DEV_IS_OPEN:
            try:
                self.restoreWorker()
                print('Check Start Store')
                if self.worker.isAlive():
                    print('stop Store')
                    self.stop_store()
                self.restoreInfo()
            except:
                pass

        aiConf = c_void_p(0)

        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid Input Mode')
            raise DevBAD_PARAMETER

        dev_fd = 0

        fileName = '/dev/pxie-6368.'+str(self.boardId)
        dev_fd = os.open(fileName, os.O_RDWR)
        #self.debugPrint('Open ai_fd: ', self.ai_fd)

        device_info = self.XSERIES_DEV_INFO(
            0, "".encode('utf-8'), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

        # get card info
        status = NI6368AI.niInterfaceLib._xseries_get_device_info(
            c_int(dev_fd), byref(device_info))
        if status:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error reading card information')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        os.close(dev_fd)

        try:
            self.serial_num.putData(device_info.serial_number)
        except:
            pass

        #self.debugPrint('OK xseries_get_device_info')

        # Stop the segment TODO is this necessary since the reset is next
        NI6368AI.niLib.xseries_stop_ai(c_int(self.ai_fd))

        # reset AI segment
        status = NI6368AI.niLib.xseries_reset_ai(c_int(self.ai_fd))
        if (status):
            errno = NI6368AI.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot reset AI segment: (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Check Acquisition Configuration
        try:
            bufSize = self.buf_size.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve acquisition buffer size')
            raise DevBAD_PARAMETER

        try:
            segmentSize = self.seg_length.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve acquisition segment size')
            raise DevBAD_PARAMETER

        # Acquisition management
        try:
            acqMode = self.acq_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve acquisition mode management')
            raise DevBAD_PARAMETER

        # trigger mode
        try:
            trigMode = self.trig_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid triger mode definition')
            raise DevBAD_PARAMETER

        # trigger source
        try:
            trigSource = (self.trig_source.data())
        except:
            if(trigMode == 'EXTERNAL'):
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve Trigger source')
                raise DevBAD_PARAMETER
            else:
                trigSource = 0.
                self.trig_source.putData(Float32(trigSource))
        self.debugPrint('PXI 6368 Trigger source: ', trigSource)

        try:
            numTrigger = len(trigSource) - 1
            """
            Trigger time must be set to 0. in multi trigger
            acquisition for correct evaluation of the number
            of samples to acquire for each trigger
            """
            trigTime = 0.
        except:
            numTrigger = 1
            trigTime = trigSource
        #self.debugPrint('Trigger number: ', numTrigger)

        # clock mode
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
                # self.debugPrint('External')
                clockSource = self.clock_source.evaluate()
                self.debugPrint('PXI 6368 External CLOCK: ', clockSource)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid clock definition')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint('PXI 6368 CLOCK: ', clockSource)

        # Time management

        try:
            useTime = self.use_time.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve time or samples management')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if acqMode == 'TRANSIENT REC.':
            if useTime == 'YES':
                try:
                    startTime = float(self.start_time.data())
                    endTime = float(self.end_time.data())
                    self.debugPrint('PXI 6368 startTime  = ', startTime)
                    self.debugPrint('PXI 6368 endTime    = ', endTime)
                    self.debugPrint('PXI 6368 trigTime   = ', trigTime)

                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot Read Start or End time')
                    raise mdsExceptions.TclFAILED_ESSENTIAL

                # Originale
                startIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigTime), clockSource), startTime)
                # self.debugPrint("Originale startIdx ", startIdx
                endIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigTime), clockSource), endTime) + 1
                #self.debugPrint("Originale endIdx ", endIdx)

                self.debugPrint('PXI 6368 startIdx = ', Int32(int(startIdx)))
                self.start_idx.putData(Int32(int(startIdx)))

                self.debugPrint('PXI 6368 endIdx   = ', Int32(int(endIdx)))
                self.end_idx.putData(Int32(int(endIdx)))

            else:
                endIdx = self.end_idx.data()
                startIdx = self.start_idx.data()

            nSamples = endIdx - startIdx

            postTrigger = nSamples + startIdx
            preTrigger = nSamples - endIdx

            self.debugPrint('PXI 6368 nSamples     = ', Int32(int(nSamples)))
            self.debugPrint('PXI 6368 seg_length   = ', self.seg_length.data())

        else:  # Continuous Acquisition
            if useTime == 'YES':
                try:
                    startTime = float(self.start_time.data())
                    self.debugPrint('PXI 6368 startTime  = ', startTime)
                    self.debugPrint('PXI 6368 trigTime = ', trigTime)

                    startIdx = Data.execute('x_to_i($1, $2)', Dimension(
                        Window(0, None, trigTime), clockSource), startTime)

                except:
                    startIdx = 0
                self.start_idx.putData(Int32(int(startIdx)))
            else:
                startIdx = self.start_idx.data()
            nSamples = -1

        if acqMode == 'TRANSIENT REC.':
            if startIdx >= 0:
                NI6368AI.niInterfaceLib.xseries_create_ai_conf_ptr(
                    byref(aiConf), c_int(0), c_int(startIdx + nSamples), (numTrigger))
                #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)
            else:
                self.debugPrint('PXI 6368 preTrigger   = ',
                                Int32(int(preTrigger)))
                self.debugPrint('PXI 6368 postTrigger   = ',
                                Int32(int(postTrigger)))
                if trigTime > startTime or trigMode == 'INTERNAL':
                    self.debugPrint(
                        'PXI 6368 Acquire only post trigger when triger time > start Time or trigger mode internal')
                    nSamples = postTrigger
                    startIdx = 0
                    self.start_idx.putData(Int32(int(0)))
                    self.start_time.putData(Float32(trigTime))
                    NI6368AI.niInterfaceLib.xseries_create_ai_conf_ptr(
                        byref(aiConf), c_int(-startIdx), c_int(nSamples), (numTrigger))
                    #niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(0), 0)
        else:
            NI6368AI.niInterfaceLib.xseries_create_ai_conf_ptr(
                byref(aiConf), c_int(0), c_int(0), 0)

        #XSERIES_AI_DMA_BUFFER_SIZE = 0
        status = NI6368AI.niLib.xseries_set_ai_attribute(
            aiConf, c_int(0), c_int(80))
        if(status != 0):
            errno = NI6368AI.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Set DMA buffer size : (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # disable external gate
        status = NI6368AI.niLib.xseries_set_ai_external_gate(
            aiConf, self.XSERIES_AI_EXTERNAL_GATE_DISABLED, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE)
        if(status != 0):
            errno = NI6368AI.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Cannot disable external gate!: (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # SET trigger
        if (trigMode == 'EXTERNAL'):
            # if(acqMode == 'TRANSIENT REC.'):
            self.debugPrint(
                "PXI 6368 select start trigger External (START1 signal)")
            status = NI6368AI.niLib.xseries_set_ai_start_trigger(
                aiConf, self.XSERIES_AI_START_TRIGGER_PFI1, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE, 1)
            # test
            #status = niLib.xseries_set_ai_reference_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_PFI1, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE ,1)

            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot set external trigger: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        else:
            self.debugPrint(
                "PXI 6368 select start trigger Internal (START1 signal)")
            #status = niLib.xseries_set_ai_start_trigger(aiConf, self.XSERIES_AI_START_TRIGGER_SW_PULSE, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE, 0)
            status = NI6368AI.niLib.xseries_set_ai_start_trigger(
                aiConf, self.XSERIES_AI_START_TRIGGER_LOW, self.XSERIES_AI_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE, 0)
            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot set auto start trigger: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        # SET clock
        if(clockMode == 'INTERNAL'):
            period = int(100000000/frequency)  # TB3 clock 100MHz
            self.debugPrint("PXI 6368 Internal CLOCK TB3 period ", period)

            status = NI6368AI.niLib.xseries_set_ai_scan_interval_counter(
                aiConf, self.XSERIES_SCAN_INTERVAL_COUNTER_TB3, self.XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE, c_int(period), c_int(2))
            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot Set internal sample clock: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        else:
            self.debugPrint(
                "PXI 6368 Program the sample clock (START signal) to start on a rising edge")
            status = NI6368AI.niLib.xseries_set_ai_sample_clock(
                aiConf, self.XSERIES_AI_SAMPLE_CONVERT_CLOCK_PFI0, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE, c_int(1))
            if(status == 0):
                # Program the convert to be the same as START.
                status = NI6368AI.niLib.xseries_set_ai_convert_clock(
                    aiConf, self.XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING, self.XSERIES_AI_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)
                self.debugPrint("xseries_set_ai_convert_clock ",
                                self.XSERIES_AI_SAMPLE_CONVERT_CLOCK_INTERNALTIMING)
            if(status == 0):
                # Program the sample and convert clock timing specifications
                status = NI6368AI.niLib.xseries_set_ai_scan_interval_counter(
                    aiConf, self.XSERIES_SCAN_INTERVAL_COUNTER_TB3, self.XSERIES_SCAN_INTERVAL_COUNTER_POLARITY_RISING_EDGE, c_int(100),  c_int(2))
                self.debugPrint("xseries_set_ai_scan_interval_counter ",
                                self.XSERIES_SCAN_INTERVAL_COUNTER_TB3)
            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot configure external device clock: (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        # Channel configuration
        numChannels = 16
        activeChan = 0
        for chan in range(1, numChannels+1):
            try:
                # Empy the node which will contain  the segmented data
                getattr(self, 'channel_%d_data_raw' % (chan)).deleteData()

                getattr(self, 'channel_%d_data_raw' %
                        (chan)).setCompressOnPut(False)
                enabled = self.enableDict[getattr(
                    self, 'channel_%d_state' % (chan)).data()]
                gain = self.gainDict[getattr(
                    self, 'channel_%d_range' % (chan)).data()]
                data = self.getTree().tdiCompile("NIanalogInputScaled( build_path($), build_path($) )", getattr(
                    self, 'channel_%d_data_raw' % (chan)).getPath(),  getattr(self, 'channel_%d_calib_param' % (chan)).getPath())
                data.setUnits("Volts")
                getattr(self, 'channel_%d_data' % (chan)).putData(data)
            except:
                # self.debugPrint(sys.exc_info()[0])
                self.debugPrint(traceback.format_exc())
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            if(enabled):
                # self.debugPrint(' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = NI6368AI.niLib.xseries_add_ai_channel(
                    aiConf, c_short(chan-1), gain, inputMode, c_byte(1))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot add channel '+str(chan))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                #self.debugPrint ('PXI 6368 CHAN '+ str(chan) + ' CONFIGURED')
                activeChan = chan
            # else:
                #self.debugPrint ('PXI 6368 CHAN '+ str(chan) + ' DISABLED' )

        # endfor

        NI6368AI.niLib.xseries_stop_ai(c_int(self.ai_fd))

        try:
            status = NI6368AI.niInterfaceLib.xseries_set_ai_conf_ptr(
                c_int(self.ai_fd), aiConf)
            #status = NI6368AI.niLib.xseries_load_ai_conf( c_int(self.ai_fd), aiConf)
            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot load ai configuration : (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except IOError:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Exception: cannot load ai configuration: (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        self.debugPrint("===============================================")

    def start_store(self):

        self.debugPrint("================ PXI 6368 Start Store =============")

        if self.restoreInfo() != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Acquisition in progress module check
        try:
            self.restoreWorker()
            print ('Check worker is running')
            if self.worker.isAlive():
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Module is acquiring')
                return
        except:
            pass

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        chanMap = []

        numChannels = 16
        for chan in range(0, numChannels):
            try:
                enabled = self.enableDict[getattr(
                    self, 'channel_%d_state' % (chan+1)).data()]
                if enabled:
                    chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan + 1))
                raise mdsExceptions.TclFAILED_ESSENTIAL

        print ('Tree opening')
        treePtr = c_void_p(0)
        NI6368AI.niInterfaceLib.openTree(c_char_p(self.getTree().name.encode('utf-8')), c_int(self.getTree().shot), byref(treePtr))
        print ('Tree opened')

        stopAcq = c_void_p(0)
        NI6368AI.niInterfaceLib.getStopAcqFlag(byref(stopAcq))

        self.worker.configure(self, self.ai_fd, chanMap, self.diffChanMap, treePtr, stopAcq)

        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        if self.worker.hasError():
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if not self.worker.isAlive():
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not started')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint("===============================================")

    def stop_store(self):

        error = False
        self.debugPrint("=============== PXI 6368 stop_store ===========")

        try:
            self.restoreWorker()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not started')
            return

        if self.worker.isAlive():
            #          self.debugPrint ("PXI 6368 stop_worker")
            #          self.debugPrint("PXI 6368 join to worker thread")
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

        self.debugPrint("===============================================")

    def wait_store(self):

        error = False
        self.debugPrint("=============== PXI 6368 wait_store ===========")

        try:
            self.restoreWorker()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not started')
            return

        if self.worker.isAlive():
            self.worker.stop()
            self.worker.join()
            self.debugPrint("PXI 6368 worker thread stopped")

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

    def trigger(self):
        self.debugPrint("=============== PXI 6368 trigger  ===========")

        if self.restoreInfo() != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            status = NI6368AI.niLib.xseries_start_ai(c_int(self.ai_fd))
            if(status != 0):
                errno = NI6368AI.niInterfaceLib.getErrno()
                Data.execute('DevLogErr($1,$2)', self.device.getNid(
                ), 'Cannot Start Acquisition : (%d) %s' % (errno, os.strerror(errno)))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Exception Cannot Start Acquisition')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint("===============================================")
