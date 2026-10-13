import MDSplus
from ctypes import CDLL, c_char_p, c_short, byref, c_int
import socket
import numpy as np
from threading import Thread
import time
import subprocess
import struct


def removeBlanksInString(msg):
    inString = False
    outMsg = ''
    for i in range(len(msg)):
        if msg[i] == '"':
            inString = not inString
        else:
            if inString and msg[i] == ' ':
                outMsg+='_'
            else:
                outMsg += msg[i]
    return outMsg


def recvall(sock):
    data = []

    while True:
        ch = sock.recv(1)
        if not ch:
            raise EOFError("Socket closed before CRLF")
        data.append(ch)
        if len(data) >= 2 and data[-2:] == [b'\r', b'\n']:
            return b''.join(data[:-2])

# Helper function to recv n bytes or return None if EOF is hit
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

class EPICA(MDSplus.Device):
 
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP', 'type': 'text'}, 
        {'path': ':PORT', 'type': 'numeric', 'value': 1894}, 
       
        {'path': ':TRIGGER', 'type': 'numeric', 'value': 0}, 
        {'path': ':DECIMATION', 'type': 'numeric', 'value': 0}, 
        {'path': ':DELAY', 'type': 'numeric', 'value':100}, 
        {'path': ':ACQ_MS', 'type': 'numeric'}, 
        {'path': '.INFO', 'type': 'structure'},
        {'path': '.INFO:VERSION', 'type': 'text'}, 
        {'path': '.INFO:FPGA_VER', 'type': 'text'},
        {'path': '.INFO:SERIAL', 'type': 'text'}]
    for i in range(8):
        parts.extend([
            {'path': '.INFO.MODULE_%d' % (i+1), 'type': 'structure'},
            {'path': '.INFO.MODULE_%d:HW_INFO' % (i+1), 'type': 'text'},
            {'path': '.INFO.MODULE_%d:SER_NUM' % (i+1), 'type': 'text'},
            {'path': '.INFO.MODULE_%d:IS_PRESENT' % (i+1), 'type': 'text'},
        ])
    parts.extend([{'path': '.CONFIG', 'type': 'structure'}])
    for i in range(8):
        parts.extend([
            {'path': '.CONFIG.MODULE_%d' % (i+1), 'type': 'structure'},
            {'path': '.CONFIG.MODULE_%d:CFG' % (i+1), 'type': 'numeric', 'value':0},
            {'path': '.CONFIG.MODULE_%d:SHUNT_A' % (i+1), 'type': 'numeric'},
            {'path': '.CONFIG.MODULE_%d:SHUNT_B' % (i+1), 'type': 'numeric'}
        ])

    parts.extend([{'path': '.OUTPUTS', 'type': 'structure'}])
    for i in range(8):
        parts.extend([
            {'path': '.OUTPUTS.MODULE_%d' % (i+1), 'type': 'structure'},
            {'path': '.OUTPUTS.MODULE_%d:MODE' % (i+1), 'type': 'text', 'value':'DC'},
            {'path': '.OUTPUTS.MODULE_%d:RAMP' % (i+1), 'type': 'text'},
            {'path': '.OUTPUTS.MODULE_%d:DC_VAL' % (i+1), 'type': 'numeric', 'value':0}
        ])

    parts.extend([{'path': '.ACQUISITION', 'type': 'structure'}])
    for i in range(8):
        parts.extend([
            {'path': '.ACQUISITION.MODULE_%d' % (i+1), 'type': 'structure'},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_A' % (i+1), 'type': 'structure'},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_A:V' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_A:I' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_B' % (i+1), 'type': 'structure'},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_B:V' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            {'path': '.ACQUISITION.MODULE_%d.CHAN_B:I' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
    ])
    socketDict = {}
    startTimeDict = {}
    trigTimeDict = {}

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

        ramps = []
        modes = []
        dcs = []
        maxRampSteps = 0
        for i in range(8):
            currMode = self.getNode('OUTPUTS.MODULE_'+str(i+1)+':MODE').data()
            modes.append(currMode)
            try:
                if currMode == 'RAMP':
                    rampStr = self.getNode('OUTPUTS.MODULE_'+str(i+1)+':RAMP').data()
                    ramp = ''
                    rampComponents = rampStr.split()
                    if maxRampSteps < len(rampComponents):
                        maxRampSteps = len(rampComponents)

                    for currVal in rampComponents:
                        if currVal.startswith('0x'):
                            ramp += currVal + ' '
                        else:
                            ramp += '0x'+currVal+' '
                    ramps.append(ramp)
                    dcs.append(0)
                elif currMode == 'DC':
                    dcs.append(self.getNode('OUTPUTS.MODULE_'+str(i+1)+':DC_VAL').data())
                    ramps.append(0)
                else:
                    print('Invalid output mode setting for module '+str(i+1))
                    raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            except:
                print('Invalid output setting for module '+str(i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        cfg = []
        shunta = []
        shuntb = []

        for i in range(8):
            try:
                currCfg = self.getNode('CONFIG.MODULE_'+str(i+1)+':CFG').data()
                currShunta = self.getNode('CONFIG.MODULE_'+str(i+1)+':SHUNT_A').data()
                currShuntb = self.getNode('CONFIG.MODULE_'+str(i+1)+':SHUNT_B').data()
                cfg.append(currCfg)
                shunta.append(currShunta)
                shuntb.append(currShuntb)
            except:
                print('Invalid configuration for module '+str(i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            delay = self.getNode('DELAY').data()
        except:
            print("Missing Delay")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            decimation = self.getNode('DECIMATION').data()
        except:
            print("Missing Decimation")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL


        try:
            sock = EPICA.socketDict[self.getNid()]
            sock.close()
            sock.connect((ip, port))
            answ = recvall(sock)
        except:
            print('Connecting....')
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                sock.connect((ip, port))
                answ = recvall(sock)
                EPICA.socketDict[self.getNid()] = sock
            except:
                print("Cannot connect to "+ip+"  Port "+str(port))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

 #Info readout
        try:
            print('Sending IDN?')
            sock.send(b'IDN?')
            answ = recvall(sock)
            print(answ)
            answTokens = answ.split()
            self.getNode('.INFO:SERIAL').putData(answTokens[1])
            self.getNode('.INFO:VERSION').putData(answTokens[2])
            self.getNode('.INFO:FPGA_VER').putData(answTokens[3])
        except:
            print('Cannot get device info')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

#module info readout
        modulePresent = []
        presentCount = 0
        try:
            print('Sending TOP?')
            sock.send(b'TOP?')
            moduleInfoStr = recvall(sock)
            print(moduleInfoStr)
          #  moduleInfoTokens = moduleInfoStr.split()
            moduleInfoTokens = removeBlanksInString(str(moduleInfoStr)).split()
            for i in range(8):
                currModuleTokens = moduleInfoTokens[i].split('=')
                if currModuleTokens[1] == '-':
                    modulePresent.append(False)
                    self.getNode('INFO.MODULE_'+str(i+1)+':IS_PRESENT').putData('NO')
                    print('Module '+str(i+1)+ 'missing')
                else:
                    presentCount += 1
                    modulePresent.append(True)
                    self.getNode('INFO.MODULE_'+str(i+1)+':IS_PRESENT').putData('YES')
                    self.getNode('INFO.MODULE_'+str(i+1)+':HW_INFO').putData(currModuleTokens[1].split('/')[0])
                    self.getNode('INFO.MODULE_'+str(i+1)+':SER_NUM').putData(currModuleTokens[1].split('/')[1])
        except:
            print('Cannot get device info')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        print(str(presentCount)+' modules found')

#configure modules
        for i in range(8):
            try:
                if modulePresent[i]:
                    configStr = 'CFG'+str(i)+ ' '+str(cfg[i]) + ' '+str(shunta[i])+' '+str(shuntb[i])+';'
                    print('Sending '+configStr)
                    sock.send(configStr.encode('utf-8'))
                    answ = recvall(sock).decode()
                    print(answ)
                    if not answ.startswith('OK'):
                        print('Error returned by CFG command: '+answ + ' for module '+str(i+1))
                        raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            except:
                print('Error sending configuration for module '+str(i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

#configure ramps
            rampCfgCommand = 'AWG'
            for i in range(8):
                try:
                    if modulePresent[i]:
                        if modes[i] == 'RAMP':
                            rampCmd = 'BRD'+str(i)+' '+ramps[i][:-1]+';'
                            print('Sending '+rampCmd)
                            sock.send(rampCmd.encode('utf-8'))
                            answ = recvall(sock).decode()
                            print(answ)
                            if answ.startswith('ERR'):
                                print('Error received when sending ramp valued for module '+str(i+1))
                                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
                            rampCfgCommand += ' '+str(i)+':R'
                        else:
                            rampCfgCommand += ' '+str(i)+':D:'+hex(dcs[i])
                except:
                    print('Error configuring ramp/DC value for module '+str(i+1))
                    raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

            print(rampCfgCommand)
            try:
                rampCfgCommand += ';'
                print('Sending '+rampCfgCommand)
                sock.send(rampCfgCommand.encode('utf-8'))
                answ = recvall(sock).decode()
                print(answ)
                if answ.startswith('ERR'):
                    print('Error received when sending ramp/DC configuration')
                    raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            except:
                print('Error sending ramp/DC configuration')
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

            try:
                cmd = 'BST'+str(delay)+';'
                print('Sending '+cmd)
                sock.send(cmd.encode('utf-8'))
                answ = recvall(sock)
                print(answ)
            except:
                print('Error sending playback step configuration')
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

#Compute and store ACQ_MS is not defined 
            try:
                acqMs = self.getNode(':ACQ_MS').data()
            except:
                acqMs = maxRampSteps * int(delay) // 1000
            self.getNode(':ACQ_MS').putData(MDSplus.Int32(acqMs))
            print('Acquisition length set to '+str(acqMs)+' milliseconds')
            for chan in range(8):
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_A:V' % (chan+1)).deleteData()
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_A:I' % (chan+1)).deleteData()
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_B:V' % (chan+1)).deleteData()
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_B:I' % (chan+1)).deleteData()



 
    def trigger(self):
        try:
            firstTriggerTime = EPICA.startTimeDict[self.getNid()]
            EPICA.trigTimeDict [self.getNid()] = time.time() - firstTriggerTime
        except:
            EPICA.startTimeDict[self.getNid()] = time.time()
            EPICA.trigTimeDict[self.getNid()] = 0

        try:
            sock = EPICA.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            decimation = self.getNode('DECIMATION').data()
        except:
            print("Cannot read decimation")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            acqMs = self.getNode('ACQ_MS').data()
        except:
            print("Cannot read acquisition lenght")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            cmd = 'ACQ'+str(int(acqMs))+ ' '+str(int(decimation))+'?'
            print('Sending '+cmd)
            sock.send(cmd.encode('utf-8')) 
            answ = recvall(sock).decode()
            print(answ)
            if answ.startswith('ERR'):
                print('Error received when starting DMA readout')
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            retTokens = answ.split()
            fileIdx = int(retTokens[3].split('=')[1])
            numChans = int(retTokens[4].split('=')[1])
            pts = int(retTokens[5].split('=')[1])
            dmaSize = int(retTokens[6].split('=')[1])
            decimation = int(retTokens[7].split('=')[1])
            periodUs = int(retTokens[7].split('=')[1])
            decimationDict = {0:32,1:64,2:128,3:256,4:512,5:1024,6:2048,7:4096};   
            freq = 20E6/decimationDict[decimation]
            triggerTime = EPICA.trigTimeDict [self.getNid()]

            startTime = MDSplus.Float64(triggerTime) 
            endTime = MDSplus.Float64(triggerTime + pts/freq)        
            timebase = MDSplus.Range(startTime, endTime, MDSplus.Float64(1/freq))   
        except:
            print('Error requesting DMA readout')
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        for chan in range(8):
            if self.getNode('INFO.MODULE_'+str(chan+1)+':IS_PRESENT').data() == 'YES':
                remoteFile = 'petalinux@'+self.getNode('IP').data()+':dma/%05d_acq_CH%02d.dak'%(fileIdx, chan)
                localFile = '%05d_acq_CH%02d.dak'%(fileIdx, chan)
                print('Copying '+remoteFile +'...')
                subprocess.run(['scp', remoteFile, localFile], check=True,)
                print('Copied')

                currChanAVolt = np.zeros(pts//2, dtype=np.int16)
                currChanACurr = np.zeros(pts//2, dtype=np.int16)
                currChanBVolt = np.zeros(pts//2, dtype=np.int16)
                currChanBCurr = np.zeros(pts//2, dtype=np.int16)
                print('ALLOCATI ', pts//2)
                try:
                    chanArray = np.fromfile(localFile, dtype = np.int32)
                except:
                    print('Cannot read local file '+localFile)
        #Baco nel riportare i pts...sembra qualche volta uno di piu'
                if len(chanArray) < pts:
                    print('Unexpected number or read samples. Read: '+str(len(chanArray))+' Expected: '+str(pts))
                    raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

                if pts % 2 != 0:
                    pts -= 1
        #################################################             
                print(len(chanArray), pts//2)

                for sampleCount in range(pts):
                    data = chanArray[sampleCount]
                    if sampleCount % 2 == 0:
                        currChanAVolt[sampleCount//2] =  data & np.uint32(0x0000FFFF)
                        currChanACurr[sampleCount//2] = (data & np.uint32(0xFFFF0000)) >> 16
                    else:
                        currChanBVolt[sampleCount//2] =  data & np.uint32(0x0000FFFF)
                        currChanBCurr[sampleCount//2] = (data & np.uint32(0xFFFF0000)) >> 16

                self.getNode('.ACQUISITION.MODULE_%d.CHAN_A:V' % (chan+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int16Array(currChanACurr))
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_A:I' % (chan+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int16Array(currChanAVolt))
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_B:V' % (chan+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int16Array(currChanBCurr))
                self.getNode('.ACQUISITION.MODULE_%d.CHAN_B:I' % (chan+1)).makeSegment(startTime, endTime, timebase, MDSplus.Int16Array(currChanBVolt))
