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

from MDSplus import mdsExceptions, Device, Data, Float32, Tree
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p, c_double
import os
import time


class NI6259EV(Device):
    """NI PXI-6259 M-series multi functional data acquisition card"""
    parts = [
        {'path': ':BOARD_ID', 'type': 'numeric', 'value': 0},
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':INPUT_MODE', 'type': 'text', 'value': 'RSE'},
        {'path': ':FREQ1_DIV', 'type': 'numeric', 'value': 1},
        {'path': ':FREQ2_DIV', 'type': 'numeric', 'value': 1},
        {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 1000},
        {'path': ':SEG_LENGTH', 'type': 'numeric', 'value': 10000},
        {'path': ':CLOCK_START', 'type': 'numeric'},
        {'path': ':CLOCK_FREQ', 'type': 'numeric'},
        {'path': ':SERIAL_NUM', 'type': 'numeric'},
        {'path': ':CONVERT_CLK', 'type': 'numeric', 'value': 20},
        {'path': ':HISTORY_LEN', 'type': 'numeric', 'value': 0.2},
    ]

    for i in range(32):
        parts.extend([
            {'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'},
            {'path': '.CHANNEL_%d:MODE' % (
                i+1), 'type': 'text', 'value': 'DISABLED'},
            {'path': '.CHANNEL_%d:POLARITY' % (
                i+1), 'type': 'text', 'value': 'BIPOLAR'},
            {'path': '.CHANNEL_%d:RANGE' % (
                i+1), 'type': 'text', 'value': '10V'},
            {'path': '.CHANNEL_%d:EVENT_NAME' % (i+1), 'type': 'text'},
            {'path': '.CHANNEL_%d:START_TIME' % (
                i+1), 'type': 'numeric', 'value': 0},
            {'path': '.CHANNEL_%d:END_TIME' % (
                i+1), 'type': 'numeric', 'value': 1E-2},
            {'path': '.CHANNEL_%d:CALIB' % (
                i+1), 'type': 'numeric', 'options': ('no_write_model')},
            {'path': '.CHANNEL_%d:RAW' % (
                i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')},
            {'path': '.CHANNEL_%d:DATA' % (
                i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')},
        ])
    del(i)
    parts.extend([
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
         'options': ('no_write_shot',)},
        {'path': ':START_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
         'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
         'options': ('no_write_shot',)},
    ])


# File descriptor
    fd = 0

# Driver constants
    AI_CHANNEL_TYPE_CALIBRATION = c_int(0)
    AI_CHANNEL_TYPE_DIFFERENTIAL = c_int(1)
    AI_CHANNEL_TYPE_NRSE = c_int(2)
    AI_CHANNEL_TYPE_RSE = c_int(3)
    AI_POLARITY_BIPOLAR = c_int(0)
    AI_POLARITY_UNIPOLAR = c_int(1)
    AI_SAMPLE_SELECT_PFI0 = c_int(1)
    AI_SAMPLE_SELECT_SI_TC = c_int(0)
    AI_SAMPLE_POLARITY_RISING_EDGE = c_int(0)
    AI_CONVERT_SELECT_SI2TC = c_int(0)
    AI_CONVERT_POLARITY_RISING_EDGE = c_int(1)

    AI_CONTINUOUS = c_int(3)

    AI_START_SELECT_PULSE = c_int(0)
    AI_START_SELECT_PFI1 = c_int(2)

    AI_START_SELECT_RTSI0 = c_int(11)
    AI_START_SELECT_RTSI1 = c_int(12)

    AI_START_SELECT = c_int(9)
    AI_START_POLARITY = c_int(10)
    AI_START_POLARITY_RISING_EDGE = c_int(0)

    AI_REFERENCE_SELECT_PULSE = c_int(0)
    AI_REFERENCE_SELECT_PFI1 = c_int(2)
    AI_REFERENCE_SELECT = c_int(11)
    AI_REFERENCE_POLARITY = c_int(12)
    AI_REFERENCE_POLARITY_RISING_EDGE = c_int(0)

    PXI6259_AI_START_TRIGGER = c_int(3)
    PXI6259_RTSI1 = c_int(3)
    PXI6259_RTSI2 = c_int(4)

    DEV_IS_OPEN = 1
    DEV_OPEN = 2


#Dictionaries and maps
    inputModeDict = {'RSE': AI_CHANNEL_TYPE_RSE, 'NRSE': AI_CHANNEL_TYPE_NRSE,
                     'DIFFERENTIAL': AI_CHANNEL_TYPE_DIFFERENTIAL}
    enableDict = {'ENABLED': True, 'DISABLED': False}
    gainDict = {'10V': c_byte(1), '5V': c_byte(2), '2V': c_byte(3), '1V': c_byte(
        4), '500mV': c_byte(5), '200mV': c_byte(6), '100mV': c_byte(7)}
    gainValueDict = {'10V': 10., '5V': 5., '2V': 2.,
                     '1V': 1., '500mV': 0.5, '200mV': 0.2, '100mV': 0.1}
    polarityDict = {'UNIPOLAR': AI_POLARITY_UNIPOLAR,
                    'BIPOLAR': AI_POLARITY_BIPOLAR}
    diffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23]
    nonDiffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32]
    niLib = None
    niInterfaceLib = None
    ni6259Fds = {}
    workers = {}
    ni6259chanModes = {}
    ni6259chanEvents = {}
    ni6259chanPreTimes = {}
    ni6259chanPostTimes = {}

    def debugPrint(self, msg="", obj=""):
        print(self.name + ":" + msg, obj)


# saveInfo and restoreInfo allow to handle open file descriptors


    def saveInfo(self):
        print('SAVE INFO')
        NI6259EV.ni6259Fds[self.nid] = self.fd

    def restoreInfo(self):
        if NI6259EV.niLib is None:
            NI6259EV.niLib = CDLL("libpxi6259.so")
        if NI6259EV.niInterfaceLib is None:
            NI6259EV.niInterfaceLib = CDLL("libNiInterface.so")

        if self.nid in NI6259EV.ni6259Fds.keys():
            self.fd = NI6259EV.ni6259Fds[self.nid]
            return self.DEV_IS_OPEN
        else:
            try:
                boardId = self.board_id.data()
            except:
                Data.execute('DevLogErr($1,$2)',
                             self.getNid(), 'Missing Board Id')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                fileName = '/dev/pxi6259.'+str(boardId)+'.ai'
                self.fd = os.open(fileName, os.O_RDWR)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot open device ' + fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        try:
            if( niLib.pxi6259_reset_ai(self.fd) ):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        return self.DEV_OPEN

    def closeInfo(self):
        if self.nid in NI6259EV.ni6259Fds.keys():
            self.fd = NI6259EV.ni6259Fds[self.nid]
            del(NI6259EV.ni6259Fds[self.nid])
            try:
                if NI6259EV.niLib.pxi6259_reset_ai(self.fd):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot reset device ' + self.fd)
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot reset device ' + self.fd)
                raise mdsExceptions.TclFAILED_ESSENTIAL
            os.close(self.fd)
        return

# Worker Management
    def saveWorker(self):
        NI6259EV.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in NI6259EV.workers.keys():
            self.worker = NI6259EV.workers[self.nid]
            return True
        else:
            return False

# AsynchStore class
    class AsynchStore(Thread):

        ACQ_NOERROR = 0
        ACQ_ERROR = 1

        def configure(self, device, fd, chanMap, hwChanMap, treePtr, stopAcq):
            self.device = device
            self.fd = fd
            self.chanMap = chanMap
            self.hwChanMap = hwChanMap
            self.treePtr = treePtr
            self.stopAcq = stopAcq
            self.stopReq = False
            self.error = self.ACQ_NOERROR

        def run(self):

            self.device.setTree(
                Tree(self.device.getTree().name, self.device.getTree().shot))
            self.device = self.device.copy()

            nid = self.device.getNid()
            chanModes = NI6259EV.ni6259chanModes[nid]
            chanEvents = NI6259EV.ni6259chanEvents[nid]
            chanPreTimes = NI6259EV.ni6259chanPreTimes[nid]
            chanPostTimes = NI6259EV.ni6259chanPostTimes[nid]
            chanFd = []
            chanNid = []
            f1Div = self.device.freq1_div.data()
            f2Div = self.device.freq2_div.data()
            baseFreq = self.device.clock_freq.data()
            baseStart = self.device.clock_start.data()
            coeff_array = c_float*4
            coeff = coeff_array()

            maxDelay = self.device.history_len.data()
            # NI6259AI.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));
            numChans = len(self.chanMap)
            isBurst_c = (c_int * numChans)()
            f1Divs_c = (c_int * numChans)()
            f2Divs_c = (c_int * numChans)()
            preTimes_c = (c_double * numChans)()
            postTimes_c = (c_double * numChans)()
            eventNames_c = (c_char_p * numChans)()

            for chan in range(numChans):
                # self.device.debugPrint 'CHANNEL', self.chanMap[chan]+1
                # self.device.debugPrint '/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]])
                if chanModes[chan] == 'CONTINUOUS(FREQ1)':
                    isBurst_c[chan] = 0
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f1Div
                    eventNames_c[chan] = c_char_p('')
                elif chanModes[chan] == 'CONTINUOUS(FREQ2)':
                    isBurst_c[chan] = 0
                    f1Divs_c[chan] = f2Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p('')
                elif chanModes[chan] == 'BURST(FREQ1)':
                    isBurst_c[chan] = 1
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f1Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan])
                elif chanModes[chan] == 'BURST(FREQ2)':
                    isBurst_c[chan] = 1
                    f1Divs_c[chan] = f2Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan])
                elif chanModes[chan] == 'DUAL SPEED':
                    isBurst_c[chan] = 0
                    f1Divs_c[chan] = f1Div
                    f2Divs_c[chan] = f2Div
                    eventNames_c[chan] = c_char_p(chanEvents[chan])
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Invalid Mode for channel '+str(chan + 1))
                    raise DevBAD_PARAMETER

                preTimes_c[chan] = chanPreTimes[chan]
                postTimes_c[chan] = chanPostTimes[chan]

                try:
                    boardId = getattr(self.device, 'board_id').data()
                    print('APRO', '/dev/pxi6259.'+str(boardId)+'.ai.' +
                          str(self.hwChanMap[self.chanMap[chan]]))
                    currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(
                        self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)
                    print('APERTO')
                except Exception as e:
                    self.device.debugPrint(e)
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'Cannot open Channel ' + str(self.chanMap[chan]))
                    self.error = self.ACQ_ERROR
                    return
                chanNid.append(getattr(self.device, 'channel_%d_raw' %
                                       (self.chanMap[chan]+1)).getNid())

                self.device.debugPrint(
                    'chanFd '+'channel_%d_raw' % (self.chanMap[chan]+1), chanFd[chan])

                gain = getattr(self.device, 'channel_%d_range' %
                               (self.chanMap[chan]+1)).data()
                gain_code = self.device.gainDict[gain]

                # time.sleep(0.600)
                n_coeff = c_int(0)
                status = NI6259EV.niInterfaceLib.pxi6259_getCalibrationParams(
                    currFd, gain_code, coeff, byref(n_coeff))

                if(status < 0):
                    errno = NI6259EV.niInterfaceLib.getErrno()
                    msg = 'Error (%d) %s' % (errno, os.strerror(errno))
                    self.device.debugPrint(msg)
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536' % (self.chanMap[chan]))

                gainValue = self.device.gainValueDict[gain] * 2.
                coeff[0] = coeff[2] = coeff[3] = 0
                coeff[1] = c_float(gainValue / 65536.)
                print('SCRIVO CALIBRAZIONE', coeff)
                getattr(self.device, 'channel_%d_calib' %
                        (self.chanMap[chan]+1)).putData(Float32(coeff))
                print('SCRITTO')

            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()

            if(bufSize > segmentSize):
                segmentSize = bufSize
            else:
                c = segmentSize/bufSize
                if (segmentSize % bufSize > 0):
                    c = c+1
                segmentSize = c*bufSize

            status = NI6259EV.niLib.pxi6259_start_ai(c_int(self.fd))

            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'Cannot Start Acquisition ')
                self.error = self.ACQ_ERROR
                return

            saveList = c_void_p(0)
            NI6259EV.niInterfaceLib.startSaveEV(byref(saveList))
            #count = 0

            chanNid_c = (c_int * len(chanNid))(*chanNid)
            chanFd_c = (c_int * len(chanFd))(*chanFd)

            while not self.stopReq:
                status = NI6259EV.niInterfaceLib.pxi6259EV_readAndSaveAllChannels(
                    c_int(numChans), chanFd_c, isBurst_c, f1Divs_c, f2Divs_c,
                    c_double(maxDelay), c_double(baseFreq),
                    preTimes_c, postTimes_c, c_double(baseStart),
                    c_int(bufSize), c_int(segmentSize), eventNames_c,
                    chanNid_c, self.treePtr, saveList, self.stopAcq)

                if status < 1:
                    return 0

            status = NI6259EV.niLib.pxi6259_stop_ai(c_int(self.fd))

            for chan in range(len(self.chanMap)):
                os.close(chanFd[chan])
            self.device.debugPrint('ASYNCH WORKER TERMINATED')
