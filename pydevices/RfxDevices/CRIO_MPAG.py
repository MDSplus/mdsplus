from MDSplus import mdsExceptions, Device, Data, Range, Dimension, Window, Int32, Float32, Float64, Tree
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, byref, c_int, c_void_p, c_byte, c_float, c_char_p, c_uint, c_short, c_byte, c_double, c_uint8, c_int32, c_uint16
import os
from time import sleep
import sys, traceback
import datetime

class CRIO_MPAG(Device):
    """NI Compact RIO MITICA AGPS analogue signals"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},

        {'path':':FIFO_DEPTH', 'type':'numeric', 'value':30000},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':10000},
        {'path':':SEG_LENGTH', 'type':'numeric', 'value':10000},

        {'path':':CLOCK_MODE', 'type':'text', 'value':'INTERNAL'},
        {'path':':CLOCK_FREQ', 'type':'numeric', 'value':1000},
        {'path':':CLOCK_SOURCE', 'type':'numeric'},

        {'path':':START_IDX', 'type':'numeric', 'value':0},
        {'path':':END_IDX', 'type':'numeric'},
        {'path':':START_TIME', 'type':'numeric','value':0},
        {'path':':END_TIME', 'type':'numeric','value':1},
        {'path':':USE_TIME', 'type':'text', 'value':'YES'},

        {'path':':TRIG_MODE', 'type':'text', 'value':"INTERNAL"},
        {'path':':TRIG_SOURCE', 'type':'numeric'}]


    for slave in ['A','B','C']:
        parts.append({'path':'.SLAVE_CRIO_%s'%(slave), 'type':'structure'})
        for i in range(1,19):
            parts.append({'path':'.SLAVE_CRIO_%s:CHANNEL_%02d'%(slave, i), 'type':'structure'})
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:LABEL'%(slave, i), 'type':'text'  })
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:COMMENT'%(slave, i), 'type':'text'  })
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:DATA'%(slave, i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put') , 'valueExpr': 'Data.compile("($1 + 10./32767. * $2 * $3)", head.slave_crio_%s_channel_%02d_offset, head.slave_crio_%s_channel_%02d_gain, head.slave_crio_%s_channel_%02d_data_raw)'%(slave.lower(),i,slave.lower(),i,slave.lower(),i)})
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:DATA_RAW'%(slave,i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:RES_RAW'%(slave,i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:GAIN'%(slave,i), 'type':'numeric'  })
            parts.append({'path':'.SLAVE_CRIO_%s.CHANNEL_%02d:OFFSET'%(slave,i), 'type':'numeric'  })

    for adcIdx in ['1','2']:
        adcPath = '.NI_9220_%s'%(adcIdx)
        parts.append({'path':'.NI_9220_%s'%(adcIdx), 'type':'structure'})
        for i in range(1,17):
            parts.append({'path':'%s:CHANNEL_%02d'%(adcPath, i), 'type':'structure'})
            parts.append({'path':'%s.CHANNEL_%02d:LABEL'%(adcPath, i), 'type':'text'  })
            parts.append({'path':'%s.CHANNEL_%02d:COMMENT'%(adcPath, i), 'type':'text'  })
            parts.append({'path':'%s.CHANNEL_%02d:DATA'%(adcPath, i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put') , 'valueExpr': 'Data.compile("($1 + 10./32767. * $2 * $3)", head.ni_9220_%s_channel_%02d_offset, head.ni_9220_%s_channel_%02d_gain, head.ni_9220_%s_channel_%02d_data_raw)'%(adcIdx, i, adcIdx,i, adcIdx, i)})
            parts.append({'path':'%s.CHANNEL_%02d:DATA_RAW'%(adcPath,i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
            parts.append({'path':'%s.CHANNEL_%02d:RES_RAW'%(adcPath,i), 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  })
            parts.append({'path':'%s.CHANNEL_%02d:GAIN'%(adcPath,i), 'type':'numeric'  })
            parts.append({'path':'%s.CHANNEL_%02d:OFFSET'%(adcPath,i), 'type':'numeric'  })


    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','READY',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':WAIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('TIMING_SERVER','POST_PULSE_CHECK',50,None),Method(None,'wait_store',head))",
        'options':('no_write_shot',)})


    clockModeDict = {'INTERNAL':c_uint8(1) , 'EXTERNAL':c_uint8(0)}


    session = c_void_p(0)
    niInterfaceLib = None
    mpagSession = {}
    workers = {}
    NUM_SLAVE_CHANNEL = 18
    NUM_SLAVE = 3
    NUM_9220_CHANNEL = 16
    NUM_9220_DEVICE = 2

    TICK_40MHz = 40000000.

    DEV_IS_OPEN = 1
    DEV_NOT_OPEN = 2

    STOP_PLC_COMMAND  = c_int(0)
    START_PLC_COMMAND = c_int(1)


    MAX_ACQ_FREQUENCY = 30000 #Max acquisition frequency 30kHz


    def debugPrint(self, msg="", obj="", logErrno = False):
          msger=""
          if logErrno and CRIO_MPAG.niInterfaceLib is not None:
              errno = CRIO_MPAG.niInterfaceLib.getErrno();
              print ("errno ", errno)
              if errno is not None:
                  msger = 'Error (%d) %s' % (errno, os.strerror( errno ))
          print( self.name + ":" + msg, obj, msger );


    def saveInfo(self):
        CRIO_MPAG.mpagSession[self.nid] = self.session

    def restoreInfo(self):
         try:
            self.session = CRIO_MPAG.mpagSession[self.nid]
            return self.DEV_IS_OPEN
         except:
            return self.DEV_NOT_OPEN
    
 
    def initializeInfo(self, boardId, fifoDepthSize):
        if CRIO_MPAG.niInterfaceLib is None:
           CRIO_MPAG.niInterfaceLib = CDLL("libNiInterface.so")

        try:
            status = CRIO_MPAG.niInterfaceLib.crioMpagInit( byref(self.session), c_char_p(boardId.encode('utf-8')), c_uint(fifoDepthSize) )
            if status < 0 :
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except BaseException as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open cRIO MPAG session : %s'%(str(e)))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return

    def closeInfo(self):
        try:
            CRIO_MPAG.niInterfaceLib.closeMpagFpgaSession(CRIO_MPAG.mpagSession[self.nid])
            del(CRIO_MPAG.mpagSession[self.nid])
        except:
            pass
        return
################################### Worker Management
    def saveWorker(self):
      CRIO_MPAG.workers[self.getNid()] = self.worker

    def restoreWorker(self):
      try:
        self.worker = CRIO_MPAG.workers[self.nid]
        return True
      except:
        print('Cannot restore worker!!')
        return False

########################AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device, treePtr):
            self.device = device
            self.treePtr = treePtr
            self.stopAcq = c_void_p(0)

        def run(self):

            self.device.setTree(
                Tree(self.device.getTree().name, self.device.getTree().shot))
            self.device = self.device.copy()

            bufSize = self.device.buf_size.data()
            segmentSize = self.device.seg_length.data()
            trigSource = self.device.trig_source.data() 
            startTime = float( self.device.start_time.data() )
            clockSource = self.device.clock_source.evaluate()
            period = float( clockSource.getDelta() )
            timeAt0 = startTime
            numSamples = -1 # Forced to continuous acquisition
            clockMode = self.device.clockModeDict[self.device.clock_mode.data()]

            print ('startSave')
            saveList = c_void_p(0)
            CRIO_MPAG.niInterfaceLib.startSave(byref(saveList))

            print ('getStopAcqFlag')
            CRIO_MPAG.niInterfaceLib.getStopAcqFlag(byref(self.stopAcq))
 
          
            #print 'setStopAcqFlag'
            #self.stopAcq.value = 0;        
            #CRIO_MPAG.niInterfaceLib.setStopAcqFlag(self.stopAcq)
            #print 'setStopAcqFlag'

            chanState = []
            chanNid = []
            resNid = []
            numChan = 0;
            for slave in ['a','b','c']:
                for chan in range(1, self.device.NUM_SLAVE_CHANNEL+1):
                    #print '--- slave_crio_%c_channel_%02d_data_raw'%(slave, chan)                   
                    dataRawNode = getattr(self.device, 'slave_crio_%c_channel_%02d_data_raw'%(slave, chan))
                    dataResNode = getattr(self.device, 'slave_crio_%c_channel_%02d_res_raw'%(slave, chan))  
                    if getattr(self.device, 'slave_crio_%c_channel_%02d'%(slave, chan)).isOn() :
                        chanState.append(1)
                    else :
                        chanState.append(0)
                    chanNid.append(dataRawNode.getNid())
                    resNid.append(dataResNode.getNid())
                    numChan = numChan + 1
             
            for adcIdx in ['1','2']:
                for chan in range(1, self.device.NUM_9220_CHANNEL+1):
                    #print '--- ni_9220_%c_channel_%02d_data_raw'%(slave, chan)                   
                    dataRawNode = getattr(self.device, 'ni_9220_%c_channel_%02d_data_raw'%(adcIdx, chan))
                    dataResNode = getattr(self.device, 'ni_9220_%c_channel_%02d_res_raw'%(adcIdx, chan))  
                    if getattr(self.device, 'ni_9220_%c_channel_%02d'%(adcIdx, chan)).isOn() :
                        chanState.append(1)
                    else :
                        chanState.append(0)
                    chanNid.append(dataRawNode.getNid())
                    resNid.append(dataResNode.getNid())
                    numChan = numChan + 1
               

            chanNid_c      = (c_int * len(chanNid) )(*chanNid)
            resNid_c       = (c_int * len(resNid))(*resNid)
            chanState_c    = (c_int * len(chanState))(*chanState)

            print (self.device.session)
            print ("numChan ",numChan)
            #print "chanState_c ",chanState_c
            print ("bufSize ",bufSize)
            print ("segmentSize ",segmentSize )
            print ("numSamples ",numSamples)
            #print "chanNid_c ",chanNid_c
            print ("clock_source mid ",self.device.clock_source.getNid())
            print ("timeAt0 ",timeAt0)
            print ("period ",period)
            print ("tree ptr ",self.treePtr)
            print ("saveList ",saveList)
            print ("stopAcq ",self.stopAcq)
            print ("shot ",self.device.getTree().shot)
            #print "resNid_c ",resNid_c


	    #status = CRIO_MPAG.niInterfaceLib.startMpagFpga(self.device.session)

              
            status =  CRIO_MPAG.niInterfaceLib.mpag_readAndSaveAllChannels(self.device.session, c_int(numChan), chanState_c,c_int(bufSize), c_int(segmentSize), c_int(numSamples), chanNid_c, clockMode, self.device.clock_source.getNid(), c_float( timeAt0 ), c_float(period),self.treePtr, saveList, (self.stopAcq), c_int(self.device.getTree().shot), resNid_c)

            self.device.debugPrint('ASYNCH WORKER TERMINATED')
            CRIO_MPAG.niInterfaceLib.stopSave(saveList)
            CRIO_MPAG.niInterfaceLib.freeStopAcqFlag(self.stopAcq)
            
            return

        def stop(self):
            print (self.stopAcq)       
            CRIO_MPAG.niInterfaceLib.setStopAcqFlag(self.stopAcq)

      
#############End Inner class AsynchStore

    def init(self):

        try:
            fifoDepthSize = self.fifo_depth.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Fifo depth undefined, value set to defaul 40000')
            fifoDepthSize = 40000
            self.fifo_depth.putData(Int32(fifoDepthSize))

        try:
            boardId = self.board_id.data();
            boardIdDev='RIO%d'%(int(boardId))  
            print ('-------->'+boardIdDev)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Board Identifier undefined')
            raise DevBAD_PARAMETER

        try:

            if self.restoreInfo() == self.DEV_IS_OPEN :
               if self.restoreWorker():
                  if self.worker.isAlive():
                      print ('stop Store')
                      self.stop_store()
                      self.wait_store()
               self.closeInfo()
               sleep(1)

            self.initializeInfo(boardIdDev, fifoDepthSize)
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open MPAG device : %s'%(str(ex)))
            raise mdsExceptions.TclFAILED_ESSENTIAL


        self.saveInfo()

        try:
            bufferSize = self.buf_size.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition Buffer size undefined, value set to defaul 1000')
            bufferSize = 1000
            self.buf_size.putData(Int32(bufferSize))


        try:
            segmentLength = self.seg_length.data();
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition Segment length undefined, value set to defaul 10000')
            segmentLength = 10000
            self.seg_size.putData(Int32(segmentLength))



#clock mode
        try:
            clockMode = self.clock_mode.data()
            print ('ClockMode -------->'+clockMode)
            if clockMode == 'INTERNAL' :
                frequency = self.clock_freq.data()
                print (frequency)
                if( frequency > self.MAX_ACQ_FREQUENCY ):
                    frequency = self.MAX_ACQ_FREQUENCY
                    self.debugPrint('cRIO MPAG Frequency out of limits. Set to max frequency value : %f' %(self.MAX_ACQ_FREQUENCY))
                    self.clock_freq.putData(frequency)

                clockSource = Range(None, None, Float64(1./frequency))
                print (clockSource)
                self.debugPrint('cRIO MPAG CLOCK: ', clockSource)
                self.clock_source.putData(clockSource)
            else:
                clockSource = self.clock_source.evaluate()
                print (clockSource)
                frequency = self.clock_freq.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid clock definition : '+str(ex))
            raise DevBAD_PARAMETER

#trigger source
        try:
            trigMode = self.trig_mode.data()
            print ('Trig --------> '+trigMode)
            if trigMode == 'EXTERNAL':
                trigSource = self.trig_source.data()
            else:
                try:
                    trigSource = self.trig_source.data()
                except:
                    trigSource = 0;
                    self.trig_source.putData(Float32(trigSource))
            self.debugPrint('cRIO MPAG Trigger source: ',trigSource)
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve Trigger source : '+str(ex))
            raise DevBAD_PARAMETER


#Time management
        """
        try:
            acqMode = self.acq_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition Mode undefined')
            raise DevBAD_PARAMETER
        """
        acqMode = 'CONTINUOUS'
        if acqMode == 'TRANSIENT REC.':
            try:
                useTime = self.use_time.data()
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot resolve time or samples management')
                raise DevBAD_PARAMETER

            if useTime == 'YES':
                try:
                    startTime = self.start_time.data()
                    endTime   = self.end_time.data()

                    self.debugPrint('cRIO MPAG startTime    = ', startTime)
                    self.debugPrint('cRIO MPAG endTime      = ', endTime)
                    self.debugPrint('cRIO MPAG trigSource   = ', trigSource)
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot Read Start or End time')
                    raise DevBAD_PARAMETER

                startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), startTime)
                endIdx   = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), endTime)


                self.debugPrint('cRIO MPAG  startIdx = ', Int32(int(startIdx + 0.5)))
                self.start_idx.putData(Int32(int(startIdx + 0.5)))

                self.debugPrint('cRIO MPAG  endIdx   = ', Int32(int(endIdx + 0.5)))
                self.end_idx.putData(Int32(int(endIdx + 0.5)))

            else:
                endIdx   = self.end_idx.data()
                startIdx = self.start_idx.data()

            #Solo acquisizione continua per il momento
            nSamples = -1

        else: #Continuous Acquisition
            nSamples = -1
 

#Channel configuration
        #activeChan = 0
        #chanState = []
        #chanNid = []
        #resNid = []

        mapToAO_Array =c_int32*16
        mapToAO = mapToAO_Array()

        for slave in ['a','b','c']:
            for chan in range(1, self.NUM_SLAVE_CHANNEL+1):                   
                #Empty the node which will contain  the segmented data   
                dataRawNode = getattr(self, 'slave_crio_%c_channel_%02d_data_raw'%(slave, chan))
                dataRawNode.deleteData()
                dataRawNode.setCompressOnPut(False)
                dataResNode = getattr(self, 'slave_crio_%c_channel_%02d_res_raw'%(slave, chan))  
                dataResNode.deleteData()
                dataResNode.setCompressOnPut(False)

        for adcIdx in ['1','2']:
            for chan in range(1, self.NUM_9220_CHANNEL+1):                   
                #Empty the node which will contain  the segmented data   
                dataRawNode = getattr(self, 'ni_9220_%c_channel_%02d_data_raw'%(adcIdx, chan))
                dataRawNode.deleteData()
                dataRawNode.setCompressOnPut(False)
                dataResNode = getattr(self, 'ni_9220_%c_channel_%02d_res_raw'%(adcIdx, chan))  
                dataResNode.deleteData()
                dataResNode.setCompressOnPut(False)


        #Da definire in base alla frequenza di acquisizione impostata

        #highStrobeTick = 50 
        highStrobeTick = int(1e6/frequency)*6 

        status = CRIO_MPAG.niInterfaceLib.setMpagAcqParam(self.session, c_float(frequency), c_uint16(highStrobeTick), mapToAO) 
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG acquisition device initialization error.')
            return 0

        """
        status = CRIO_MPAG.niInterfaceLib.startMpagFpga(self.session)
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG start FPGA error.')
            return 0
        """

        return 1

    def start_store(self):

        
        if self.restoreInfo() == self.DEV_NOT_OPEN :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL
 

        treePtr = c_void_p(0)
        CRIO_MPAG.niInterfaceLib.openTree(c_char_p((self.getTree().name).encode('utf-8')), c_int(self.getTree().shot), byref(treePtr))
 
        self.worker = self.AsynchStore()        
        self.worker.daemon = True 
        self.worker.stopReq = False


        self.worker.configure(self.copy(), treePtr)

        self.saveWorker()
        self.worker.start()

        """
        status = CRIO_MPAG.niInterfaceLib.startMpagFpga(self.session)
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG start FPGA error.')
            return 0
        """

 
        return 1


    def stop_store(self):

        if self.restoreInfo() == self.DEV_NOT_OPEN :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if self.restoreWorker() :
            if self.worker.isAlive():
                print ("stop_worker")
                self.worker.stop()
        return 1


    def wait_store(self):

        if self.restoreInfo() == self.DEV_NOT_OPEN :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG device not initialized')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if self.restoreWorker() :
            if self.worker.isAlive():
                print ("stop_worker")
                self.worker.stop()
                self.worker.join()
        
        print ("Close Info")
        self.closeInfo()
        return 1


    def start_cmd(self):

        try:
            boardId = self.board_id.data();
            boardIdDev='RIO%d'%(int(boardId))  
            print ('-------->'+boardIdDev)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Board Identifier undefined')
            raise DevBAD_PARAMETER

        niLib = CDLL("libNiInterface.so")

        status = niLib.executeMpagFpgaCommand( c_char_p(boardIdDev.encode('utf-8')), CRIO_MPAG.START_PLC_COMMAND )
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG start command error.')
            return 0
       
        return 1


    def stop_cmd(self):

        try:
            boardId = self.board_id.data();
            boardIdDev='RIO%d'%(int(boardId))  
            print ('-------->'+boardIdDev)
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Board Identifier undefined')
            raise DevBAD_PARAMETER

        niLib = CDLL("libNiInterface.so")

        status = niLib.executeMpagFpgaCommand( c_char_p(boardIdDev.encode('utf-8')), CRIO_MPAG.STOP_PLC_COMMAND )
        if status < 0 :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'cRIO MPAG stop command error.')
            return 0
       
        return 1

    
