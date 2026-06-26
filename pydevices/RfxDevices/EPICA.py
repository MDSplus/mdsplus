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


class EPICA(MDSplus.Device):
 
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP', 'type': 'text'}, 
        {'path': ':PORT', 'type': 'numeric', 'value': 8888}, 
        {'path': ':TRIGGER', 'type': 'numeric', 'value': 0}, 
        {'path': ':DECIMATION', 'type': 'numeric', 'value': 0}, 
        {'path': ':FIFO_1', 'type': 'text'}, 
        {'path': ':FIFO_2', 'type': 'text'}, 
        {'path': ':FIFO_3', 'type': 'text'}, 
        {'path': ':FIFO_4', 'type': 'text'}, 
        {'path': ':FIFO_5', 'type': 'text'}, 
        {'path': ':FIFO_6', 'type': 'text'}, 
        {'path': ':FIFO_7', 'type': 'text'}, 
        {'path': ':FIFO_8', 'type': 'text'}, 
        {'path': ':DELAY', 'type': 'numeric',  'value': 100}, 
        {'path': ':SHUNTA_1', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_2', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_3', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_4', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_5', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_6', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_7', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTA_8', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_1', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_2', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_3', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_4', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_5', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_6', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_7', 'type': 'numeric', 'value':0}, 
        {'path': ':SHUNTB_8', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_1', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_2', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_3', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_4', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_5', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_6', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_7', 'type': 'numeric', 'value':0}, 
        {'path': 'CONFIG_8', 'type': 'numeric', 'value':0}, 
    ]
    for i in range(8):
        parts.extend([
            {'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'},
            {'path': '.CHANNEL_%d:V' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            {'path': '.CHANNEL_%d:I' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
       ])

    socketDict = {}

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
            fifoStr = self.getNode('FIFO_1').data()
        except:
            print("Missing FIFO")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        fifoRegs = fifoStr.split()
        fifo = []
        try:
            for fifoReg in fifoRegs:
                fifo.append(int(fifoReg, 16))
        except:
            print('invalid format for FIFO. Hex values separated by space must be defined')
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


        shuntA = []
        for i in range(8):
            try:
                shunt = self.getNode('SHUNTA_%d' % (i+1)).data()
            except:
                print('Missing SHUNTA %d' % (i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            shuntA.append(shunt)
        shuntB = []
        for i in range(8):
            try:
                shunt = self.getNode('SHUNTB_%d' % (i+1)).data()
            except:
                print('Missing SHUNTA %d' % (i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            shuntB.append(shunt)

        config = []
        for i in range(8):
            try:
                conf = self.getNode('CONFIG_%d' % (i+1)).data()
            except:
                print('Missing CONFIG %d' % (i+1))
                raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            config.append(conf)

        try:
            sock = EPICA.socketDict[self.getNid()]
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

            sock.send(b'FIF')
            l = len(fifo)
            sock.send(l.to_bytes(4,'little'))
            print(fifo)
            for fifoReg in fifo:
                print('Sending ', fifoReg)
                sock.send(fifoReg.to_bytes(4, 'little'))
            print(sock.recv(2))

            sock.send(b'DEL')
            sock.send(delay.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'DEC')
            sock.send(decimation.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'SHA')
            for  shunt in shuntA:
                sock.send(shunt.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'SHB')
            for  shunt in shuntB:
                sock.send(shunt.item().to_bytes(4,'little'))
            print(sock.recv(2))

            sock.send(b'CID')
            for  conf in config:
                sock.send(conf.item().to_bytes(4,'little'))
            print(sock.recv(2))

        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        EPICA.socketDict[self.getNid()] = sock

    def arm(self):
        try:
            sock = EPICA.socketDict[self.getNid()]
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
            sock = EPICA.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'TRG')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def clear(self):
        try:
            sock = EPICA.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'CLR')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

 

    def store(self):
        try:
            sock = EPICA.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
  
        try:
            sock.send(b'RDD')
            pts =  int.from_bytes(sock.recv(4),'little')
            activeChans =  int.from_bytes(sock.recv(4),'little')
            print('PTS: ', pts)
            print('Active Chans: ', activeChans)
            numDmaSamples = pts * activeChans
            dmaSamples = np.frombuffer(recvall(sock, 2 * numDmaSamples), dtype = np.int16)  
            print('LETTI SAMPLES')          
        except:
            print("SCannot read samples")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            trigTime = self.getNode('TRIGGER').data()
        except:
            trigTime = 0

        try:
            decimation = self.getNode('DECIMATION').data()
        except:
            print("SCannot read decimation")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        decimationDict = {0:32,1:64,2:128,2:256,4:512,5:1024,6:2048,7:4096};   
        freq = 20E6/decimationDict[decimation]
        timebase = MDSplus.Range(trigTime, trigTime + pts/freq, 1/freq)            
        for chan in range(int(activeChans/2)):
            convExpr = self.getTree().tdiCompile("$VALUE")
 
            rawMdsData = MDSplus.Int32Array(dmaSamples[2 * chan * pts:2 * chan * pts + pts])
            rawMdsData.setUnits("Count")
            convExpr.setUnits("Volt")
            currSig = MDSplus.Signal(convExpr, rawMdsData, timebase)
            self.getNode('.CHANNEL_%d:V' % (chan+1)).putData(currSig)

            rawMdsData = MDSplus.Int32Array(dmaSamples[(2 * chan +1) * pts:(2 * chan +1) * pts + pts])
            rawMdsData.setUnits("Count")
            convExpr.setUnits("Ampere")
            currSig = MDSplus.Signal(convExpr, rawMdsData, timebase)
            self.getNode('.CHANNEL_%d:I' % (chan+1)).putData(currSig)


    


