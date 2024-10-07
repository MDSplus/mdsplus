import MDSplus
from ctypes import CDLL, c_char_p, c_short, byref, c_int
import socket
import numpy as np
from threading import Thread
import time

def recvall(sock, n):
# Helper function to recv n bytes or return None if EOF is hit
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

stopAcq = {}


class ELAD(MDSplus.Device):
 
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP', 'type': 'text'}, 
        {'path': ':PORT', 'type': 'numeric', 'value': 8000}, 
        {'path': ':HW_TRIG', 'type': 'numeric', 'value': 0},
        {'path': ':HW_AUTO_TRIG', 'type': 'numeric', 'value': 0},
        {'path': ':TRIG_TIME', 'type': 'numeric', 'value': 0},
        {'path': ':PTS', 'type': 'numeric', 'value': 1000},
        {'path': ':FREQ_DIV', 'type': 'numeric', 'value': 10000},
        {'path': ':AUTOZERO_TIM', 'type': 'numeric', 'value': 2.},
        {'path': ':REC_PORT', 'type': 'numeric', 'value': 8111},
        {'path': ':ACT_CHANS', 'type': 'numeric', 'value':2},
        {'path': ':CALIBRATION', 'type': 'text', 'value':"NO"},
        {'path': ':CHOP_ENA', 'type': 'text', 'value':"NO"},
        {'path': ':STREAM_MODE', 'type': 'text', 'value':'TCP'},
        {'path': ':JSCOPE_EV', 'type': 'text', 'value':'JSCOPE_EVENT'},
    ]
    for i in range(12):
        parts.extend([
            {'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'},
            {'path': '.CHANNEL_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            {'path': '.CHANNEL_%d:CHOP_ENA' % (i+1), 'type': 'numeric', 'value': 0 },
            {'path': '.CHANNEL_%d:LH_MODE' % (i+1), 'type': 'numeric', 'value':0},
            {'path': '.CHANNEL_%d:ID' % (i+1), 'type': 'text'},
            {'path': '.CHANNEL_%d:STATUS' % (i+1), 'type': 'text'},
            {'path': '.CHANNEL_%d:EPROM' % (i+1), 'type': 'text'}
        ])
    for i in range(12):
        parts.extend([
            {'path': '.STREAM_%d' % (i+1), 'type': 'structure'},
            {'path': '.STREAM_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
        ])
    for i in range(12):
        parts.extend([
            {'path': '.STRINT_%d' % (i+1), 'type': 'structure'},
            {'path': '.STRINT_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
        ])
    parts.extend([
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('ELAD_SERVER','INIT',50,None),Method(None,'init',head))",
         'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('ELAD_SERVER','STORE',50,None),Method(None,'store',head))",
         'options': ('no_write_shot',)},
    ])
    del(i)

    socketDict = {}
    class AsynchStoreUdp(Thread):

            def configure(self, device):
                self.device = device
                self.nid = device.getNid()
                try:
                    self.trigTime = device.trig_time.data()
                except:
                    self.trigTime = 0.

            def run(self):
                sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
                sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                sock.bind(('',self.device.rec_port.data()))
                freqDiv = self.device.freq_div.data()
                actFreq = 1E6/freqDiv
                segmentSize = int(0.5* 1E6/freqDiv)  #save a segment every 0.5 seconds
                print('SEGMENT SIZE: ', segmentSize)
                activeChans = self.device.act_chans.data()
                print('ACTIVE CHANS: ', activeChans)
                chans = []
                for chanIdx in range(2*activeChans+1):
                    chans.append(np.zeros(segmentSize, dtype = np.int32))
                for chanIdx in range(12):
                    getattr(self.device, 'stream_%d_data' % (chanIdx+1)).deleteData()
                for chanIdx in range(12):
                    getattr(self.device, 'strint_%d_data' % (chanIdx+1)).deleteData()
                stopped = False
                while not stopped:
                    actSegmentSize = 0
                    for sampleIdx in range(segmentSize):
                        sampleBuf = sock.recv(4*(2*activeChans+1))
                        chans[2*activeChans][sampleIdx] = np.frombuffer(sampleBuf[:4], dtype = np.int32)
                        for chanIdx in range(2*activeChans):
                            print(np.frombuffer(sampleBuf[(chanIdx + 1)*4:(chanIdx + 2)*4], dtype = np.int32))
                            chans[chanIdx][sampleIdx] = np.frombuffer(sampleBuf[(chanIdx + 1)*4:(chanIdx + 2)*4], dtype = np.int32)
                        actSegmentSize = actSegmentSize+1
                        if stopAcq[self.nid]:
                            break
                    if stopAcq[self.nid]:
                        stopped = True
                    period = freqDiv/1E6
                    startTime = MDSplus.Float64(chans[2*activeChans][0]*period)
                    endTime = MDSplus.Float64(chans[2*activeChans][actSegmentSize - 1]*period)
                    timebase = MDSplus.Range(startTime, endTime, MDSplus.Float64(period))
                    for chanIdx in range(activeChans):
                        getattr(self.device, 'stream_%d_data' % (chanIdx+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int32Array(chans[chanIdx]))
                    for chanIdx in range(activeChans):
                        getattr(self.device, 'strint_%d_data' % (chanIdx+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int32Array(chans[activeChans+chanIdx]))
                    MDSplus.Event.setevent(self.jscope_ev.data())


    class AsynchStoreTcp(Thread):

            def configure(self, device):
                self.device = device
                self.nid = device.getNid()
                try:
                    self.trigTime = device.trig_time.data()
                except:
                    self.trigTime = 0.

            def run(self):
                serverSocket = socket.socket()
                serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                serverSocket.bind((socket.gethostname(),self.device.rec_port.data()))
                serverSocket.listen(1)
                sock, addr = serverSocket.accept()
                print('TCP Connection established')
                freqDiv = self.device.freq_div.data()
                actFreq = 1E6/freqDiv
                segmentSize = int(1000* (int(1E6/freqDiv)/int(1000)))  #save a segment every second rouded to 1000 samples
                if segmentSize == 0:
                    print('Invalid frequency division for TCP streaming. It must be less than 1000 (Samplig rate > 1kHz)')
                    return
                activeChans = self.device.act_chans.data()
                chans = []
                for chanIdx in range(2*activeChans+1):
                    chans.append(np.zeros(segmentSize, dtype = np.int32))
                for chanIdx in range(12):
                    getattr(self.device, 'stream_%d_data' % (chanIdx+1)).deleteData()
                for chanIdx in range(12):
                    getattr(self.device, 'strint_%d_data' % (chanIdx+1)).deleteData()
                stopped = False
                actSamples = 0
                prevSamples = 0
                while not stopped:
                    tcpPacketLen = 1000*4*(2*activeChans)
                    for sampleSetIdx in range(int(segmentSize/1000)):
                        sampleBuf = bytearray()
                        while len(sampleBuf) < tcpPacketLen:
                            packet = sock.recv(tcpPacketLen - len(sampleBuf))
                            if not packet:
                                return None
                            sampleBuf.extend(packet)
                        for chanIdx in range(2*activeChans):
                            chans[chanIdx][sampleSetIdx*1000:(sampleSetIdx+1)*1000] = np.frombuffer(sampleBuf[chanIdx*4*1000:(chanIdx + 1)*4*1000], dtype = np.int32)
                        actSamples += 1000
                        if stopAcq[self.nid]:
                            break
                    if stopAcq[self.nid]:
                        stopped = True
                    period = freqDiv/1E6
                    startTime = MDSplus.Float64(prevSamples*period)
                    endTime = MDSplus.Float64(actSamples*period)
                    timebase = MDSplus.Range(startTime, endTime, MDSplus.Float64(period))
                    print('segment size: '+str(len(chans[chanIdx])))
                    print('startTime: '+str(startTime))
                    print('endTime: '+str(startTime))
                    print('dim: '+str(timebase))
                    for chanIdx in range(activeChans):
                        getattr(self.device, 'stream_%d_data' % (chanIdx+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int32Array(chans[chanIdx]))
                    for chanIdx in range(activeChans):
                        getattr(self.device, 'strint_%d_data' % (chanIdx+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int32Array(chans[activeChans+chanIdx]))
                    MDSplus.Event.setevent('ELAD_JSCOPE')
                    prevSamples = actSamples
                sock.shutdown(socket.SHUT_RDWR)
                sock.close()
                serverSocket.shutdown(socket.SHUT_RDWR)
                serverSocket.close()


    def init(self):
        try:
            ip = self.ip.data()
        except:
            print("Missing IP")
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            port = self.port.data()
        except:
            print("Missing Port")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            pts = self.pts.data()
        except:
            print("Missing PTS")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            freqDiv = self.freq_div.data()
        except:
            print("Missing Frequency division")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            autozeroTime = self.autozero_tim.data()
        except:
            print("Missing Autozero time")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            useHwTrigger = self.hw_trig.data()
        except:
            print("Missing use HW Trigger flag")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        
        try:
            useHwAutozeroTrigger = self.hw_auto_trig.data()
        except:
            print("Missing use HW Trigger flag")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
    
        try:
            recPort = self.rec_port.data()
        except:
            print("Missing Receive port")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        
        modeReg = int(0) 
        chopActive = False    
        globalChopEnable = self.chop_ena.data() == 'YES'        
        for chan in range(12):
            try:
                chopEna = self.__getattr__('channel_%d_chop_ena' % (chan+1)).data()
            except:
                chopEna = 0
            if globalChopEnable and chopEna > 0:
                modeReg |= (1 << (chan+1))
                chopActive = True
            try:
                lhMode = self.__getattr__('channel_%d_lh_mode' % (chan+1)).data()
            except:
                lhMode = 0
            if lhMode > 0:
                modeReg |= (1 << (chan+16))
        if chopActive:
            modeReg |= 1
        calEnabled = self.calibration.data()
        if calEnabled == 'YES':
            modeReg |= (1 << 30)
        #globalChopEnable = self.chop_ena.data() == 'YES'

        localIp = socket.gethostbyname(socket.gethostname())

        try:
            sock = ELAD.socketDict[self.getNid()]
            sock.close()
            sock.connect((ip, port))
        except:
            print('Connecting....')
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                sock.connect((ip, port))
            except:
                print("Cannot connect to "+ip+"  Port "+str(port))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        
        try:
            sock.send(b'CHK')
            one = 1
            sock.send(one.to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'PTS')
            sock.send(pts.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'DIV')
            sock.send(freqDiv.item().to_bytes(4,'little'))
            print(sock.recv(2))

            autoTicks = np.int32(autozeroTime * 1000)
            sock.send(b'AUT')
            sock.send(autoTicks.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'IPP')
            ipLen = np.int32(len(localIp))
            sock.send(ipLen.item().to_bytes(4,'little'))
            sock.send(bytes(localIp, 'utf-8'))
            sock.send(recPort.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'MOD')
            sock.send(modeReg.to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'IDS')
 #           ids = np.frombuffer(recvall(sock, 8 * 12), dtype = np.int8)

            ids = np.frombuffer(recvall(sock, 16 * 12), dtype = np.int8)   
 
            for chan in range(12):
                id = ''
                for i in range(16):  # era 7
     #              id += hex(ids[chan*12+i])[2:] #id += hex(ids[chan*8+i])[2:]
                    id += chr(ids[(chan*16)+i])
                print(id)
                self.__getattr__('channel_%d_id' % (chan+1)).putData(MDSplus.String(id))

   #         sock.send(b'EPR')
   #         for chan in range(12):
   #             numBytes = int.from_bytes(sock.recv(4),'little')
   #             epromBuffer  =  recvall(sock, numBytes)
   #             epromString = epromBuffer.decode("utf-8")
   #             self.__getattr__('channel_%d_eprom' % (chan+1)).putData(MDSplus.String(epromString))

        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        ELAD.socketDict[self.getNid()] = sock

    def arm(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'ARM')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def trigger(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'TRG')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

 
    def autozeroTrigger(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'TRS')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def startStream(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        
        try:
            streamMode = self.stream_mode.data()
        except:
            print("Missing STREAM_MODE")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        if streamMode != 'UDP' and streamMode != 'TCP':
            print("Invalid STREAM_MODE: "+streamMode)
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        
        if streamMode == 'UDP':
            self.worker = self.AsynchStoreUdp()
        else:
            self.worker = self.AsynchStoreTcp()
        self.worker.configure(self)
        self.worker.daemon = True
        stopAcq[self.getNid()] = False
        self.worker.start()

        time.sleep(1)

        localIp = socket.gethostbyname(socket.gethostname())
        try:
            recPort = self.rec_port.data()
        except:
            print("Missing Receive port")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        

        sock.send(b'IPP')
        ipLen = np.int32(len(localIp))
        sock.send(ipLen.item().to_bytes(4,'little'))
        sock.send(bytes(localIp, 'utf-8'))
        sock.send(recPort.item().to_bytes(4,'little'))
        print(sock.recv(2))

        try:
            sock.send(b'STS')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def stopStream(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        stopAcq[self.getNid()] = True
        try:
            sock.send(b'STO')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def store(self):
        try:
            sock = ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            trigTime = self.trig_time.data()
        except:
            print("Cannot retrieve trigger time")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            activeChans = self.act_chans.data()
        except:
            print("Cannot retrieve number of active chans")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'STA')
            statusReg =  int.from_bytes(sock.recv(4),'little')
            activeChans = statusReg & 0x0000000F
            print('Active Chans: ', activeChans)
            self.act_chans.putData(MDSplus.Int32(activeChans))
            for chan in range(12):
                if statusReg & (1 << (4+chan)) > 0:
                    self.__getattr__('channel_%d_status' % (chan+1)).putData(MDSplus.String("ERROR"))
                else:
                    self.__getattr__('channel_%d_status' % (chan+1)).putData(MDSplus.String("OK"))
            sock.send(b'STR')
            numSamples = int.from_bytes(sock.recv(4),'little')
            print('num Samples: ', numSamples)
            samples = np.zeros(numSamples, np.int32)
            numChanSamples = int(numSamples/activeChans)
            print('num Chan Samples: ', numChanSamples)
            samples = np.frombuffer(recvall(sock, 4 * numSamples), dtype = np.int32)
            print('Received Samples: ', len(samples))
        except:
            print("Cannot read samples from socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        timebase = MDSplus.Range(trigTime, trigTime + 1E-6 * numChanSamples, 1E-6)            
        for chan in range(activeChans):
            currSig = MDSplus.Signal(samples[chan * numChanSamples:(chan+1) * numChanSamples], None, timebase)
            self.__getattr__('channel_%d_data' % (chan+1)).putData(currSig)

        #sock.close()


    


