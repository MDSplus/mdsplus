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
#
#   2019 03 07: Trig time is initialized to trig source value also in INTERNAL mode
#

from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window
from MDSplus import Int32, Float32, Float64, Float32Array, Tree
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER

from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p
import os
import time
import sys
import traceback


class NI6259AI(Device):
    """NI PXI-6259 M-series multi functional data acquisition card"""
    parts = [{'path': ':BOARD_ID', 'type': 'numeric', 'value': 0},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':INPUT_MODE', 'type': 'text', 'value': 'RSE'},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 1000},
             {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 1000},
             {'path': ':SEG_LENGTH', 'type': 'numeric', 'value': 10000},
             {'path': ':CLOCK_SOURCE', 'type': 'numeric'}]

    for i in range(0, 32):
        parts.append({'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d:STATE' % (
            i+1), 'type': 'text', 'value': 'ENABLED'})
        parts.append({'path': '.CHANNEL_%d:POLARITY' % (
            i+1), 'type': 'text', 'value': 'BIPOLAR'})
        parts.append({'path': '.CHANNEL_%d:RANGE' % (
            i+1), 'type': 'text', 'value': '10V'})
        parts.append({'path': '.CHANNEL_%d:DATA' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})

    parts.append({'path': ':END_IDX', 'type': 'numeric'})

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':START_IDX', 'type': 'numeric', 'value': 0})
    parts.append({'path': ':ACQ_MODE', 'type': 'text',
                  'value': 'TRANSIENT REC.'})
    parts.append({'path': ':TRIG_MODE', 'type': 'text', 'value': 'INTERNAL'})
    parts.append({'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0})

    parts.append({'path': ':USE_TIME', 'type': 'text', 'value': 'YES'})
    parts.append({'path': ':START_TIME', 'type': 'numeric', 'value': 0})
    parts.append({'path': ':END_TIME', 'type': 'numeric', 'value': 1})

    for i in range(0, 32):
        parts.append({'path': '.CHANNEL_%d:DATA_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
        parts.append({'path': '.CHANNEL_%d:CALIB_PARAM' % (i+1),
                      'type': 'numeric', 'options': ('no_write_model')})
    del(i)

    parts.append({'path': ':CONV_CLK', 'type': 'numeric', 'value': 20})
    parts.append({'path': ':SERIAL_NUM', 'type': 'numeric'})

    for i in range(0, 32):
        parts.append({'path': '.CHANNEL_%d:RES_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})

    parts.append({'path': ':WAIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',55,None),Method(None,'wait_store',head))",
                  'options': ('no_write_shot',)})

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

    AI_START_SELECT_PULSE = c_int(0)
    AI_START_SELECT_PFI1 = c_int(2)

    AI_START_SELECT_RTSI0 = c_int(11)
    AI_START_SELECT_RTSI1 = c_int(12)


# check CODAC version for
    try:
        codacVer = os.environ.get('CODAC_VERSION')
        if codacVer[0] == '5':
            # Codac Core 5.0
            AI_START_SELECT = c_int(10)
            AI_START_POLARITY = c_int(11)
        else:
            # Codac Core 6.0
            AI_START_SELECT = c_int(11)
            AI_START_POLARITY = c_int(12)
    except:
        pass

    AI_START_POLARITY_RISING_EDGE = c_int(0)
    AI_REFERENCE_SELECT_PULSE = c_int(0)
    AI_REFERENCE_SELECT_PFI1 = c_int(2)

 # Codac Core 5.0
    AI_REFERENCE_SELECT = c_int(12)
    AI_REFERENCE_POLARITY = c_int(13)
    AI_REFERENCE_POLARITY_RISING_EDGE = c_int(0)
    PXI6259_AI_START_TRIGGER = c_int(3)
    PXI6259_RTSI1 = c_int(3)
    PXI6259_RTSI2 = c_int(4)

#Dictionaries and maps
    inputModeDict = {'RSE': AI_CHANNEL_TYPE_RSE, 'NRSE': AI_CHANNEL_TYPE_NRSE,
                     'DIFFERENTIAL': AI_CHANNEL_TYPE_DIFFERENTIAL}
    enableDict = {'ENABLED': True, 'DISABLED': False}
    gainDict = {'10V': c_byte(1), '5V': c_byte(2), '2V': c_byte(3), '1V': c_byte(
        4), '500mV': c_byte(5), '200mV': c_byte(6), '100mV': c_byte(7)}
    gainValueDict = {'10V': 10., '5V': 5., '2V': 2.,
                     '1V': 1., '500mV': 0.5, '200mV': 0.2, '100mV': 0.1}
    gainDividerDict = {'10V': 1., '5V': 2., '2V': 5.,
                       '1V': 10., '500mV': 20., '200mV': 50., '100mV': 100.}

    polarityDict = {'UNIPOLAR': AI_POLARITY_UNIPOLAR,
                    'BIPOLAR': AI_POLARITY_BIPOLAR}
    diffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23]
    nonDiffChanMap = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32]
    niLib = None
    niInterfaceLib = None
    ni6259Fds = {}
    workers = {}

    DEV_IS_OPEN = 1
    DEV_OPEN = 2

    def debugPrint(self, msg="", obj=""):
        msger = ""
        if NI6259AI.niInterfaceLib is not None:
            errno = NI6259AI.niInterfaceLib.getErrno()
            msger = ''
            # errno 11 Resource temporarily unavailable is removed
            if errno is not None and errno != 0 and errno != 11:
                msger = 'Error (%d) %s' % (errno, os.strerror(errno))
        print(self.name + ":" + msg, obj, msger)
        sys.stdout.flush()

# saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        NI6259AI.ni6259Fds[self.nid] = self.fd

    def restoreInfo(self):
        if NI6259AI.niLib is None:
            NI6259AI.niLib = CDLL("libpxi6259.so")
        if NI6259AI.niInterfaceLib is None:
            NI6259AI.niInterfaceLib = CDLL("libNiInterface.so")

        if self.nid in NI6259AI.ni6259Fds.keys():
            self.fd = NI6259AI.ni6259Fds[self.nid]
            return self.DEV_IS_OPEN
        else:
            try:
                boardId = self.board_id.data()
            except:
                emsg = 'Missing Board Id'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            try:
                devName = '/dev/pxi6259.'+str(boardId)
                dfd = os.open(devName, os.O_RDWR)
                serialNum = c_int(0)
                if NI6259AI.niLib.pxi6259_get_board_serial_number(dfd, byref(serialNum)) == 0:
                    self.serial_num.putData(serialNum)
                os.close(dfd)
            except:
                pass
            try:
                fileName = '/dev/pxi6259.'+str(boardId)+'.ai'
                self.fd = os.open(fileName, os.O_RDWR)
            except Exception as e:
                emsg = 'Cannot open device %s : %s' % (fileName, str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

        """
        try:
            if( NI6259AI.niLib.pxi6259_reset_ai(self.fd) ):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot reset device '+ fileName)
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        return self.DEV_OPEN

    def closeInfo(self):
        if self.nid in NI6259AI.ni6259Fds.keys():
            self.fd = NI6259AI.ni6259Fds[self.nid]
            del(NI6259AI.ni6259Fds[self.nid])
            try:
                if NI6259AI.niLib.pxi6259_reset_ai(self.fd):
                    emsg = 'Cannot reset device ' + str(self.fd)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevBAD_PARAMETER(emsg)
            except:
                emsg = 'Cannot reset device ' + str(self.fd)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            os.close(self.fd)

# Worker Management
    def saveWorker(self):
        NI6259AI.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in NI6259AI.workers.keys():
            self.worker = NI6259AI.workers[self.nid]
        else:
            self.debugPrint('Cannot restore worker!!')

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

            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            #counters = [0]*len(self.chanMap)
            boardId = self.device.board_id.data()
            acqMode = self.device.acq_mode.data()
            transientRec = False

            sampleToSkip = self.device.start_idx.data()
            #endIdx = self.device.end_idx.data()

            startTime = float(self.device.start_time.data())
            #trigSource = self.device.trig_source.data()
            clockSource = self.device.clock_source.evaluate()

            period = float(clockSource.getDelta())

            if(acqMode == 'TRANSIENT REC.'):
                transientRec = True

            chanFd = []
            chanNid = []
            resNid = []
            coeffsNid = []

            coeff_array = c_float*4
            coeff = coeff_array()

            # NI6259AI.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq));
            gainDividers = []
            for chan in range(len(self.chanMap)):
                try:
                    # self.device.debugPrint 'CHANNEL', self.chanMap[chan]+1
                    # self.device.debugPrint '/dev/pxi6259.'+str(boardId)+'.ai.'+str(self.hwChanMap[self.chanMap[chan]])
                    currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(
                        self.hwChanMap[self.chanMap[chan]]), os.O_RDWR | os.O_NONBLOCK)
                    chanFd.append(currFd)

                    chanNid.append(getattr(self.device, 'channel_%d_data_raw' % (
                        self.chanMap[chan]+1)).getNid())
                    # self.device.debugPrint "chanFd "+'channel_%d_data'%(self.chanMap[chan]+1), chanFd[chan], " chanNid ", chanNid[chan]
                    resNid.append(getattr(self.device, 'channel_%d_res_raw' % (
                        self.chanMap[chan]+1)).getNid())
                    coeffsNid.append(getattr(self.device, 'channel_%d_calib_param' % (
                        self.chanMap[chan]+1)).getNid())

                    gain = getattr(self.device, 'channel_%d_range' %
                                   (self.chanMap[chan]+1)).data()
                    gain_code = self.device.gainDict[gain]
                    gainDividers.append(self.device.gainDividerDict[gain])
                    n_coeff = c_int(0)
                    status = NI6259AI.niInterfaceLib.pxi6259_getCalibrationParams(
                        currFd, gain_code, coeff, byref(n_coeff))

                    if(status < 0):
                        errno = NI6259AI.niInterfaceLib.getErrno()
                        msg = 'Error (%d) %s' % (errno, os.strerror(errno))
                        self.device.debugPrint(msg)
                        Data.execute('DevLogErr($1,$2)', self.device.getNid(
                        ), 'Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536' % ((self.chanMap[chan])))
                        gainValue = self.device.gainValueDict[gain] * 2.
                        coeff[0] = coeff[2] = coeff[3] = 0
                        coeff[1] = c_float(gainValue / 65536.)

                    getattr(self.device, 'channel_%d_calib_param' %
                            (self.chanMap[chan]+1)).putData(Float32Array(coeff))

                except Exception as e:
                    self.device.debugPrint(e)
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'Cannot open Channel ' + str(self.chanMap[chan]))
                    self.error = self.ACQ_ERROR
                    return
            if(not transientRec):

                if(bufSize > segmentSize):
                    segmentSize = bufSize
                else:
                    c = segmentSize/bufSize
                    if (segmentSize % bufSize > 0):
                        c = c+1
                    segmentSize = c*bufSize

                numSamples = -1
                self.device.debugPrint("PXI 6259 CONTINUOUS ", numSamples)

            else:
                NI6259AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)

                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data()
                except:
                    numSamples = 0
                    self.device.debugPrint("PXI 6259 NUM SAMPLES ", numSamples)

            status = NI6259AI.niLib.pxi6259_start_ai(c_int(self.fd))

            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'Cannot Start Acquisition ')
                self.error = self.ACQ_ERROR
                return

            saveList = c_void_p(0)
            NI6259AI.niInterfaceLib.startSave(byref(saveList))
            #count = 0

            val = 0
            chanNid_c = (c_int * len(chanNid))(*chanNid)
            chanFd_c = (c_int * len(chanFd))(*chanFd)
            resNid_c = (c_int * len(resNid))(*resNid)
            coeffsNid_c = (c_int * len(coeffsNid))(*coeffsNid)
            gainDividers_c = (c_float * len(gainDividers))(*gainDividers)

            #timeAt0 = trigSource + startTime
            #self.device.debugPrint("PXI 6259 TIME AT0 ", numSamples)
            timeAt0 = startTime

            while not self.stopReq:
                try:
                    status = NI6259AI.niInterfaceLib.pxi6259_readAndSaveAllChannels(c_int(len(self.chanMap)), chanFd_c, c_int(int(round(bufSize))), c_int(int(round(segmentSize))), c_int(sampleToSkip), c_int(
                        numSamples), chanNid_c, gainDividers_c, coeffsNid_c, self.device.clock_source.getNid(), c_float(timeAt0), c_float(period), self.treePtr, saveList, self.stopAcq, c_int(self.device.getTree().shot), resNid_c)
                except Exception as ex:
                    self.device.debugPrint(
                        'Acquisition thread start error : %s' % (str(ex)))
                    self.error = self.ACQ_ERROR
                    self.stopReq = True

   # Check termination
                if (numSamples > 0 or (transientRec and status == -1)):
                    self.stopReq = True

                if status < 0:
                    self.device.debugPrint(
                        'Acquisition Loop exit with code %d' % (status))
                    self.error = self.ACQ_ERROR
                    self.stopReq = True

            if(transientRec and status == -1):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'PXI 6259 Module is not in stop state')
                self.error = self.ACQ_ERROR

            if(status < 0):
                if(status == -1):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PXI 6259 Module is not triggered')
                if(status == -2):
                    Data.execute('DevLogErr($1,$2)',
                                 self.device.getNid(), 'PXI 6259 DMA overflow')
                if(status == -3):
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PXI 6259 Exception on acquisition function')
                self.error = self.ACQ_ERROR

            status = NI6259AI.niLib.pxi6259_stop_ai(c_int(self.fd))

            for chan in range(len(self.chanMap)):
                os.close(chanFd[chan])
            self.device.debugPrint('ASYNCH WORKER TERMINATED')
            NI6259AI.niInterfaceLib.stopSave(saveList)
            NI6259AI.niInterfaceLib.freeStopAcqFlag(self.stopAcq)

            self.device.closeInfo()

        def stop(self):
            self.stopReq = True
            NI6259AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)

        def closeTree(self):
            # On first test dosen't work
            NI6259AI.niInterfaceLib.closeTree(self.treePtr)
            self.device.debugPrint('CLOSE TREE')


# End Inner class AsynchStore

##########init############################################################################


    def init(self):

        self.debugPrint('================= PXI 6259 Init ===============')
        print('USE PRIVATE CTX: '+str(self.getTree().usingPrivateCtx()))

# Module in acquisition check
        if self.restoreInfo() == self.DEV_IS_OPEN:
            try:
                self.restoreWorker()
                if self.worker.isAlive():
                    print ('stop Store')
                    self.stop_store()
                self.restoreInfo()
            except:
                pass

        aiConf = c_void_p(0)
        NI6259AI.niInterfaceLib.pxi6259_create_ai_conf_ptr(byref(aiConf))
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
        for chan in range(0, numChannels):

            # Empy the node which will contain  the segmented data
            getattr(self, 'channel_%d_data_raw' % (chan+1)).deleteData()
            getattr(self, 'channel_%d_data_raw' %
                    (chan+1)).setCompressOnPut(False)
            getattr(self, 'channel_%d_res_raw' % (chan+1)).deleteData()
            getattr(self, 'channel_%d_res_raw' %
                    (chan+1)).setCompressOnPut(False)
            try:
                enabled = self.enableDict[getattr(
                    self, 'channel_%d_state' % (chan+1)).data()]
                polarity = self.polarityDict[getattr(
                    self, 'channel_%d_polarity' % (chan+1)).data()]
                gain = self.gainDict[getattr(
                    self, 'channel_%d_range' % (chan+1)).data()]

                data = Data.compile("NIpxi6259analogInputScaled(build_path($), build_path($), $ )", getattr(
                    self, 'channel_%d_data_raw' % (chan+1)).getPath(),  getattr(self, 'channel_%d_calib_param' % (chan+1)).getPath(), gain)
                data.setUnits("Volts")
                getattr(self, 'channel_%d_data' % (chan+1)).putData(data)
            except Exception as e:
                self.debugPrint(estr(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan + 1))
                raise DevBAD_PARAMETER
            if(enabled):
                if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
                    currChan = self.diffChanMap[chan]
                else:
                    currChan = chan
                # self.debugPrint 'POLARITY: ' + str(polarity) + ' GAIN: ' + str(gain) + ' INPUT MODE: ' + str(inputMode)
                status = NI6259AI.niLib.pxi6259_add_ai_channel(
                    aiConf, c_byte(currChan), polarity, gain, inputMode, c_byte(0))
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot add channel '+str(currChan + 1))
                    raise DevBAD_PARAMETER
                #self.debugPrint('PXI 6259 CHAN '+ str(currChan+1) + ' CONFIGURED')
                activeChan = activeChan + 1
        # endfor

        """
        try:
            nSamples = self.num_samples.data()
        except:
            nSamples = -1
        """

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
            self.debugPrint('PXI 6259 Trigger mode: ', trigMode)
            if(trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXTERNAL_RTSI1' or trigMode == 'EXT_PFI1_R_RTSI1'):
                # self.debugPrint "AI_START_SELECT ", self.AI_START_SELECT
                # self.debugPrint "aiConf ", aiConf
                # self.debugPrint "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                # self.debugPrint "niLib ", NI6259AI.niLib
                # self.debugPrint "AI_START_POLARITY ", self.AI_START_POLARITY
                # self.debugPrint "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE

                if(acqMode == 'TRANSIENT REC.'):
                    """
                    status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
                    self.debugPrint "status ", status
                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PFI1)
                        self.debugPrint "status ", status
                    if( status == 0 ):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
                        self.debugPrint "status ", status
                    if( status != 0 ):
                        self.debugPrint "status ", status
                        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot set external trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    """
                    if(trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXT_PFI1_R_RTSI1'):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)
                        self.debugPrint('AI_START_SELECT_PFI1 %d' % (status))
                    else:
                        self.debugPrint("1 OK AI_START_SELECT_RTSI1")
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)
                    if(status == 0):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                        self.debugPrint(
                            'AI_START_POLARITY_RISING_EDGE %d' % (status))
                    if(status != 0):
                        Data.execute('DevLogErr($1,$2)', self.getNid(),
                                     'Cannot set external trigger')
                        raise DevBAD_PARAMETER

                else:
                    if(trigMode == 'EXT_PFI1_R_RTSI1'):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PFI1)
                    else:
                        self.debugPrint("2 OK AI_START_SELECT_RTSI1")
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_SELECT, self.AI_START_SELECT_RTSI1)

                    if(status == 0):
                        status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                            aiConf, self.AI_START_POLARITY, self.AI_START_POLARITY_RISING_EDGE)
                    if(status != 0):
                        Data.execute('DevLogErr($1,$2)', self.getNid(),
                                     'Cannot set external trigger')
                        raise DevBAD_PARAMETER

                if(trigMode == 'EXT_PFI1_R_RTSI1'):
                    status = NI6259AI.niLib.pxi6259_export_ai_signal(
                        aiConf, self.PXI6259_AI_START_TRIGGER,  self.PXI6259_RTSI1)
                    if(status != 0):
                        Data.execute('DevLogErr($1,$2)', self.getNid(),
                                     'Cannot route PFI1 signal to RTSI1')
                        raise DevBAD_PARAMETER
            else:
                # self.debugPrint "AI_START_SELECT ", self.AI_START_SELECT
                # self.debugPrint "aiConf ", aiConf
                # self.debugPrint "AI_START_SELECT_PFI1 ", self.AI_START_SELECT_PFI1
                # self.debugPrint "niLib ", NI6259AI.niLib
                # self.debugPrint "AI_START_POLARITY ", self.AI_START_POLARITY
                # self.debugPrint "AI_START_POLARITY_RISING_EDGE ", self.AI_START_POLARITY_RISING_EDGE
                # self.debugPrint "acqMode ", acqMode

                if(acqMode == 'TRANSIENT REC.'):
                    #status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_START_SELECT, self.AI_START_SELECT_PULSE)
                    # if( status == 0 ):
                    status = NI6259AI.niLib.pxi6259_set_ai_attribute(
                        aiConf, self.AI_REFERENCE_SELECT, self.AI_REFERENCE_SELECT_PULSE)
                    # if( status == 0 ):
                    #    status = NI6259AI.niLib.pxi6259_set_ai_attribute(aiConf, self.AI_REFERENCE_POLARITY, self.AI_REFERENCE_POLARITY_RISING_EDGE)
                    if(status != 0):
                        Data.execute('DevLogErr($1,$2)', self.getNid(),
                                     'Cannot set external trigger')
                        raise DevBAD_PARAMETER

        except:
            traceback.print_exc(file=sys.stdout)
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid triger mode definition')
            raise DevBAD_PARAMETER

# trigger source
        try:
            if(trigMode == 'EXTERNAL_PFI1' or trigMode == 'EXTERNAL_RTSI1' or trigMode == 'SW_RTSI1'):
                trigSource = self.trig_source.data()
            else:
                try:
                    trigSource = self.trig_source.data()
                except:
                    trigSource = 0
            self.debugPrint('PXI 6259 Trigger source: ', trigSource)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve Trigger source')
            raise DevBAD_PARAMETER

# clock mode
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if((activeChan == 1 and frequency > 1250000) or (activeChan > 1 and frequency > 1000000./activeChan)):
                    self.debugPrint('PXI 6259 Frequency out of limits')
                    if(activeChan == 1):
                        frequency = 1250000.
                    else:
                        frequency = 1000000./activeChan
                    self.clock_source.putData(frequency)

                divisions = int(20000000./frequency)
                status = NI6259AI.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(divisions), c_int(
                    3), self.AI_SAMPLE_SELECT_SI_TC, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot Set Sample Clock')
                    raise DevBAD_PARAMETER
                """
                if nSamples > 0:
                    clockSource = Range(Float64(0), Float64(nSamples * divisions/20000000.) , Float64(divisions/20000000.))
                else:
                    clockSource = Range(Float64(0), Float64(3600), Float64(divisions/20000000.))
                """
                clockSource = Range(None, None, Float64(divisions/20000000.))
                self.debugPrint('PXI 6259 CLOCK: ', clockSource)
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
                status = NI6259AI.niLib.pxi6259_set_ai_sample_clk(aiConf, c_int(16), c_int(
                    3), self.AI_SAMPLE_SELECT_PFI0, self.AI_SAMPLE_POLARITY_RISING_EDGE)
                if(status != 0):
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot configure device clock')
                    raise DevBAD_PARAMETER

            convClk = self.conv_clk.data()
            if (activeChan == 1 and convClk == 20):
                convClk = 16

            status = NI6259AI.niLib.pxi6259_set_ai_convert_clk(aiConf, c_int(convClk), c_int(
                3), self.AI_CONVERT_SELECT_SI2TC, self.AI_CONVERT_POLARITY_RISING_EDGE)
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot Set Convert Clock')
                raise DevBAD_PARAMETER
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid clock definition')
            raise DevBAD_PARAMETER

# Time management
        if acqMode == 'TRANSIENT REC.':
            try:
                useTime = self.use_time.data()

            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve time or samples management')
                raise DevBAD_PARAMETER

            if useTime == 'YES':
                try:
                    startTime = self.start_time.data()
                    endTime = self.end_time.data()

                    self.debugPrint('PXI 6259 startTime = ', startTime)
                    self.debugPrint('PXI 6259 endTime   = ', endTime)
                    self.debugPrint('PXI 6259 trigSource   = ', trigSource)

                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot Read Start or End time')
                    raise DevBAD_PARAMETER

                startIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigSource), clockSource), startTime)

                endIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigSource), clockSource), endTime)

                """
                if endTime > 0:
                    endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), endTime + trigSource)
                else:
                    endIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + endTime), clockSource), trigSource)

                self.debugPrint 'endIdx   = ', Int32(int(endIdx))
                self.end_idx.putData(Int32(int(endIdx)))

                if startTime > 0:
                    startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), startTime + trigSource)
                else:
                    startIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + startTime), clockSource), trigSource)
                """
                self.debugPrint('PXI 6259 startIdx = ',
                                Int32(int(startIdx + 0.5)))
                self.start_idx.putData(Int32(int(startIdx + 0.5)))

                self.debugPrint('PXI 6259 endIdx   = ',
                                Int32(int(endIdx + 0.5)))
                self.end_idx.putData(Int32(int(endIdx + 0.5)))

                # self.prts.putData(Int32(int(preTrigger)))
                # self.num_samples.putData(Int32(int(postTrigger)))

            else:
                endIdx = self.end_idx.data()
                startIdx = self.start_idx.data()

                """
                postTrigger = nSamples;
                preTrigger = 0
                try:
                    preTrigger = self.prts.data()
                except:
                    preTrigger = 0
                    nSamples =  postTrigger + preTrigger
                """

            nSamples = endIdx - startIdx + 1

            postTrigger = nSamples + startIdx
            #preTrigger  = nSamples - endIdx

            if startIdx >= 0:
                status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(
                    aiConf, c_int(startIdx + nSamples), 0, 0)
            else:
                if trigSource > startTime:
                    self.debugPrint('PXI 6259 Acquire only post trigger')
                    nSamples = postTrigger
                    startIdx = 0
                    self.start_idx.putData(Int32(int(0)))
                    self.start_time.putData(Float32(trigSource))
                    status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(
                        aiConf, c_int(nSamples), 0, 0)

            """
            nSamples = endIdx - startIdx + 1
            postTrigger = nSamples + startIdx
            preTrigger = nSamples - endIdx
            """
            self.debugPrint('PXI 6259 nSamples   = ', Int32(int(nSamples)))

#           status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(postTrigger), c_int(preTrigger), 0)

        else:  # Continuous Acquisition
            nSamples = -1
            # if nSamples > 0:
            #    status = NI6259AI.niLib.pxi6259_set_ai_number_of_samples(aiConf, c_int(nSamples), 0, 0)

        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot Set Number of Samples')
            raise DevBAD_PARAMETER

        status = NI6259AI.niLib.pxi6259_load_ai_conf(c_int(self.fd), aiConf)
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot load configuration')
            raise DevBAD_PARAMETER

        """
        if acqMode == 'TRANSIENT REC.':
            status = niLib.pxi6259_start_ai(c_int(self.fd))
            #status = 0

            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot Start Acquisition ')
                return
        """
        self.saveInfo()
        self.debugPrint("===============================================")

# StartStore
    def start_store(self):

        self.debugPrint(
            '================= PXI 6259 start store ===============')

        if self.restoreInfo() != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

# Module in acquisition check
        try:
            self.restoreWorker()
            if self.worker.isAlive():
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Module is in acquisition')
                return
        except:
            pass

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        chanMap = []
        stopAcq = c_void_p(0)
        NI6259AI.niInterfaceLib.getStopAcqFlag(byref(stopAcq))
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
                enabled = self.enableDict[getattr(
                    self, 'channel_%d_state' % (chan+1)).data()]
                if enabled:
                    chanMap.append(chan)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan + 1))
                raise DevBAD_PARAMETER
        treePtr = c_void_p(0)
        NI6259AI.niInterfaceLib.openTree(
            c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        if(inputMode == self.AI_CHANNEL_TYPE_DIFFERENTIAL):
            self.worker.configure(self.copy(), self.fd,
                                  chanMap, self.diffChanMap, treePtr, stopAcq)
        else:
            self.worker.configure(
                self.copy(), self.fd, chanMap, self.nonDiffChanMap, treePtr, stopAcq)
        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        if self.worker.hasError():
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint(
            "======================================================")

    def stop_store(self):

        self.debugPrint(
            '================= PXI 6259 stop store ================')
#      error = False
        """
      if self.restoreInfo() != self.DEV_IS_OPEN :
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module not Initialized')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      """

        try:
            self.restoreWorker()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not created')
            return

        if self.worker.isAlive():
            self.debugPrint("PXI 6259 stop_worker")
            self.worker.stop()
 #        self.worker.join() worker therad waithing ternmination in wait_store method
            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Acquisition thread stopped')

        if error:
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint(
            "======================================================")

    def wait_store(self):

        self.debugPrint(
            '================= PXI 6259 wait store ================')

        try:
            self.restoreWorker()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread not created')
            return

        if self.worker.isAlive():
            self.worker.stop()
            self.worker.join()
            error = self.worker.hasError()
        else:
            error = self.worker.hasError()
            if not error:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Acquisition thread already stopped')

        if error:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Acquisition thread exit with fault')
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # self.worker.closeTree()

        self.debugPrint(
            "======================================================")

    def readConfig(self):

        self.restoreInfo()

        try:
            NI6259AI.niInterfaceLib.readAiConfiguration(c_int(self.fd))
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot read board configuration')
            raise DevBAD_PARAMETER

    def trigger(self):

        if self.restoreInfo() != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not Initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            status = NI6259AI.niLib.pxi6259_start_ai(c_int(self.fd))
            if(status != 0):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'Cannot Start Acquisition ')
                raise DevBAD_PARAMETER
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Exception Cannot Start Acquisition')
            raise DevBAD_PARAMETER
