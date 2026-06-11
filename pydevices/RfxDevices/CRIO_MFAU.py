from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Tree
from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p, c_uint, c_short, c_double, c_uint64, c_uint8, c_uint16, c_uint32
import os
from time import sleep
import sys, traceback

class CRIO_MFAU(Device):
    """NI Compact RIO MITICA Interlock Fast Acquisition Units"""
    parts=[{'path':':COMMENT', 'type':'text'},
        {'path':':FIFO_DEPTH', 'type':'numeric', 'value':30000},
        {'path':':TSMP_FREQ', 'type':'numeric', 'value':1},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':10000},
        {'path':':PTP_SYNC', 'type':'text', 'value':"ENABLED"},
        {'path':':EN_ACQ_MODE', 'type':'text', 'value':"EXTERNAL"},
        {'path':':CLOCK_SOURCE', 'type':'numeric'}]
    for i in range(1,5): #OPIF 1-4
        parts.append({'path':'.OPIF_%d'%(i), 'type':'structure'})
        for j in range(1,4): #RX 1-3
           parts.append({'path':'.OPIF_%d.RX_%d'%(i,j), 'type':'structure'})
           for k in range(1,10): #CHAN 1-9        
             parts.append({'path':'.OPIF_%d.RX_%d.CHANNEL_%d'%(i,j,k), 'type':'structure' })
             parts.append({'path':'.OPIF_%d.RX_%d.CHANNEL_%d:DESCRIPTION'%(i,j,k), 'type':'text'  })
             parts.append({'path':'.OPIF_%d.RX_%d.CHANNEL_%d:DATA'%(i,j,k), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
           parts.append({'path':'.OPIF_%d.RX_%d.PARITY'%(i,j), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })

    del(i)
    del(j)
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('INTERLOCK_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('INTERLOCK_SERVER','READY',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('INTERLOCK_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})

    PtpSyncDict = {'ENABLED':1 , 'DISABLED':0}
    EnAcqModeDict = {'INTERNAL':0 , 'EXTERNAL':1}
    tsmpDict = {40000000:0 , 10000000:1, 5000000:2 , 1000000:3}
    #TPEModeDict = {'DISABLED':0 , 'SLOW':1, 'FAST':3}

    session = c_void_p(0)
    niInterfaceLib = None
    fauSession = {}
    workers = {}

    #NUM_DIO = c_int(60)  #spider=64
    NUM_DIO_FLS = c_int(60)  
    NUM_DIO_OIU = c_int(60)  
    NUM_DIO_TOTAL = c_int(120)  

    TICK_40MHz = 40000000.

    def saveInfo(self):
        CRIO_MFAU.fauSession[self.nid] = self.session

    def restoreInfo(self):
         try:
            self.session = CRIO_MFAU.fauSession[self.nid]
         except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
 
    def initializeInfo(self):
        if CRIO_MFAU.niInterfaceLib is None:
           CRIO_MFAU.niInterfaceLib = CDLL("libNiInterface.so")

        try:
            fifoDepthSize = self.fifo_depth.data();
        except:
            fifoDepthSize = 30000
        try:
            status = CRIO_MFAU.niInterfaceLib.crioFauMiticaInit( byref(self.session), c_uint(fifoDepthSize) )
            if status < 0 :
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except BaseException as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open cRIO FAU session : '+str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return

    def closeInfo(self):
        try:
            CRIO_MFAU.niInterfaceLib.closeFauMiticaFpgaSession(CRIO_MFAU.fauSession[self.nid])
            del(CRIO_MFAU.fauSession[self.nid])
            self.session = 0
        except:
            pass
        return
################################### Worker Management
    def saveWorker(self):
      CRIO_MFAU.workers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        self.worker = CRIO_MFAU.workers[self.nid]
      except:
        print('Cannot restore worker!!')

########################AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device, tsmpFreq, treePtr):
            self.device = device
            self.tsmpFreq = tsmpFreq
            self.treePtr = treePtr
            self.stopReq = False
            ##self.stopFlag = c_byte(0);
            

        def run(self):

            self.device.setTree(Tree(self.device.getTree().name, self.device.getTree().shot))
            self.device = self.device.copy()

            bufSize = self.device.buf_size.data()

            clockSource = self.device.clock_source.data() 
            print ('AsynchStore clock time', clockSource)

                      
            chanNid = []
            for opif in range(1,5):  #1-2 FLS ; 3-4 OIU
               for rx in range(1,4):
                   for chan in range(1,10):
                      chanNid.append(getattr(self.device, 'opif_%d_rx_%d_channel_%d_data'%(opif,rx,chan)).getNid())
                   chanNid.append(getattr(self.device, 'opif_%d_rx_%d_parity'%(opif,rx)).getNid())
            
            chanNid_c = (c_int * len(chanNid) )(*chanNid)



            ##self.stopFlag.value = 0;
            self.stopFlag = c_void_p(0)
            CRIO_MFAU.niInterfaceLib.getStopAcqFlag(byref(self.stopFlag))   
        
            self.saveList = c_void_p(0) ##
            CRIO_MFAU.niInterfaceLib.FAU_MiticaStartSave(byref(self.saveList)) 

            """
            while not self.stopReq:
               print ('AsynchStore LOOP')
               ##currElem = CRIO_MFAU.niInterfaceLib.fauSaveAcqData(self.device.session, c_double(1./self.tsmpFreq), c_double(trigSource), c_int(bufSize),  self.device.NUM_DIO, self.treePtr, chanNid_c, byref(self.stopFlag) );
            """     
#            status = CRIO_MFAU.niInterfaceLib.fauMiticaQueuedAcqData(self.device.session, (self.saveList),  c_double(1./self.tsmpFreq), c_double(clockSource), c_int(bufSize),  self.device.NUM_DIO, self.treePtr, chanNid_c, (self.stopFlag) );
            status = CRIO_MFAU.niInterfaceLib.fauMiticaQueuedAcqData(self.device.session, (self.saveList),  c_double(1./self.tsmpFreq), c_double(clockSource), c_int(bufSize),  self.device.NUM_DIO_FLS, self.device.NUM_DIO_OIU, self.treePtr, chanNid_c, (self.stopFlag) );           
               #if self.stopFlag.value == 1:
               #  self.stopReq = True           

            print ('AsynchStore stop LOOP')
            CRIO_MFAU.niInterfaceLib.FAU_MiticaStopSave(self.saveList) ##
            CRIO_MFAU.niInterfaceLib.freeStopAcqFlag(self.stopFlag) 
 
            CRIO_MFAU.niInterfaceLib.setFauMiticaStopSCTML(self.device.session, c_byte(1))

            for i in range(10) :
              acqState = c_short();
              CRIO_MFAU.niInterfaceLib.getFauMiticaAcqState(self.device.session, byref(acqState))
              sleep(0.05)
              print ("-- Init Acquisition State ", acqState.value)


        def stop(self):
            print ('AsynchStore stop Request')
            self.stopReq = True
            CRIO_MFAU.niInterfaceLib.setStopAcqFlag(self.stopFlag)
            #self.stopFlag.value = 1

      
#############End Inner class AsynchStore

    def init(self):

        try:
            self.initializeInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open FAU device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
             
        self.saveInfo()

        try:
            ptpSync = self.ptp_sync.data()
            print ("PTP Sync is ", enAcqMode)
            if(ptpSync!="ENABLED" and ptpSync!="DISABLED"):
               print ("PTP Sync selected is not allowed.")
               raise mdsExceptions.TclFAILED_ESSENTIAL
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Read Mitica FAU ptp sync enable error. Set to DISABLED' + str(ex))
            ptpSync = 'DISABLED'
            self.ptp_sync.putData( ptpSync )

        try:
            enAcqMode = self.en_acq_mode.data()
            print ("Enable Acquisition Mode = ", enAcqMode)
            if(enAcqMode!="INTERNAL" and enAcqMode!="EXTERNAL"):
               print ("Enable Acquisition Mode is not allowed.")
               raise mdsExceptions.TclFAILED_ESSENTIAL
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Read Mitica FAU enable acquisition mode error. Set to INTERNAL' + str(ex))
            enAcqMode = 'INTERNAL'
            self.en_acq_mode.putData( enAcqMode )
 
        try:
            tsmpFreq = self.tsmp_freq.data()
            tickFreqCode = self.tsmpDict[tsmpFreq]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Read FAU time stamp frequency error. Set to 40MHz')
            tsmpFreq = 40000000
            tickFreqCode = 0
            self.tsmp_freq.putData( Int32(tsmpFreq) )
    
        try:
            clockSource = self.clock_source.data() 
        except:
            if(clockMode == 'EXTERNAL'):
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Clock source.')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                clockSource = 0.
                self.clock_source.putData( Float32(clockSource) )

        print ("Clock Source = ", clockSource)

#
#        bit=0
#        pteEnaMask      = int(0)
#        pteSlowFastMask = int(0)
#        for mod in range(0,8):
#           for ch in range(0,8):
#               pteModeCode = self.TPEModeDict[getattr(self, 'module_%d_channel_%d_pte_mode'%(mod+1,ch)).data()]
#               pteEnaMask      = pteEnaMask      | ((pteModeCode & int(1) == 1) << bit)
#               pteSlowFastMask = pteSlowFastMask | ((pteModeCode & int(2) == 2) << bit)
#               bit = bit + 1
 
#        print ('pteSlowFastMask ', pteSlowFastMask)
#        status = CRIO_MFAU.niInterfaceLib.setFauAcqParam(self.session, c_uint64(pteEnaMask), c_uint64(pteSlowFastMask), c_uint(pteSlowCount), c_uint(pteFastCount), c_short(tickFreqCode) )

#        if status < 0 :
#            Data.execute('DevLogErr($1,$2)', self.getNid(), 'FAU acquisition device initialization error.')
#            return 0


        status = CRIO_MFAU.niInterfaceLib.setFauMiticaAcqParam(self.session, c_byte(EnAcqModeDict[enAcqMode]), c_byte(PtpSyncDict[ptpSync]) )
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU setFauAcqParam error.')
            return 0


        status = CRIO_MFAU.niInterfaceLib.startFauMiticaFpga(self.session)
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU start FPGA error.')
            return 0

        for i in range(10) :
          acqState = c_short();
          CRIO_MFAU.niInterfaceLib.getFauMiticaAcqState(self.session, byref(acqState))
          sleep(0.05)
          print ("-- Init Acquisition State ", acqState.value)

        return 1




    def start_store(self):

        try:
            self.restoreInfo() 
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            tsmpFreq = self.tsmp_freq.data()
            #tickFreqCode = self.tsmpDict[tsmpFreq]
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Read MFAU time stamp frequency error. Set to 40MHz')
            tsmpFreq = 40000000
            #tickFreqCode = 0
            self.tsmp_freq.putData( Int32(tsmpFreq) )
 

        treePtr = c_void_p(0)
        CRIO_MFAU.niInterfaceLib.openTree(c_char_p(self.getTree().name.encode('utf-8')), c_int(self.getTree().shot), byref(treePtr))
 
        self.worker = self.AsynchStore()        
        self.worker.daemon = True 
        self.worker.stopReq = False

        self.worker.configure(self.copy(), tsmpFreq, treePtr)

        """ 
        trigMode = self.trig_mode.data()
        print ("Trigg mode ",  trigMode )      
        if( trigMode == "INTERNAL" ):
              status = CRIO_MFAU.niInterfaceLib.startFauAcquisition(self.session)
              print ("Start FAU acquisition")    
              if status < 0:
                  Data.execute('DevLogErr($1,$2)', self.getNid(), 'FAU start acquisition device error.')
                  raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        sleep(1)
         
        acqState = c_short();
        CRIO_MFAU.niInterfaceLib.getFauMiticaAcqState(self.session, byref(acqState))
        print ("Start Store Acquisition State ", acqState.value)

        self.saveWorker()
        self.worker.start()
 

        return 1

#if en_acq_mode is INTERNAL: acquisition is started asserting AcqEna to 1 via the startFauMiticaAcquisition function
#if en_acq_mode is EXTERNAL: acquisition is started via PXI 6683H on PFI0
    def trigger(self):       
        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        enAcqMode = self.en_acq_mode.data()
        print ("Enable Acquisition Mode ",  enAcqMode)           
        if( enAcqMode == "INTERNAL" ):
           status = CRIO_MFAU.niInterfaceLib.startFauMiticaAcquisition(self.session)
           print ("Start Mitica FAU acquisition")    
           if status < 0:
               Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU start acquisition device error.')
               raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
           print ("CANNOT issue start acquisition trigger. Enable Acquisition Mode is set to EXTERNAL.") 

        acqState = c_short();
        CRIO_MFAU.niInterfaceLib.getFauMiticaAcqState(self.session, byref(acqState))
        print ("Acquisition State: ", acqState.value)

        return 1


    def stop_store(self):

        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'FAU device not initialized')
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
        return 1


    def setTestOutputSignals(self):
        try:
            self.restoreInfo()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        tx1_mode=2 #ON, OFF, FREQ #self.tx1_mode.data()
        tx1_tickCnt=40000    #self.tx1_tick.data()
        tx1_and_rx1=0        #self.tx1_and.data()

        status = CRIO_MFAU.niInterfaceLib.setFauMiticaTestTX1(self.session, c_uint16(tx1_mode), c_uint32(tx1_tickCnt), c_uint8(tx1_and_rx1))
        print ("Start Mitica FAU acquisition")    
        if status < 0:
           Data.execute('DevLogErr($1,$2)', self.getNid(), 'MFAU start acquisition device error.')
           raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1
   
