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




import MDSplus
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

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('EladIn', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT)
class MARTE2_ELAD(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Time', 'type': 'int32', 'dimensions': 0, 'parameters': []},
        {'name': 'Chan_1', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_2', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_3', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_4', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_5', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_6', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_7', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_8', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_9', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_10', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_11', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_12', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_1_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_2_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_3_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_4_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_5_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_6_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_7_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_8_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_9_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_10_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_11_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        {'name': 'Chan_12_int', 'type': 'float64', 'dimensions':0, 'parameters': []},
        ]
    parameters = [
        {'name': 'Port', 'type': 'int32', 'value': 8200}, 
        {'name': 'EladPort', 'type': 'int32', 'value': 8888}, 
        {'name': 'EladIp', 'type': 'string'}, 
        {'name': 'TrigTime', 'type': 'float64', 'value':0}, 
        {'name': 'FreqDiv', 'type': 'int32', 'value': 100}, 
        {'name': 'Pts', 'type': 'int32', 'value': 100000}, 
        {'name': 'AutozeroTim', 'type': 'float64', 'value':1.}, 
    ]

    parts = []
    socketDict = {}

    @classmethod
    def postBuild(cls):
        cls.parts.append({'path': '.CHANNELS_DMA', 'type': 'structure'})
        for i in range(12):
            cls.parts.extend([
                {'path': '.CHANNELS_DMA.CHANNEL_%d' % (i+1), 'type': 'structure'},
                {'path': '.CHANNELS_DMA.CHANNEL_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'no_write_model', 'compress_on_put')},
            ])

    def prepareMarteInfo(self):
        freqDiv = self.getNode('parameters.par_5:value')
        period = 1E-6 * freqDiv
        self.getNode('timebase').putData(MDSplus.Range(None, None, MDSplus.Float64(period)))


    def init(self):
        try:
            ip = self.getNode('parameters.par_3:value').data()
        except:
            print("Missing IP")
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            port = self.getNode('parameters.par_2:value').data()
        except:
            print("Missing Port")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            pts = self.getNode('parameters.par_6:value').data()
        except:
            print("Missing PTS")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            freqDiv = self.getNode('parameters.par_5:value').data()
        except:
            print("Missing Frequency division")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            autozeroTime = self.getNode('parameters.par_7:value').data()
        except:
            print("Missing Autozero time")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    
        try:
            sock = MARTE2_ELAD.socketDict[self.getNid()]
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

        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        MARTE2_ELAD.socketDict[self.getNid()] = sock

    def arm(self):
        try:
            sock = MARTE2_ELAD.socketDict[self.getNid()]
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
            sock = MARTE2_ELAD.socketDict[self.getNid()]
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
            sock = MARTE2_ELAD.socketDict[self.getNid()]
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
            sock = MARTE2_ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        localIp = socket.gethostbyname(socket.gethostname())
        try:
            recPort = self.getNode('parameters.par_2:value').data()
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
            sock = MARTE2_ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            sock.send(b'STO')
            print(sock.recv(2))
        except:
            print("Socket communication failed")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    def store(self):
        try:
            sock = MARTE2_ELAD.socketDict[self.getNid()]
        except:
            print("Cannot retrieve socket")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            trigTime = self.getNode('parameters.par_4:value').data()
        except:
            print("Cannot retrieve trigger time")
            raise  MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        activeChans = 12
        try:
            sock.send(b'STR')
            numSamples = int.from_bytes(sock.recv(4),'little')
            print('num Samples: ', numSamples)
            samples = np.zeros(numSamples, np.int)
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
            self.__getattr__('channels_dma_channel_%d_data' % (chan+1)).putData(currSig)

        #sock.close()

