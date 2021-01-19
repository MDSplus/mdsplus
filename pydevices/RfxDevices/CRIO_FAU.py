from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64
from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p, c_uint, c_short, c_byte, c_double, c_uint64
import os
from time import sleep
import sys
import traceback


class CRIO_FAU(Device):
    """NI Compact RIO SPIDER Interlock Fast Acquisition Units"""
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':FIFO_DEPTH', 'type': 'numeric', 'value': 30000},
             {'path': ':PTE_FRQ_SLOW', 'type': 'numeric', 'value': 50},
             {'path': ':PTE_FRQ_FAST', 'type': 'numeric', 'value': 50000},
             {'path': ':TSMP_FREQ', 'type': 'numeric', 'value': 40000000},
             {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 10000},
             {'path': ':TRIG_MODE', 'type': 'text', 'value': "INTERNAL"},
             {'path': ':TRIG_SOURCE', 'type': 'numeric'}]
    for i in range(0, 8):
        parts.append({'path': '.MODULE_%d' % (i+1), 'type': 'structure'})
        for j in range(0, 8):
            parts.append({'path': '.MODULE_%d.CHANNEL_%d' % (
                i+1, j), 'type': 'structure'})
            parts.append(
                {'path': '.MODULE_%d.CHANNEL_%d:DESCRIPTION' % (i+1, j), 'type': 'text'})
            parts.append({'path': '.MODULE_%d.CHANNEL_%d:DATA' % (
                i+1, j), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
            parts.append({'path': '.MODULE_%d.CHANNEL_%d:PTE_MODE' % (
                i+1, j), 'type': 'text', 'value': 'DISABLED'})

    del(i)
    del(j)
    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('TIMING_SERVER','INIT',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('TIMING_SERVER','READY',50,None),Method(None,'start_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('TIMING_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
                  'options': ('no_write_shot',)})

    trigModeDict = {'INTERNAL': True, 'EXTERNAL': False}
    tsmpDict = {40000000: 0, 10000000: 1, 5000000: 2, 1000000: 3}
    TPEModeDict = {'DISABLED': 0, 'SLOW': 1, 'FAST': 3}

    session = c_void_p(0)
    niInterfaceLib = None
    fauSession = {}
    workers = {}

    NUM_DIO = c_int(64)

    TICK_40MHz = 40000000.

    def saveInfo(self):
        CRIO_FAU.fauSession[self.nid] = self.session

    def restoreInfo(self):
        try:
            self.session = CRIO_FAU.fauSession[self.nid]
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL

    def initializeInfo(self):
        if CRIO_FAU.niInterfaceLib is None:
            CRIO_FAU.niInterfaceLib = CDLL("libNiInterface.so")

        try:
            fifoDepthSize = self.fifo_depth.data()
        except:
            fifoDepthSize = 30000
        try:
            status = CRIO_FAU.niInterfaceLib.crioFauInit(
                byref(self.session), c_uint(fifoDepthSize))
            if status < 0:
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except BaseException as e:
            print(str(e))
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open cRIO FAU session')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return

    def closeInfo(self):
        try:
            CRIO_FAU.niInterfaceLib.closeFauFpgaSession(
                CRIO_FAU.fauSession[self.nid])
            del(CRIO_FAU.fauSession[self.nid])
            self.session = 0
        except:
            pass
        return
# Worker Management

    def saveWorker(self):
        CRIO_FAU.workers[self.getNid()] = self.worker

    def restoreWorker(self):
        try:
            self.worker = CRIO_FAU.workers[self.nid]
        except:
            print('Cannot restore worker!!')

# AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device, tsmpFreq, treePtr):
            self.device = device
            self.tsmpFreq = tsmpFreq
            self.treePtr = treePtr
            self.stopReq = False
            self.stopFlag = c_byte(0)

        def run(self):

            bufSize = self.device.buf_size.data()

            trigSource = self.device.trig_source.data()
            print('AsynchStore trigget time', trigSource)

            chanNid = []
            for mod in range(1, 9):
                for chan in range(0, 8):
                    chanNid.append(
                        getattr(self.device, 'module_%d_channel_%d_data' % (mod, chan)).getNid())

            chanNid_c = (c_int * len(chanNid))(*chanNid)

            self.stopFlag.value = 0
            ##self.saveList = c_void_p(0) ##
            ##CRIO_FAU.niInterfaceLib.FAUstartSave(byref(self.saveList)) ##

            while not self.stopReq:
                currElem = CRIO_FAU.niInterfaceLib.fauSaveAcqData(self.device.session, c_double(1./self.tsmpFreq), c_double(
                    trigSource), c_int(bufSize),  self.device.NUM_DIO, self.treePtr, chanNid_c, byref(self.stopFlag))
                ##status = CRIO_FAU.niInterfaceLib.fauQueuedAcqData(self.device.session, byref(self.saveList),  c_double(1./self.tsmpFreq), c_double(trigSource), c_int(bufSize),  self.device.NUM_DIO, self.treePtr, chanNid_c, byref(self.stopFlag) );

                if self.stopFlag:
                    self.stopReq = True

            print('AsynchStore stop')

            return

        def stop(self):
            print ('AsynchStore stop Request')
            #self.stopReq = True
            self.stopFlag.value = 1
            ##CRIO_FAU.niInterfaceLib.FAUstopSave(byref(self.saveList)) ##

# End Inner class AsynchStore

    def init(self):

        try:
            self.initializeInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open FAU device')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        # try:
        #    pteMask = self.pte_mask.data();
        # except:
        #    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Read FAU pulse train mask error. Set to 0')
        #    pteMask = 0

        try:
            pteFreqFast = self.pte_frq_fast.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Read FAU pulse train fast frequency error. Set to 50kHz')
            pteFreqFast = 50000

        pteFastCount = int(self.TICK_40MHz / pteFreqFast + 100)

        try:
            pteFreqSlow = self.pte_frq_slow.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Read FAU pulse train slow frequency error. Set to 50Hz')
            pteFreqSlow = 50

        pteSlowCount = int(self.TICK_40MHz / pteFreqSlow + 100)

        try:
            trigMode = self.trig_mode.data()
            print ("Trigger Node = ", trigMode)
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Read FAU trigger mode error. Set to INTERNAL' + str(ex))
            trigMode = 'INTERNAL'
            self.trig_mode.putData(trigMode)

        try:
            tsmpFreq = self.tsmp_freq.data()
            tickFreqCode = self.tsmpDict[tsmpFreq]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Read FAU time stamp frequency error. Set to 40MHz')
            tsmpFreq = 40000000
            tickFreqCode = 0
            self.tsmp_freq.putData(Int32(tsmpFreq))

        try:
            trigSource = self.trig_source.data()
        except:
            if(trigMode == 'EXTERNAL'):
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve Trigger source.')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                trigSource = 0.
                self.trig_source.putData(Float32(trigSource))

        print ("Trigger Source = ", trigSource)

        bit = 0
        pteEnaMask = 0
        pteSlowFastMask = 0
        for mod in range(0, 8):
            for ch in range(0, 8):
                pteModeCode = self.TPEModeDict[getattr(
                    self, 'module_%d_channel_%d_pte_mode' % (mod+1, ch)).data()]
                pteEnaMask = pteEnaMask | ((pteModeCode & 1 == 1) << bit)
                pteSlowFastMask = pteSlowFastMask | (
                    (pteModeCode & 2 == 2) << bit)
                bit = bit + 1

        print ('pteSlowFastMask ', pteSlowFastMask)
        status = CRIO_FAU.niInterfaceLib.setFauAcqParam(self.session, c_uint64(pteEnaMask), c_uint64(
            pteSlowFastMask), c_uint(pteSlowCount), c_uint(pteFastCount), c_short(tickFreqCode))

        if status < 0:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'FAU acquisition device initialization error.')
            raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        status = CRIO_FAU.niInterfaceLib.startFauFpga(self.session)
        if status < 0:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'FAU start FPGA error.')
            raise mdsExceptions.DevCOMM_ERROR

        acqState = c_short()
        CRIO_FAU.niInterfaceLib.getFauAcqState(self.session, byref(acqState))
        print ("Acquisition State ", acqState.value)

        return

    def start_store(self):

        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'FAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            tsmpFreq = self.tsmp_freq.data()
            tickFreqCode = self.tsmpDict[tsmpFreq]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Read FAU time stamp frequency error. Set to 40MHz')
            tsmpFreq = 40000000
            tickFreqCode = 0
            self.tsmp_freq.putData(Int32(tsmpFreq))

        treePtr = c_void_p(0)
        CRIO_FAU.niInterfaceLib.openTree(
            c_char_p(self.getTree().name), c_int(self.getTree().shot), byref(treePtr))

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False

        self.worker.configure(self, tsmpFreq, treePtr)

        """ 
        trigMode = self.trig_mode.data()
        print "Trigg mode ",  trigMode       
        if( trigMode == "INTERNAL" ):
              status = CRIO_FAU.niInterfaceLib.startFauAcquisition(self.session)
              print "Start FAU acquisition"    
              if status < 0:
                  Data.execute('DevLogErr($1,$2)', self.getNid(), 'FAU start acquisition device error.')
                  raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        sleep(1)

        acqState = c_short()
        CRIO_FAU.niInterfaceLib.getFauAcqState(self.session, byref(acqState))
        print ("Acquisition State ", acqState.value)

        self.saveWorker()
        self.worker.start()

        return

    def trigger(self):
        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'FAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        trigMode = self.trig_mode.data()
        print ("Trigg mode ",  trigMode)
        if(trigMode == "INTERNAL"):
            status = CRIO_FAU.niInterfaceLib.startFauAcquisition(self.session)
            print ("Start FAU acquisition")
            if status < 0:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'FAU start acquisition device error.')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        sleep(1)

        acqState = c_short()
        CRIO_FAU.niInterfaceLib.getFauAcqState(self.session, byref(acqState))
        print ("Acquisition State ", acqState.value)

        return

    def stop_store(self):

        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'FAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.restoreWorker()
        if self.worker.isAlive():
            print ("stop_worker")
            self.worker.stop()
            self.worker.join()
            print ("Close Info")
        else:
            print ("Worker isn't running")

        self.closeInfo()
        return
