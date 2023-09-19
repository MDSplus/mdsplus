import MDSplus
from ctypes import CDLL, c_char_p, c_short, byref, c_int
import socket
import numpy as np


def recvall(sock, n):
# Helper function to recv n bytes or return None if EOF is hit
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

class ELAD(MDSplus.Device):
 
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP', 'type': 'text'}, 
        {'path': ':PORT', 'type': 'numeric', 'value': 8000}, 
        {'path': ':HW_TRIG', 'type': 'numeric', 'value': 0},
        {'path': ':TRIG_TIME', 'type': 'numeric', 'value': 0},
        {'path': ':PTS', 'type': 'numeric', 'value': 1000},
    ]
    for i in range(12):
        parts.extend([
            {'path': '.CHANNEL_%d' % (i+1), 'type': 'structure'},
            {'path': '.CHANNEL_%d:DATA' % (i+1), 'type': 'signal', 'options': (
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
            useHwTrigger = self.hw_trig.data()
        except:
            print("Missing use HW Trigger flag")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        

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

            sock.send(b'ARM')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        ELAD.socketDict[self.getNid()] = sock

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
            sock.send(b'STR')
            numSamples = int.from_bytes(sock.recv(4),'little')
            print('num Samples: ', numSamples)
            samples = np.zeros(numSamples, np.int)
            numChanSamples = int(numSamples/12)
            print('num Chan Samples: ', numChanSamples)
            samples = np.frombuffer(recvall(sock, 4 * numSamples), dtype = np.int32)
            print('Received Samples: ', len(samples))
        except:
            print("Cannot read samples from socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        timebase = MDSplus.Range(trigTime, trigTime + 1E-6 * numChanSamples, 1E-6)            
        for chan in range(12):
            currSig = MDSplus.Signal(samples[chan * numChanSamples:(chan+1) * numChanSamples], None, timebase)
            self.__getattr__('channel_%d_data' % (chan+1)).putData(currSig)
        sock.close()

    


