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

"""
RfxDevices
==========
@authors: Gabriele Manduchi (Consorzio RFX Padova)
@copyright: 2018
@license: GNU GPL
"""
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, c_int, byref
from time import sleep
import sys
import numpy as np


class NI6683(Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':REL_START', 'type':'numeric', 'value': -5},
        {'path':':ABS_START', 'type':'numeric', 'value': 0}]
    chanNames = ['PFI0', 'PFI1', 'PFI2', 'STAR0', 'STAR1','STAR2','STAR3','STAR4','STAR5','STAR6','STAR7','STAR8',
        'STAR9','STAR10','STAR11','STAR12', 'PXI_TRIG0', 'PXI_TRIG1', 'PXI_TRIG2', 'PXI_TRIG3', 'PXI_TRIG4', 
        'PXI_TRIG5','PXI_TRIG6', 'PXI_TRIG7'] 
    for chanName in chanNames:
        parts.append({'path':'.'+chanName, 'type':'structure'})
        parts.append({'path':'.'+chanName+':MODE', 'type':'text', 'value':'DISABLED'})
        parts.append({'path':'.'+chanName+':START', 'type':'numeric', 'value':-1})
        parts.append({'path':'.'+chanName+':END', 'type':'numeric', 'value':-1})
        parts.append({'path':'.'+chanName+':FREQUENCY', 'type':'numeric', 'value':1})
        parts.append({'path':'.'+chanName+':DUTY_CYCLE', 'type':'numeric', 'value':50})
        parts.append({'path':'.'+chanName+':PULSE_LEN', 'type':'numeric', 'value':1})
        parts.append({'path':'.'+chanName+':RAW_EVENTS', 'type':'numeric'})
        parts.append({'path':'.'+chanName+':REL_EVENTS', 'type':'numeric'})
        parts.append({'path':'.'+chanName+':EVENT_NAME', 'type':'text'})
    del(chanName)
    parts.append({'path':':DEV_TYPE', 'type':'text'})
    parts.append({'path':':INIT','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PON',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PON',51,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})


    DEV_IS_OPEN = 1
    DEV_OPEN = 2
    ACQ_NOERROR = 0
    ACQ_ERROR = 1
    niLib = None
    ni6683Fds = {}
    NISYNC_TIME_IMMEDIATE_NANOS = 0
    NISYNC_LEVEL_LOW = 0
    NISYNC_LEVEL_HIGH = 1
    NISYNC_READ_BLOCKING = 0
    NISYNC_READ_NONBLOCKING = 1


    def debugPrint(self, msg="", obj=""):
          print( self.name + ":" + msg, obj );

#saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        NI6683.ni6683Fds[self.nid] = self.fd

    def restoreInfo(self):
        if NI6683.niLib is None:
            NI6683.niLib = CDLL("libnisync.so")
            NI6683.termNameDict = {'PFI0': 0,'PFI1':1,'PFI2':2,'PXI_TRIG0': 11,
               'PXI_TRIG1': 12,'PXI_TRIG2': 13,'PXI_TRIG3': 14,'PXI_TRIG4': 15,'PXI_TRIG5': 16,
               'PXI_TRIG6': 17,'PXI_TRIG7': 18, 'PXI_STAR0': 19,'PXI_STAR0': 19,'PXI_STAR1': 20,
               'PXI_STAR2': 21,'PXI_STAR3': 22,'PXI_STAR4': 23,'PXI_STAR5': 24,'PXI_STAR6': 25,
               'PXI_STAR7': 26,'PXI_STAR8': 27,'PXI_STAR9': 28,'PXI_STAR10': 29,'PXI_STAR11': 30,
               'PXI_STAR12': 31}

        if self.nid in NI6683.ni6683Fds.keys():
            self.fd = NI6683.ni6683Fds[self.nid]
            return self.DEV_IS_OPEN
        else:
            try:
                boardId = self.board_id.data();
            except:
                emsg = 'Missing Board Id'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                raise DevBAD_PARAMETER
            try:
                devType = self.dev_type.data();
            except:
                emsg = 'Missing Dev Type'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                raise DevBAD_PARAMETER

            try:
	        self.fd =  NI6683.niLib.nisync_open_device(c_int(devType), c_int(boardId));
            except Exception as e:
                emsg = 'Cannot open device : %s'%(str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR
            self.termDict = {}
	    for termName in NI6683.termNameDict.keys():
                try:
                  self.termDict[termName] = NI6683.niLib.nisync_open_terminal(c_int(devType), c_int(boardId), c_int(self.termNameDict[termName]), 
                      c_int(NISYNC_READ_NONBLOCKING));
                except Exception as e:
                    emsg = 'Cannot open terminal %s : %s'%(termName, str(e))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevCOMM_ERROR
        return self.DEV_OPEN


    def closeInfo(self):
        if self.nid in NI6683.ni6683Fds.keys():
            self.fd = NI6683.ni6683Fds[self.nid]
            del(NI6683.ni6683Fds[self.nid])
	    for termName in NI6683.termNameDict.keys():
                try:
                  os.close(self.termDict[termName]);
                except Exception as e:
                    emsg = 'Cannot close terminal %s : %s'%(termName, str(e))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevCOMM_ERROR
            try:
                os.close(self.fd)
            except:
                emsg = 'Cannot close device ' + str(self.fd)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR

    
    def getAbsTime(self, relTime):
        try:
            return relTime - self.rel_start.data() + self.abs_start.data()
        except:
            emsg = 'Cannot convert relative time to absolute ' + str(self.fd)
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER

    def getStartEnd(self, termName):
        try:
            start = getattr(self, termName.lower()+'_start').data()
        except:
            start = 0
        if start < 0:
            start = 0
        try:
            end = getattr(self, termName.lower()+'_end').data()
        except:
            end = sys.maxint
        if end <= 0:
            end = sys.maxint
        if start >= end:
            emsg = 'End time less than start time in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
        return self.getAbsTime(start), self.getAbsTime(end)

    def getStartPulse(self, termName):
        try:
            start = getattr(self, termName.lower()+'_start').data()
        except:
            emsg = 'Error reading start time in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
	if np.isscalar(start):
            start = [start]
        try:
            pulseLen = getattr(self, termName.lower()+'_pulse_len').data()
        except:
            emsg = 'Error reading pulse len in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
	if np.isscalar(pulseLen):
            pulseLen = [pulseLen]
	minLen = len(start)
        if minLen > len(pulseLen):
            minLen = len(pulseLen)
        startNs = []
        endNs = []
        for i in range(0, minLen):
            currStartNs = self.getAbsTime(start[i])
            startNs.append(currStartNs)
            endNs.append(currStartNs + int(pulseLen[i] * 1000000000))
        return startNs, endNs


    def checkStatus(self, status, message):
        if(status != 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), message + 'status: %d'%(status))
            raise DevCOMM_ERROR


    def init(self):

        self.debugPrint('=================  PXI 6683 init ===============')

        self.restoreInfo()
        self.recorderTermNames = []
        for termName in NI6683.termNameDict.keys():
            mode = getattr(self, termName.lower()+'_mode').data()
            if mode == 'DISABLED':
                status = NI6683.niLib.nisync_disable_future_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
            elif mode == 'CLOCK':
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
                startNs, endNs = self.getStartEnd(termName)
                try:
                     freq = getattr(self, termName.lower()+'_frequency').data()
                except:
                   emsg = 'Invalid Frequency in ' + termName
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
                if frequency <= 0:
                   emsg = 'Invalid Frequency in ' + termName
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
                periodNs = int(1000000000./float(frequency))
                try:
                    dutyCycle = getattr(self, termName.lower()+'_duty_cycle').data()
                except:
                   emsg = 'Invalid Duty Cycle in ' + termName
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
                if dutyCycle <= 0 or dutyCicle >= 100:
                   emsg = 'Invalid Duty Cycle in ' + termName
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
                dutyCycleNs = int(periodNs * dutyCycle/100.)
                status = NI6683.niLib.replace_clock_ns(c_int(self.termNameDict[termName]), c_ulonglong(startNs), 
                    c_ulonglong(endNs), c_ulonglong(periodNs), c_ulonglong(dutyCycleNs))
                if status != 0:
                   emsg = 'Error in replace clock for %s status = %d '%(termName, status)
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
            elif mode == 'HIGH PULSE':
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable future events')
                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termNameDict[termName]), c_ulonglong(startNs), c_ubyte(NYSYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot geterate future event')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termNameDict[termName]), c_ulonglong(endNs), c_ubyte(NYSYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot geterate future event')
            elif mode == 'LOW PULSE':
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot enable future time events')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termNameDict[termName]), c_int(NYSYNC_LEVEL_HIGH))
                self.checkStatus(status, 'Cannot set terminal level')
                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termNameDict[termName]), c_ulonglong(startNs), c_ubyte(NYSYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot generate future event')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termNameDict[termName]), c_ulonglong(endNs), c_ubyte(NYSYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot generate future event')
            elif mode == 'HIGH':
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termNameDict[termName]), c_int(NYSYNC_LEVEL_HIGH))
                self.checkStatus(status, 'Cannot set terminal level')
            elif mode == 'LOW':
                status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termNameDict[termName]), c_int(NYSYNC_LEVEL_LOW))
                self.checkStatus(status, 'Cannot set terminal level')
            elif mode == 'RECORDER':
                status = NI6683.niLib.nisync_disable_future_time_events(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_enable_timestamp_trigger(c_int(self.termNameDict[termName]))
                self.checkStatus(status, 'Cannot enable timestamp triggers')
                self.recorderTermNames.append(termName)

    def start_store(self):

        self.debugPrint('=================  PXI 6683 start_store ===============')

        self.restoreInfo()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.configure(self)
        self.worker.start()    
             
                
    def stop_store(self):
      self.debugPrint('================= PXI 6683 stop store ================')
      if self.worker.isAlive():
          self.debugPrint("PXI 6683 stop_worker")
          self.worker.stop()
          self.worker.join()
                

    class AsynchStore(Thread):

        def configure(self, device):
            self.stopReq = False
            self.device = device
	    self.poll = select.poll()
	    self.nameDict = {}
            for name in self.device.recorderTermNames:
                self.poll.register(self.device.termDict[name])
                self.nameDict[self.device.termDict[name]] = name

        def run(self):
            while not self.stopReq:
                readyFds = self.poll.poll(1000)
                for fdTuple in readyFds:
                    readyFd = fdTuple[0]
                    timestamp = c_ulonglong()
                    status = NI6683.niLib.nisync_read_timestamps_ns(c_int(readyFd), byref(timestamp), c_int(1))
                    self.device.checkStatus(status, 'Cannot tread timestamps')
                    termName = self.nameDict[readyFd]
                    recorderNid = getattr(self.device, termName.lower()+'_raw_events')
                    recorderNid.putRow(10, timestamp, timestamp)

        def stop(self):
            self.stopReq = True


