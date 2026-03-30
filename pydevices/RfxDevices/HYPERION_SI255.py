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

#from MDSplus import mdsExceptions, Device, Data, Int8Array
from MDSplus import mdsExceptions, Device, Data, version, Float32, Int64
from threading import Thread
#from numpy import array
import socket
import time

#import hyperion
#import asyncio

class HYPERION_SI255(Device): 
    """HYPERION SI255"""

    parts = [  
        {'path': ':NAME', 'type': 'text'}, 
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':IP_ADDR', 'type': 'text', 'value':'192.168.111.54'}, 
        {'path': ':ACQ_MODE', 'type': 'text', 'value': 'PEAK'}, #PEAK or SPECTRA
        {'path': ':ACQ_FREQ', 'type': 'numeric', 'value': 1.0}, #Freq. in Hz
        {'path': ':TIME0', 'type': 'numeric', 'value': 0}, #Pulse time int64, should point to 6683
    ]
    for i in range(1, 9):
        parts.extend([
            {'path': '.CHANNEL_%02d' % (i), 'type': 'structure'},
            {'path': '.CHANNEL_%02d:INPUT_MODE' % (i), 'type': 'text', 'value': 'DIRECT'},  #DIRECT or SWITCH
            {'path': '.CHANNEL_%02d:INPUT_SWPATH' % (i), 'type': 'any'},
            {'path': '.CHANNEL_%02d:PEAK' % (i), 'type': 'signal'},
            {'path': '.CHANNEL_%02d:PEAK_RTIME' % (i), 'type': 'numeric', 'valueExpr': 'Data.compile("pvResample($1,,,,$2)", head.channel_%02d_peak, head.time0)' % (i)},
            {'path': '.CHANNEL_%02d:SPECTRA' % (i), 'type': 'signal'},
        ])
    del(i)
    parts.append({'path': ':INIT_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','INIT',50,None),Method(None,'startAcquisition',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('FEDE_SERVER','STORE',50,None),Method(None,'stopAcquisition',head))",
                  'options': ('no_write_shot',)})

    handles = {}
    workers = {}

    def debugPrint(self, msg="", obj=""):
        print("------ DEBUG  " + self.name + ":" + msg, obj)

    """Asynchronous readout internal class"""

    class AsynchStore(Thread):

        POSIX_TIME_AT_EPICS_EPOCH=int(631152000000000000)


        def configure(self, device):
            self.device = device
            self.stopReq = False

        def run(self):

            self.device.debugPrint("Asychronous acquisition thread")
 
            loop = self.device.asyncio.new_event_loop()
            self.device.asyncio.set_event_loop(loop)

            segSize=1000
            sampleIDX = 0
            chanAcqConf = []
            sleeptime = float(1.0/self.device.acq_freq)-float(0.01) #10ms fixed loop time correction  

            try:
              for i in range(1, 9):
                 inputMode=getattr(self.device, 'CHANNEL_%02d:INPUT_MODE' % (i)).getData() 
                 if(inputMode=="DIRECT"):
                   peakNid=getattr(self.device, 'CHANNEL_%02d:PEAK' % (i))
                   if(peakNid.isOn()):
                     chanAcqConf.append([i, 'D', peakNid])
                 else: #SWITCH ATTACHED 
                     switchNid=getattr(self.device, 'CHANNEL_%02d:INPUT_SWPATH' % (i)).getData()
                     swIpAddr=getattr(switchNid, 'IP_ADDR').getData()
                     swPort=getattr(switchNid, 'PORT').getData()
                     try:
                       connSw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                       connSw.connect((str(swIpAddr),int(swPort)))
                     except:
                       self.device.debugPrint("ERROR connecting to switch "+str(swIpAddr)+":"+str(swPort))
                       raise mdsExceptions.TclFAILED_ESSENTIAL
                     chSwAr = []
                     for j in range(1, 65): #foreach channel in switch
                       peakNid=getattr(switchNid, 'CHANNEL_%02d:PEAK' % (j))
                       if(peakNid.isOn()):
                         chSwAr.append([j,peakNid])
                     chSwLastIdx=0 #needed to loop through channels
                     chanAcqConf.append([i, 'S', chSwLastIdx, chSwAr, connSw])
            except Exception as exc:
              self.device.debugPrint("ERROR recovering input channel configurations... :"+str(exc))
              raise mdsExceptions.TclFAILED_ESSENTIAL

            #print(chanAcqConf)

            while not self.stopReq:
               try:
                peaks = self.device.handle.peaks  #acquire all channels
                #MDSplus: number of ms from 1-jan-1970; Interrogator: timestamp_int (s) timestamp_frac (ns) from 1-jan-1970
                #timestamp = int(peaks.header.timestamp_int * 1e3 + peaks.header.timestamp_frac * 1e-6)               

                #MDSplus: number of ns from 1-jan-1990; Interrogator: timestamp_int (s) timestamp_frac (ns) from 1-jan-1970
                timestamp = int(peaks.header.timestamp_int * 1e9 + peaks.header.timestamp_frac) - self.POSIX_TIME_AT_EPICS_EPOCH               

                sampleIDX+=1
                print("Sample IDX: ", sampleIDX, " acquired for all channels. Timestamp: ", timestamp)
               except Exception as exc:
                self.device.debugPrint("ERROR reading data peaks from interrogator... : "+str(exc))
               
               #store samples on MDS