#            NI6259EV.niInterfaceLib.stopSaveEV(saveList)
#            NI6259EV.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            self.device.closeInfo()

            return

        def stop(self):
            self.stopReq = True
            NI6259EV.niInterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)


# End Inner class AsynchStore

##########init############################################################################


    def init(self):

        self.debugPrint(
            '================= 11 PXI 6259 EV Init ===============')

        # self.restoreInfo()

# Module in acquisition check
        if self.restoreInfo() == self.DEV_IS_OPEN:
            try:
                if self.restoreWorker():
                    if self.worker.isAlive():
                        print ('stop Store')
                        self.stop_store()
                        self.restoreInfo()
            except:
                pass

        aiConf = c_void_p(0)
        NI6259EV.niInterfaceLib.pxi6259_create_ai_conf_ptr(byref(aiConf))
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid Input Mode')
            raise DevBAD_PARAMETER
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            numChannels = 16
        else:
            numChannels = 32

# Channel configuration
        activeChan = 0
        chanModes = []
        chanEvents = []
        chanPreTimes = []
        chanPostTimes = []

        for chan in range(0, numChannels):

            # Empy the node which will contain  the segmented data
            getattr(self, 'channel_%d_raw' % (chan+1)).deleteData()

            getattr(self, 'channel_%d_raw' % (chan+1)).setCompressOnPut(False)
            mode = getattr(self, 'channel_%d_mode' % (chan+1)).data()
            chanModes.append(mode)
            enabled = (mode != 'DISABLED')
            if not enabled:
                continue
            try:
                event = getattr(self, 'channel_%d_event_name' %
                                (chan+1)).data()
                chanEvents.append(event)
            except:
                chanEvents.append('')
            try:
                preTime = getattr(self, 'channel_%d_start_time' %
                                  (chan+1)).data()
                if preTime < 0:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Pre time for channel %d must be geater of equal to 0 '+str(chan + 1))
                    raise DevBAD_PARAMETER

                postTime = getattr(self, 'channel_%d_end_time' %
                                   (chan+1)).data()
                if postTime < 0:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Post time for channel %d must be geater of equal to 0 '+str(chan + 1))
                    raise DevBAD_PARAMETER
            except:
                if mode == 'BURST(FREQ1)' or mode == 'BURST(FREQ2)' or mode == ('DUAL SPEED'):
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Missing pre or post time for channel '+str(chan + 1))
                    raise DevBAD_PARAMETER
                else:
                    preTime = 0
                    postTime = 0

            chanPreTimes.append(preTime)
            chanPostTimes.append(postTime)

            try:
                polarity = self.polarityDict[getattr(
                    self, 'channel_%d_polarity' % (chan+1)).data()]
                gain = self.gainDict[getattr(
                    self, 'channel_%d_range' % (chan+1)).data()]

                data = Data.compile("NIpxi6259analogInputScaled(build_path($), build_path($), $ )", getattr(
                    self, 'channel_%d_raw' % (chan+1)).getPath(),  getattr(self, 'channel_%d_calib' % (chan+1)).getPath(), gain)
                data.setUnits("Volts")
                getattr(self, 'channel_%d_data' % (chan+1)).putData(data)
            except Exception as e:
                self.debugPrint(str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan + 1))
                raise DevBAD_PARAMETER
            if(enabled):
                if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
                    currChan = self.diffChanMap[chan]
                else:
                    currChan = chan
                # self.debugPrint 'POLARITY: ' + str(polarity) + ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = NI6259EV.niLib.pxi6259_add_ai_channel(
                    aiConf, c_byte(currChan), polarity, gain, inputMode, c_byte(0))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot add channel '+str(currChan + 1))
                    raise DevBAD_PARAMETER
                #self.debugPrint('PXI 6259 CHAN '+ str(currChan+1) + ' CONFIGURED')
                activeChan = activeChan + 1
        # endfor
        nid = self.getNid()
        NI6259EV.ni6259chanModes[nid] = chanModes
        NI6259EV.ni6259chanEvents[nid] = chanEvents
        NI6259EV.ni6259chanPreTimes[nid] = chanPreTimes
        NI6259EV.ni6259chanPostTimes[nid] = chanPostTimes

