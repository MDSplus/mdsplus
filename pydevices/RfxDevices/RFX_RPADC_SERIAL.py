
from MDSplus import mdsExceptions, Device, Data, Tree, Range, Float32, Int32Array, Int32
from threading import Thread
try:
    import serial
except:
    pass
import time
import numpy as np

modeDict = {'STREAMING': 0, 'EVENT_STREAMING': 1,
                'EVENT_SINGLE': 2, 'TRIGGER_STREAMING': 3, 'TRIGGER_SINGLE': 4}

clockModeDict = {'INTERNAL': 0, 'TRIG_EXTERNAL': 1, 'TRIG_SYNC': 2, 
                    'EXTERNAL': 3, 'SYNC': 4}



class RFX_RPADC_SERIAL(Device):
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':DECIMATION', 'type': 'numeric', 'value': 125000},
             {'path': ':SEG_SIZE', 'type': 'numeric', 'value': 100000},
             {'path': ':TRIGGER', 'type': 'numeric'},
             # STREAMING or EVENT_STREAMING or TRIGGER_STREAMING or TRIGGER_SINGLE
             {'path': ':MODE', 'type': 'text', 'value': 'STREAMING'},
             # event generation in respect of EV_LEVEL: UPPER or LOWER
             {'path': ':EV_MODE', 'type': 'text', 'value': 'UPPER'},
             # channel (A or B) used for event detection
             {'path': ':EV_CHANNEL', 'type': 'text', 'value': 'A'},
             {'path': ':EV_LEVEL', 'type': 'numeric', 'value': 0},  # event generation level
             {'path': ':EV_SAMPLES', 'type': 'numeric',
              'value': 2},  # event validation samples
             {'path': ':PRE_SAMPLES', 'type': 'numeric',
              'value': 100},  # pre trigger samples
             {'path': ':POST_SAMPLES', 'type': 'numeric',
              'value': 100},  # post trigger samples
             {'path': ':CLOCK_MODE', 'type': 'text',
                 'value': 'INTERNAL'},  # Clock mode
             {'path': ':EXT_CLOCK', 'type': 'numeric'},  # Ext. Clock
             {'path': ':OFFSET_A', 'type': 'numeric', 'value': 0.},
             {'path': ':OFFSET_B', 'type': 'numeric', 'value': 0.},
             {'path': ':RAW_A', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':RAW_B', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':CHAN_A', 'type': 'signal'},
             {'path': ':CHAN_B', 'type': 'signal'},
             {'path': ':INIT_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
              'options': ('no_write_shot',)},
             {'path': ':START_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
              'options': ('no_write_shot',)},
             {'path': ':STOP_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
              'options': ('no_write_shot',)},
             {'path': ':START_TIME', 'type': 'numeric', 'value': 0},
             {'path': ':DEAD_TIME', 'type': 'numeric', 'value': 1E-3},
             {'path': ':TRIG_RECV', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':TEMP_DELAY', 'type': 'numeric', 'value': -1},
             {'path': ':TEMPERATURE', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':GAIN_A', 'type': 'numeric', 'value': 1.},
             {'path': ':GAIN_B', 'type': 'numeric', 'value': 1.},
             {'path': ':RANGE_A', 'type': 'numeric', 'value': 1.},  # 1/20 Vpp
             {'path': ':RANGE_B', 'type': 'numeric', 'value': 1.},
             {'path': ':HW_OFFS_A', 'type': 'numeric', 'value': 0},
             {'path': ':HW_OFFS_B', 'type': 'numeric', 'value': 0},
             ]

    port = None
    rfx_rpadc_serialWorkers = {}
    stopAcq = False

    ####### Start Inner class AsynchStore ######    
    class AsynchStore(Thread):
        ACQ_NOERROR = 0
        ACQ_ERROR = 1
        trig_times = []
        numTrigs = 0
        
        # retrieving the useful information for a correct acquisition
        def configure(self, device):
            self.error = self.ACQ_NOERROR
            self.device = device
            self.mode = self.device.mode.data()
            self.nodeA = self.device.raw_a
            self.nodeB= self.device.raw_b
            self.nodeTemp = self.device.temperature
            self.nodeTrigRecv = self.device.trig_recv
            print('CONFIGURE!!!!!!!!!!!!!!! ' , self.nodeTrigRecv, device)
            self.startTime = self.device.start_time.data()
            self.decimation = self.device.decimation.data()
            self.pre_samples = self.device.pre_samples.data()
            self.post_samples = self.device.post_samples.data()
            self.segSize = int( self.pre_samples + self.post_samples)
            self.freq = 125e6/self.decimation
            self.data_A = []
            self.data_B = []
            self.temps = []


        def run(self):
            # setting the current opened tree with the corresponding shot
            self.device.setTree(Tree(self.device.getTree().name, self.device.getTree().shot))
            print ("SHOT: " + str(self.device.getTree().shot))
            self.device = self.device.copy()
            # if self.mode == "STREAMING" or self.mode == "EVENT_STREAMING":
                # trigger_cmd = "trigger\n"
                # RFX_RPADC_SERIAL.port.write(trigger_cmd.encode())
            
            trigger_cmd = "trigger\n"
            RFX_RPADC_SERIAL.port.write(trigger_cmd.encode())

            POSIX_TIME_AT_EPICS_EPOCH = 631152000000
            period = 1./self.freq
            last_temp_time = 0
            # line = None

            # print ("STOP ACQ: " + str(RFX_RPADC_SERIAL.stopAcq))

            # Main cycle
            while not RFX_RPADC_SERIAL.stopAcq:

                read = True
                
                # sending Temperature command if the temperature delay is passed
                tempDelay = self.device.temp_delay.data()
                currTime = time.time()
                # print (self.trig_times)
                if currTime - last_temp_time > tempDelay:
                    last_temp_time = currTime
                    print ("SENDING T")
                    Tcmd = "T\n"
                    RFX_RPADC_SERIAL.port.write(Tcmd.encode())

                # reading data from serial communication
                if RFX_RPADC_SERIAL.port.inWaiting() > 0:
                    # line = RFX_RPADC_SERIAL.port.read(RFX_RPADC_SERIAL.port.inWaiting()).decode()
                    # print(RFX_RPADC_SERIAL.port.inWaiting())
                    try:
                        line = RFX_RPADC_SERIAL.port.readline().decode().strip()
                        # print (line)
                        # line = line.decode().strip()
                    # print (line)
                    except:
                        print("NON SONO RIUSCITO A STRIPPARE '" + line  + "' CONTINUO CON IL PROSSIMO CAMPIONE\n" )
                        read = False

                    if len(line) < 2:
                        # print ("Stringa vuota")
                        continue

                    # handling the temperature read
                    if "TEMP" in line:
                        tempRead = line.split(":")[1]
                        print ("TEMP READ: " + tempRead)
                        self.temps.append(tempRead)
                        self.nodeTemp.putRow(1, Int32(tempRead)/1e3, Int32(time.time()*1000 + POSIX_TIME_AT_EPICS_EPOCH))
                        continue

                    if "TYPE" in line:
                        trigType = line.split(":")[1]
                        # print ("TRIG TYPE: " + trigType)
                        if "T" in trigType:
                            # print ("Adding sample trifgRecv ", self.nodeTrigRecv)
                            self.nodeTrigRecv.putRow(1,  Int32(1.), Int32(time.time()*1000 + POSIX_TIME_AT_EPICS_EPOCH))
                        else:
                            self.nodeTrigRecv.putRow(1,  Int32(0.), Int32(time.time()*1000 + POSIX_TIME_AT_EPICS_EPOCH))
                        continue

                    # handling TIME:#time in clock pulses from first trigger
                    if "TIME" in line:
                        trigTime = line.split(":")[1]
                        print("TRIG TIME: " + str(trigTime))
                        if self.mode == "STREAMING":
                            curTime = self.numTrigs * period
                            self.trig_times.append(curTime)
                        else: 
                            self.trig_times.append(float(trigTime) * period)
                        self.numTrigs += 1
                    
                    # evaluating the line content only if the line has been read correctly
                    
                    if read:
                        try:
                            i_line = int(line)
                        except:
                            continue
                        raw_a = np.int16(i_line & 0xFFFF)     
                        raw_b = np.int16((i_line>>16) & 0xFFFF)
                        # print ("RAW_A: " + str(format(raw_a, 'b')))
                        # print ("Chan A: " + str(raw_a) + " Chan B: " + str(raw_b))
                        self.data_A.append(raw_a)
                        self.data_B.append(raw_b)
                    else:
                        # append the previous values of raw a and raw b if the line has not been read correctly
                        self.data_A.append(raw_a)
                        self.data_B.append(raw_b)
                
                    #### MDSPLUS SEGMENTS HANDLING ####
                    if len(self.data_A) == self.segSize:
                        # print ("PERIOD" +  str(period) + "\n")
                        segStart = self.startTime + (self.trig_times[-1] - self.trig_times[0]) - self.pre_samples * period
                        # segStart = self.startTime + (self.trig_times[-1] - self.trig_times[0])
                        print ("SEG SIZE: " + str(self.segSize) + " ULTIMO TRIG: " + str(self.trig_times[-1]) + " PRIMO TRIG: " + str(self.trig_times[0]))             
                        endTime = segStart + (self.segSize-1) * period
                        
                        # print("PERIOD: " + str(period) + " start: " + str(segStart) + " end: " + str(endTime))
                        mdsDimension = Range(segStart, endTime, Float32(period))
                        print ("NEW SEGMENT WITH DIMENSION: " + str(mdsDimension))
                        # print (self.trig_times[-1], self.trig_times[-2])

                        self.nodeA.makeSegment(segStart, endTime, mdsDimension, Int32Array(self.data_A))
                        self.nodeB.makeSegment(segStart, endTime, mdsDimension, Int32Array(self.data_B))
                        self.data_A=[]
                        self.data_B=[]
            print ("AsynchStore Run TERMINATED ")
        
        def hasError(self):
            return ( self.error != self.ACQ_NOERROR)
        
        def stop(self):
            print('Stop ASYNCH WORKER TERMINATED')

            try:
                period = 1./self.freq
                # copying remaining data
                if len(self.data_A) > 0 and self.mode != "STREAMING":
                    segStart = self.startTime + (self.trig_times[-1] - self.trig_times[0]) 
                    endTime = segStart + (len(self.data_A)-1) * period

                    # print("PERIOD: " + str(period) + " start: " + str(segStart) + " end: " + str(endTime))
                    mdsDimension = Range(segStart, endTime, Float32(period))
                    print (mdsDimension)

                    self.nodeA.makeSegment(segStart, endTime, mdsDimension, Int32Array(self.data_A))
                    self.nodeB.makeSegment(segStart, endTime, mdsDimension, Int32Array(self.data_B))

                    self.data_A=[]
                    self.data_B=[]
            except:
                print ("ERROR WHILE STOPPING: CAN'T STORE THE LAST SAMPLES")
            RFX_RPADC_SERIAL.stopAcq = True

    # Workers handling
    def saveWorker(self):
        RFX_RPADC_SERIAL.rfx_rpadc_serialWorkers[self.getNid()] = self.worker

    def restoreWorker(self):
        try:
            if self.getNid() in RFX_RPADC_SERIAL.rfx_rpadc_serialWorkers.keys():
                self.worker = RFX_RPADC_SERIAL.rfx_rpadc_serialWorkers[self.getNid()]
                return True
        except Exception as ex:
            print('Error in restoring worker : %s'%(str(ex)))
            raise mdsExceptions.TclFAILED_ESSENTIAL

    # Configuring the board through serial communication
    def configure(self, mode, clockMode, preSamples, postSamples, trigFromChanA, trigAboveThreshold, evLevel, evSamples, decimation, deadTime, offsA, offsB):
        
        debug_cmd = "debug=" + str(0) + "\n"
        print(debug_cmd)
        self.port.write(debug_cmd.encode())
        
        clockMode_cmd = "clock=" + str(clockMode) + "\n"
        print(clockMode_cmd)
        self.port.write(clockMode_cmd.encode())


        mode_cmd = "mode=" + str(mode) + "\n"
        print(mode_cmd)
        self.port.write(mode_cmd.encode())

        
        pre_cmd = "pre=" + str(preSamples) + "\n"
        print(pre_cmd)
        self.port.write(pre_cmd.encode())
        
        post_cmd = "post=" + str(postSamples) + "\n"
        print(post_cmd)
        self.port.write(post_cmd.encode())

        triga_cmd = "triga=" + str(trigFromChanA) + "\n"
        print(triga_cmd)
        self.port.write(triga_cmd.encode())

        trigup_cmd = "trigup=" + str(trigAboveThreshold) + "\n"
        print(trigup_cmd)
        self.port.write(trigup_cmd.encode())

        trigtr_cmd = "trigtr=" + str(evLevel) + "\n"
        print(trigtr_cmd)
        self.port.write(trigtr_cmd.encode())

        trsmp_cmd = "trsmp=" + str(evSamples) + "\n"
        print(trsmp_cmd)
        self.port.write(trsmp_cmd.encode())

        decim_cmd = "decim=" + str(decimation) + "\n"
        print(decim_cmd)
        self.port.write(decim_cmd.encode())

        deadt_cmd = "deadt=" + str(deadTime) + "\n"
        print(deadt_cmd)
        self.port.write(deadt_cmd.encode())

        offsA_cmd = "offsa=" + str(offsA) + "\n"
        print(offsA_cmd)
        self.port.write(offsA_cmd.encode())

        offsB_cmd = "offsb=" + str(offsB) + "\n"
        print(offsB_cmd)
        self.port.write(offsB_cmd.encode())

        init_cmd = "init\n"
        self.port.write(init_cmd.encode())


    def init(self):
        print('================= RPADC_SERIAL Init ===============')

        ser = serial.Serial('/dev/ttyr00', baudrate=115200)
        #ser = serial.Serial('/dev/ttyr00', baudrate=9600, bytesize = serial.SEVENBITS, parity = serial.PARITY_EVEN, stopbits = serial.STOPBITS_ONE)
        RFX_RPADC_SERIAL.port = ser
        RFX_RPADC_SERIAL.stopAcq = False

        try:
            # reading the channel from which the Redpitaya is triggered
            if(self.ev_channel.data() == "A"):
                trigFromChanA = 1
            else:
                trigFromChanA = 0
            print('MODE')

            # reading the operation mode
            try:
                print (self.mode.data())
                mode = modeDict[self.mode.data()]
            except:
                print('Invalid mode: ' + self.mode.data())
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            # handling clock mode
            try:
                clockMode = clockModeDict[self.clock_mode.data()]
            except:
                print('Invalid clock mode: ' + self.clock_mode.data())
                raise mdsExceptions.TclFAILED_ESSENTIAL
            # if clockMode == 4 or clockMode == 2:
            #     absTriggerTimeFromFPGA = 1
            # else:
            #     absTriggerTimeFromFPGA = 0
           

            # reading pre/post samples
            if self.mode.data() == 'STREAMING':
                preSamples = 0
                postSamples = 0
            else:
                try:
                    preSamples = self.pre_samples.data()
                    postSamples = self.post_samples.data()
                except:
                    print('Error reading pre or post samples')
                    raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                startTime = self.start_time.data()
                RFX_RPADC_SERIAL.startTime = startTime
            except:
                print('Error reading start time')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            # reading event mode
            try:
                if(self.ev_mode.data() == 'UPPER'):
                    trigAboveThreshold = 1
                else:
                    trigAboveThreshold = 0
            except:
                print('Error reading the event mode')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            # handling event configuration
            try:
                evLevel = self.ev_level.data()
                evSamples = self.ev_samples.data()
            except:
                print('Cannot get evLevel/evSamples')
            
            # handling decimation
            try:
                decimation = self.decimation.data()
                RFX_RPADC_SERIAL.decim = decimation
                frequency = 125E6/decimation
                frequency1 = frequency
            except:
                print('Cannot get decimation')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            # handling clock
            try:
                trig = self.trigger.getData()
            except:
                print('Cannot get trigger')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            if self.clock_mode.data() != 'INTERNAL':
                try:
                    if self.clock_mode.data() == 'TRIG_SYNC':
                        frequency1 = 1E6
                        frequency = 125E6/decimation
                    elif self.clock_mode.data() == 'SYNC':
                        frequency = 1E6 / decimation
                        frequency1 = 1E6
                    elif self.clock_mode.data() == 'TRIG_EXTERNAL':
                        period = Data.execute(
                            'slope_of($)', self.ext_clock)
                        frequency1 = 1./period
                        frequency = 125E6 / decimation
                    else:  #EXTERNAL
                        period = Data.execute(
                            'slope_of($)', self.ext_clock)
                        frequency1 = 1./period
                        frequency = frequency1 / decimation
                except:
                    print('Cannot resolve external clock')
                    raise mdsExceptions.TclFAILED_ESSENTIAL

        
            # handling dead time
            try:
                deadTime = self.dead_time.data()
                deadTime = int(deadTime * frequency)
            except:
                print('Cannot resolve dead time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            
            try:
                offsA = self.hw_offs_a.data()
                offsB = self.hw_offs_b.data()
            except:
                print('Cannot resolve offsets definition')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            self.configure(mode, clockMode, preSamples, postSamples, trigFromChanA, trigAboveThreshold, evLevel, evSamples, decimation, deadTime, offsA, offsB)

        except Exception as e:
            print(str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL


    def start_store(self):
        print('================= RPADC_SERIAL Start Store ===============')
        start_cmd = "start\n"
        self.port.write(start_cmd.encode())

        # check module in acquisition state
        try:
            if self.restoreWorker():
                if self.worker.isAlive():
                    Data.execute('DevLogErr($1,$2)', self.getNid(), 'Module is in acquisition')
                    return
        except:
            pass

        self.worker = self.AsynchStore() 
        self.worker.daemon = True
        self.worker.stopReq = False
        RFX_RPADC_SERIAL.rfx_rpadc_serialWorkers[self.nid] = self.worker
        
        self.worker.configure(self.copy())
        self.saveWorker()
        self.worker.start()

        time.sleep(2)

        if self.worker.hasError():
            self.worker.error = self.worker.ACQ_NOERROR 
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if not self.worker.isAlive():
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition thread not started')
            raise mdsExceptions.TclFAILED_ESSENTIAL

    def stop_store(self):
        print('================= RPADC_SERIAL Stop Store ===============')
        stop_cmd = "stop\n"
        RFX_RPADC_SERIAL.port.write(stop_cmd.encode())
        # print ("STO LEGGENDO GLI ULTIMI DATI: " + str(RFX_RPADC_SERIAL.port.inWaiting()))
        j = 0
        while (RFX_RPADC_SERIAL.port.inWaiting() > 0 and j < 3):
            print ("STO LEGGENDO GLI ULTIMI DATI: " + str(RFX_RPADC_SERIAL.port.inWaiting()))
            time.sleep(1)
            j = j + 1
        
        if not self.restoreWorker():
            print("Try to restoring...")
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Acquisition thread not created')
            return

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
            RFX_RPADC_SERIAL.port.close()
            self.worker.error = self.worker.ACQ_NOERROR
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
        time.sleep(1)
        RFX_RPADC_SERIAL.port.close()