#OLD: without switch device
#               try:
#                 for i in range(1, 9):
#                   peak = peaks[i]
#                   getattr(self.device, 'CHANNEL_%02d:PEAK' % (i)).putRow(1000, Float32(peak), Int64(timestamp))      
#               except:
#                 self.device.debugPrint("ERROR saving data to MDSplus...")
               try:
                 for curChan in chanAcqConf:
                    if(curChan[1]=='D'):
                      peak = peaks[curChan[0]]
                      curChan[2].putRow(segSize, Float32(peak), Int64(timestamp)) 
                    if(curChan[1]=='S'):
                      peak = peaks[curChan[0]] 
                      chSwLastIdx=curChan[2]
                      chSwAr=curChan[3]
                      #chSwAr[chSwLastIdx][0] #curr sw ch id 
                      chSwAr[chSwLastIdx][1].putRow(segSize, Float32(peak), Int64(timestamp)) 
                      print("  *Switch on Channel ",curChan[0]," connected to input position: ", chSwAr[chSwLastIdx][0])
                      chSwLastIdx+=1
                      if(chSwLastIdx==len(chSwAr)):  #len(chSwAr) is the number of active channels in switch
                        chSwLastIdx=0
                      curChan[2]=chSwLastIdx
                      switchNextPos = chSwAr[chSwLastIdx][0] #SWITCH CHANNEL ID FOR NEXT ACQUISITION
                      connSw=curChan[4]
                      print("Sending TCP command to switch " + connSw.getpeername()[0] + ":" + str(connSw.getpeername()[1]) + " move to channel " + str(switchNextPos)) 
                      cmdString=str(switchNextPos).zfill(3)
                      print("cmdString:"+cmdString)
                      connSw.send(b"*SW" + cmdString.encode("ASCII") + b"\r")
                      #aggiunta la risposta dello switch, da testare
                      #time.sleep(0.02) #forse va aggiunto un ritardo se la risposta non viene letta
                      respRecv=connSw.recv(64) # 
                      print("response received: "+respRecv)
                      #aggiungere un check sulla risposta
                      

               except:
                 self.device.debugPrint("ERROR saving data to MDSplus...")

               #wait according to acquisition frequency
               time.sleep(sleeptime)     

            self.device.debugPrint("End acquisition thread")
            #close hyperion? seems not necessary! 

            self.device.removeInfo()
            # raise mdsExceptions.TclFAILED_ESSENTIAL

        def stop(self):
            self.device.debugPrint("STOP thread acquisition loop")
            self.stopReq = True


    def saveWorker(self):
        HYPERION_SI255.workers[self.nid] = self.worker

    ###save Info###
    # saveInfo and restoreInfo allow to manage multiple occurrences
    # and to avoid opening and closing devices handles
    def saveInfo(self):
        HYPERION_SI255.handles[self.nid] = self.handle

    ###restore worker###
    def restoreWorker(self):
        if self.nid in HYPERION_SI255.workers.keys():
            self.worker = HYPERION_SI255.workers[self.nid]
        else:
            Data.execute("DevLogErr($1,$2)", self.nid, "Cannot restore worker!!")
            raise mdsExceptions.TclFAILED_ESSENTIAL


    ###restore info###
    def restoreInfo(self):
        self.debugPrint("restore Info")
        try:
           import hyperion
        except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: cannot import hyperion library')
        try:
           import asyncio
           self.asyncio = asyncio
        except:
           Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: cannot import asyncio library')

        if self.nid in HYPERION_SI255.handles.keys():
            self.handle = HYPERION_SI255.handles[self.nid]
            self.debugPrint("RESTORE INFO HANDLE TROVATO", self.handle)
        else:
            self.debugPrint("RESTORE INFO HANDLE NON TROVATO")
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute("DevLogErr($1,$2)", self.nid, "Missing IP address")
                raise mdsExceptions.TclFAILED_ESSENTIAL

            self.debugPrint("Opening...")

            self.handle = -1
            self.ip = ipAddr
            self.handle = hyperion.Hyperion(ipAddr)