# Continuous acquisiton
        status = NI6259EV.niLib.pxi6259_set_ai_attribute(
            aiConf, self.AI_CONTINUOUS, c_int(1))
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot set continuous acquisition')
            raise DevBAD_PARAMETER
# PFI0 clock (per ora)

        status = NI6259EV.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(16), c_int(
            3), self.AI_SAMPLE_SELECT_PFI0, self.AI_SAMPLE_POLARITY_RISING_EDGE)
#        status = NI6259EV.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(0), c_int(0), self.AI_SAMPLE_SELECT_PFI0, self.AI_SAMPLE_POLARITY_RISING_EDGE)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot configure device clock')
            raise DevBAD_PARAMETER

        if activeChan == 1:
            convClk = 16
        else:
            convClk = 20
        status = NI6259EV.niLib.pxi6259_set_ai_convert_clk(aiConf, c_int(convClk), c_int(
            3), self.AI_CONVERT_SELECT_SI2TC, self.AI_CONVERT_POLARITY_RISING_EDGE)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot Set Convert Clock')
            raise DevBAD_PARAMETER

        status = NI6259EV.niLib.pxi6259_load_ai_conf(c_int(self.fd), aiConf)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot load configuration')
            raise DevBAD_PARAMETER

        self.saveInfo()
        self.debugPrint("===============================================")
        return 1

