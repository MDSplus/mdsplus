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
#   2019 12 02: First version
#

from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Float32Array
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER

from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p, c_int8
import os
import time
import sys
import traceback
import socket


class BCM2835_ADC(Device):
    """BMC 2835 Seria Periferal Interface ADC acquisition"""
    parts = [{'path': ':BOARD_IP', 'type': 'text'},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 1000},
             {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 1000},
             {'path': ':SEG_LENGTH', 'type': 'numeric', 'value': 10000},
             {'path': ':CLOCK_SOURCE', 'type': 'numeric'},
             {'path': ':END_IDX', 'type': 'numeric'},
             {'path': ':START_IDX', 'type': 'numeric', 'value': 0},
             {'path': ':ACQ_MODE', 'type': 'text', 'value': 'TRANSIENT REC.'},
             {'path': ':TRIG_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0},
             {'path': ':USE_TIME', 'type': 'text', 'value': 'YES'},
             {'path': ':START_TIME', 'type': 'numeric', 'value': 0},
             {'path': ':END_TIME', 'type': 'numeric', 'value': 1},
             {'path': ':CONV_CLK', 'type': 'numeric', 'value': 20}]

    for i in range(0, 4):
        parts.append({'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_%d:GAIN' % (
            i+1), 'type': 'numeric', 'value': 1})
        parts.append({'path': '.CHANNEL_%d:OFFSET' % (
            i+1), 'type': 'numeric', 'value': 1})
        parts.append({'path': '.CHANNEL_%d:DATA' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
        parts.append({'path': '.CHANNEL_%d:DATA_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
        parts.append({'path': '.CHANNEL_%d:RES_RAW' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('RASPBERRY_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('RASPBERRY_SERVER','INIT',50,None),Method(None,'start_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('RASPBERRY_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':WAIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('RASPBERRY_SERVER','FINISH_SHOT',55,None),Method(None,'wait_store',head))",
                  'options': ('no_write_shot',)})

    triggerModeDict = {'INTERNAL': 0, 'EXTERNAL': 1}

# File descriptor
    fd = 0

# Driver constants
    bcm2835Fds = {}

#Dictionaries and maps
    bcm2835InterfaceLib = None
    asyncStoreManagerLib = None
    workers = {}

    DEV_IS_OPEN = 1
    DEV_OPEN = 2

    def getIpAddress(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
        local_ip_address = s.getsockname()[0]
        s.close()
        return local_ip_address

    def debugPrint(self, msg="", obj=""):
        msger = ""
        """ 
          if BCM2835_ADC.bcm2835InterfaceLib is not None:
              errno = BCM2835_ADC.bcm2835InterfaceLib.getErrno();
              print "erno ", errno
              if errno is not None:
                  msger = 'Error (%d) %s' % (errno, os.strerror( errno ))
          """
        print(self.name + ":" + msg, obj, msger)

# saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        BCM2835_ADC.bcm2835Fds[self.nid] = self.fd

    def restoreInfo(self):
        if BCM2835_ADC.bcm2835InterfaceLib is None:
            BCM2835_ADC.bcm2835InterfaceLib = CDLL("libBCM2835Interface.so")
        if BCM2835_ADC.asyncStoreManagerLib is None:
            BCM2835_ADC.asyncStoreManagerLib = CDLL("libAsyncStoreManager.so")

        if self.nid in BCM2835_ADC.bcm2835Fds.keys():
            self.fd = BCM2835_ADC.bcm2835Fds[self.nid]
            return self.DEV_IS_OPEN
        else:
            try:
                boardIp = self.board_ip.data()
                IPAddr = self.getIpAddress()
                if boardIp != IPAddr:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Invalid Board IP (%s) device run on IP %s' % (boardIp, IPAddr))
                    raise DevBAD_PARAMETER
            except:
                emsg = 'Missing Board IP'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            try:
                self.fd = BCM2835_ADC.bcm2835InterfaceLib.spiConfig()
            except Exception as e:
                emsg = 'Cannot initialized device %s : %s' % (boardIp, str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

        return self.DEV_OPEN

    def closeInfo(self):
        if self.nid in BCM2835_ADC.bcm2835Fds.keys():
            self.fd = BCM2835_ADC.bcm2835Fds[self.nid]
            del(BCM2835_ADC.bcm2835Fds[self.nid])
            try:
                if BCM2835_ADC.bcm2835InterfaceLib.spiClose() < 0:
                    emsg = 'Cannot CLOSE device id ' + str(self.fd)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise
            except Exception as ex:
                emsg = 'Cannot CLOSE device Exception : %s' % str(ex)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER(emsg)
        return

# Worker Management
    def saveWorker(self):
        BCM2835_ADC.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in BCM2835_ADC.workers.keys():
            self.worker = BCM2835_ADC.workers[self.nid]
        else:
            self.debugPrint('Cannot restore worker!!')

# AsynchStore class
    class AsynchStore(Thread):

        ACQ_NOERROR = 0
        ACQ_ERROR = 1

        def configure(self, device, fd, treePtr):
            self.device = device
            self.fd = fd
            self.treePtr = treePtr
            self.stopAcq = c_void_p(0)
            self.stopReq = False
            self.error = self.ACQ_NOERROR

        def run(self):
            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            acqMode = self.device.acq_mode.data()
            trigSource = self.device.trig_source.data()
            trigMode = self.device.triggerModeDict[self.device.trig_mode.data(
            )]

            startTime = float(self.device.start_time.data())
            clockSource = self.device.clock_source.evaluate()
            period = float(clockSource.getDelta())
            timeAt0 = startTime
            numSamples = -1  # continuous

            transientRec = False

            sampleToSkip = self.device.start_idx.data()
            #endIdx = self.device.end_idx.data()

            period = float(clockSource.getDelta())

            if(acqMode == 'TRANSIENT REC.'):
                transientRec = True

            chanNid = []
            resNid = []

            for chan in range(4):
                try:
                    chanNid.append(
                        getattr(self.device, 'channel_%d_data_raw' % (chan+1)).getNid())
                    resNid.append(
                        getattr(self.device, 'channel_%d_res_raw' % (chan+1)).getNid())
                except Exception as e:
                    self.device.debugPrint(str(e))
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'Cannot open Channel %d' % (chan))
                    self.error = self.ACQ_ERROR
                    return
            print('SEGEMNT SIZE PRIMA: ', segmentSize)
            if(not transientRec):

                if(bufSize > segmentSize):
                    segmentSize = bufSize
                else:
                    c = segmentSize/bufSize
                    if (segmentSize % bufSize > 0):
                        c = c+1
                    segmentSize = c*bufSize
                numSamples = -1
                self.device.debugPrint("BMC2835 CONTINUOUS ", numSamples)
            else:
                try:
                    numSamples = self.device.end_idx.data() - self.device.start_idx.data()
                except:
                    numSamples = 0
                self.device.debugPrint("BMC2835 NUM SAMPLES ", numSamples)

            print('SEGMENT SIZE DOPO: ', segmentSize)

            print ('startSave')
            saveList = c_void_p(0)
            BCM2835_ADC.asyncStoreManagerLib.startSave(byref(saveList))

            print ('getStopAcqFlag')
            BCM2835_ADC.bcm2835InterfaceLib.getStopAcqFlag(byref(self.stopAcq))
            print (self.stopAcq)

            #count = 0

            val = 0
            chanNid_c = (c_int * len(chanNid))(*chanNid)
            resNid_c = (c_int * len(resNid))(*resNid)

            #timeAt0 = trigSource + startTime
            #self.device.debugPrint("PXI 6259 TIME AT0 ", numSamples)
            timeAt0 = startTime

            if transientRec:
                BCM2835_ADC.bcm2835InterfaceLib.setStopAcqFlag(self.stopAcq)

            while not self.stopReq:
                print ('in bcn2835_readAndSaveAllChannels')
                status = BCM2835_ADC.bcm2835InterfaceLib.bcn2835_readAndSaveAllChannels(c_int(bufSize), c_int(segmentSize), c_int(numSamples), chanNid_c, self.device.clock_source.getNid(
                ), c_float(timeAt0), c_float(period), self.treePtr, saveList, (self.stopAcq), c_int(self.device.getTree().shot), resNid_c, c_int8(trigMode))
                print ('out bcn2835_readAndSaveAllChannels')

   # Check termination
                if (numSamples > 0 or (transientRec and status == -1)):
                    self.stopReq = True

            if(transientRec and status == -1):
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'BMC2835 Module is not in stop state')
                self.error = self.ACQ_ERROR

            BCM2835_ADC.asyncStoreManagerLib.stopSave(saveList)
            BCM2835_ADC.bcm2835InterfaceLib.freeStopAcqFlag(self.stopAcq)

            # On first test dosen't work
            # BCM2835_ADC.bcm2835InterfaceLib.closeTree(self.treePtr)
            #self.device.debugPrint('CLOSE TREE')

            self.device.closeInfo()

            return

        def stop(self):
            self.stopReq = True
            BCM2835_ADC.bcm2835InterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)


# End Inner class AsynchStore

##########init############################################################################
    def init(self):

        self.debugPrint('=================  BMC2835 Init ===============')


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

# Channel configuration
        activeChan = 0
        for chan in range(0, 4):
            # Empy the node which will contain  the segmented data
            getattr(self, 'channel_%d_data_raw' % (chan+1)).deleteData()
            getattr(self, 'channel_%d_data_raw' %
                    (chan+1)).setCompressOnPut(False)
            getattr(self, 'channel_%d_res_raw' % (chan+1)).deleteData()
            getattr(self, 'channel_%d_res_raw' %
                    (chan+1)).setCompressOnPut(False)
            try:
                data = Data.compile("$1 * $2 + $3", getattr(self, 'channel_%d_data_raw' % (chan+1)),  getattr(
                    self, 'channel_%d_gain' % (chan+1)), getattr(self, 'channel_%d_offset' % (chan+1)))
                data.setUnits("Volts")
                getattr(self, 'channel_%d_data' % (chan+1)).putData(data)
            except Exception as e:
                self.debugPrint(estr(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid Configuration for channel '+str(chan + 1))
                raise DevBAD_PARAMETER
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
            self.debugPrint('BCM2835 Trigger mode: ', trigMode)
            if(trigMode == 'EXTERNAL'):
                if(acqMode == 'TRANSIENT REC.'):
                    print("External Trigger Transient recorder")
                else:
                    print ("External Trigger Continuous recorder")
            else:
                if(acqMode == 'TRANSIENT REC.'):
                    print ("Internal Trigger Transient recorder")
                else:
                    print ("Internal Trigger Continuous recorder")
        except:
            traceback.print_exc(file=sys.stdout)
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid triger mode definition')
            raise DevBAD_PARAMETER

# trigger source
        try:
            if(trigMode == 'EXTERNAL'):
                trigSource = self.trig_source.data()
            else:
                try:
                    trigSource = self.trig_source.data()
                except:
                    trigSource = 0
            self.debugPrint('BCM2835 Trigger source: ', trigSource)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve Trigger source')
            raise DevBAD_PARAMETER

# clock mode
        try:
            clockMode = self.clock_mode.data()
            if(clockMode == 'INTERNAL'):
                frequency = self.clock_freq.data()
                if(frequency > 10000 or frequency <= 0):
                    self.debugPrint('BCM2835 Frequency out of limits')
                    frequency = 1000.
                    self.clock_source.putData(frequency)

                clockSource = Range(None, None, Float64(1./frequency))
                self.debugPrint('BCM2835 CLOCK: ', clockSource)
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
                print ("External clock")
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
                    self.debugPrint('BCM2835 startTime = ', startTime)
                    self.debugPrint('BCM2835 endTime   = ', endTime)
                    self.debugPrint('BCM2835 trigSource   = ', trigSource)
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Cannot Read Start or End time')
                    raise DevBAD_PARAMETER

                startIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigSource), clockSource), startTime)
                endIdx = Data.execute('x_to_i($1, $2)', Dimension(
                    Window(0, None, trigSource), clockSource), endTime)

                self.debugPrint('BCM2835 startIdx = ',
                                Int32(int(startIdx + 0.5)))
                self.start_idx.putData(Int32(int(startIdx + 0.5)))

                self.debugPrint('BCM2835 endIdx   = ',
                                Int32(int(endIdx + 0.5)))
                self.end_idx.putData(Int32(int(endIdx + 0.5)))

                # self.prts.putData(Int32(int(preTrigger)))
                # self.num_samples.putData(Int32(int(postTrigger)))

            else:
                endIdx = self.end_idx.data()
                startIdx = self.start_idx.data()

            nSamples = endIdx - startIdx + 1
            postTrigger = nSamples + startIdx
            if startIdx >= 0:
                self.debugPrint('BCM2835 Acquire pre and post trigger')
            else:
                if trigSource > startTime:
                    self.debugPrint('BCM2835 Acquire only post trigger')
                    nSamples = postTrigger
                    startIdx = 0
                    self.start_idx.putData(Int32(int(0)))
                    self.start_time.putData(Float32(trigSource))

            self.debugPrint('BCM2835 nSamples   = ', Int32(int(nSamples)))

        else:  # Continuous Acquisition
            nSamples = -1

        self.saveInfo()
        self.debugPrint("===============================================")
        return 1

# StartStore
    def start_store(self):

        self.debugPrint(
            '================= BCM2835 start store ===============')

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

        print ('Tree opening')
        treePtr = c_void_p(0)
        BCM2835_ADC.asyncStoreManagerLib.openTree(
            c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))
        print ('Tree opened')

        self.worker.configure(self, self.fd, treePtr)

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
            '================= BCM2835 stop store ================')
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

        self.debugPrint(
            "======================================================")

        return 1

    def wait_store(self):

        self.debugPrint(
            '================= BCM2835 wait store ================')

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
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.debugPrint(
            "======================================================")
        return 1