#            self.handle = hyperion.AsyncHyperion(ipAddr) #cannot use header information
            if(self.handle.is_ready):
               self.debugPrint("Device open and ready. Device Handle: ", self.handle)
            else: 
               self.debugPrint("ERROR opening device ", self.ip)
               raise mdsExceptions.TclFAILED_ESSENTIAL
            

    ###remove info###
    def removeInfo(self):
        try:
            del HYPERION_SI255.handles[self.nid]
        except:
            self.debugPrint("ERROR TRYING TO REMOVE INFO")



    ##########init############################################################################
    def init(self):
        if self.restoreInfo() == 0:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.saveInfo()

        print("Init Hyperion SI255 interrogator")
#        try:
#            self.frames.setCompressOnPut(False)
#        except:
#            Data.execute(
#                "DevLogErr($1,$2)",
#                self.nid,
#                "Cannot disable automatic compresson on put for frames node",
#            )
#            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get Name
        try:
            name = self.name.data()
            print('Name: ', name)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'WARNING: device with no name')
            #raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get IP Address
        try:
            ipAddr = self.ip_addr.data()
            print('IP Addr: ', ipAddr)
        except:
            ipAddr = ""
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'ERROR: Put a valid IP address.')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get Acquisition Mode
        try:
            acqMode = self.acq_mode.data()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'WARNING: Acquisition Mode not defined')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        if (acqMode=="PEAK"):
           print('Acquisition Mode: PEAK')
        else:
           print('Acquisition Mode: SPECTRA. This acquisition mode is NOT yet implemented')
           raise mdsExceptions.TclFAILED_ESSENTIAL

        
        # Get Acquisition Frequency
        try:
            self.acqFreq = self.acq_freq.data()
            print('Acquisition Frequency [Hz]: ', self.acqFreq)
            if(self.acqFreq>50.0):
              Data.execute('DevLogErr($1,$2)', self.nid,'ERROR: Acquisition Frequency >50Hz.')
              raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            self.acqFreq = 1.0
            Data.execute('DevLogErr($1,$2)', self.nid,'ERROR: Put a valid acquisition Frequency.')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        #h1.channel_count   #8
#        h1.active_full_spectrum_channel_numbers = [1,2,3,4,5,6,7,8]
       
        for i in range(1, 9):
          try:
              inputMode = getattr(self, 'CHANNEL_%02d:INPUT_MODE' % (i)).data()
          except:
              Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: Invalid inputMode on channel %02d.' % (i))
              raise mdsExceptions.TclFAILED_ESSENTIAL
          print('Channel ',i,': input mode ', inputMode)
#            if (inputMode=="DIRECT"):


    ##########start acquisition############################################################################
    def startAcquisition(self):
        if self.restoreInfo() == 0:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Module in acquisition check
        try:
            self.restoreWorker()
            if self.worker != None and self.worker.isAlive():
                self.debugPrint("Stop acquisition Thread...")
                self.stopAcquisition()
                self.debugPrint("Initialize...")
                self.init()
                self.restoreInfo()
        except:
            pass

        self.debugPrint("Starting Acquisition...")
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False

        #aggiungere comandi eseguiti prima del loop di acquisizione
        self.debugPrint("OK!")
        self.worker.configure(self.copy())
        self.saveWorker()
        self.worker.start()


    ##########stop acquisition############################################################################
    def stopAcquisition(self):
        self.debugPrint("Stop acquisition Thread...")
        self.restoreWorker()
        if self.worker != None and self.worker.isAlive():
            # if self.restoreWorker() :
            self.debugPrint("Stop Worker...")
            self.worker.stop()