# StartStore
    def start_store(self):

        self.debugPrint(
            '================= PXI 6259 EVstart store ===============')

        if self.restoreInfo() != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

# Module in acquisition check
        try:
            if self.restoreWorker():
                if self.worker.isAlive():
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Module is in acquisition')
                    return
        except:
            pass

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        NI6259EV.workers[self.nid] = self.worker

        chanMap = []
        stopAcq = c_void_p(0)
        NI6259EV.niInterfaceLib.getStopAcqFlag(byref(stopAcq))
        try:
            inputMode = self.inputModeDict[self.input_mode.data()]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid Input Mode')
            raise DevBAD_PARAMETER
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            numChannels = 16
        else:
            numChannels = 32
        for chan in range(0, numChannels):
            try:
                mode = getattr(self, 'channel_%d_mode' % (chan+1)).data()
                if mode != 'DISABLED':
                    chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid mode for channel '+str(chan + 1))
                raise DevBAD_PARAMETER
        treePtr = c_void_p(0)
        NI6259EV.niInterfaceLib.openTree(
            c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            self.worker.configure(self.copy(), self.fd, chanMap,
                                  self.diffChanMap, treePtr, stopAcq)
        else:
            self.worker.configure(self.copy(), self.fd, chanMap,
                                  self.nonDiffChanMap, treePtr, stopAcq)
        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        if self.worker.hasError():
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint(
            "======================================================")

        return 1

    def stop_store(self):

        self.debugPrint(
            '================= PXI 6259 EV stop store ================')
        error = False
        """
      if self.restoreInfo() != self.DEV_IS_OPEN :
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module not Initialized')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      """

        if not self.restoreWorker():
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not created')
            return

        if self.worker.isAlive():
            self.debugPrint("PXI 6259 stop_worker")
            self.worker.stop()
            self.worker.join()
            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Acquisition thread stopped')

        if error:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint(
            "======================================================")

        return 1

    def readConfig(self):

        self.restoreInfo()

        try:
            NI6259EV.niInterfaceLib.readAiConfiguration(c_int(self.fd))
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot read board configuration')
            raise DevBAD_PARAMETER
        return 1
