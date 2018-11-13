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
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array, Uint64, Event
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, c_int, byref, c_byte, c_ulonglong, c_ubyte
from time import sleep
import sys
import numpy as np
import select

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
    ni6683Dicts = {}
    ni6683RecorderDict = {}
    ni6683WorkerDict = {}
    NISYNC_TIME_IMMEDIATE_NANOS = 0
    NISYNC_LEVEL_LOW = 0
    NISYNC_LEVEL_HIGH = 1
    NISYNC_READ_BLOCKING = 0
    NISYNC_READ_NONBLOCKING = 1
    NISYNC_EDGE_RISING	= 0
    NISYNC_EDGE_FALLING	= 1
    NISYNC_EDGE_ANY = 2


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
               'PXI_TRIG6': 17,'PXI_TRIG7': 18, 'STAR0': 19,'STAR0': 19,'STAR1': 20,
               'STAR2': 21,'STAR3': 22,'STAR4': 23,'STAR5': 24,'STAR6': 25,
               'STAR7': 26,'STAR8': 27,'STAR9': 28,'STAR10': 29,'STAR11': 30,
               'STAR12': 31}
            NI6683.typeDict = {'PXI6682': 0, 'PCI1588': 1,'PXI6683': 2,'PXI6683H': 3}


        if self.nid in NI6683.ni6683Fds.keys():
            self.fd = NI6683.ni6683Fds[self.nid]
            self.termDict = NI6683.ni6683Dicts[self.nid]
            return self.DEV_IS_OPEN
        else:
            try:
                boardId = self.board_id.data();
            except:
                emsg = 'Missing Board Id'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                raise DevBAD_PARAMETER
            try:
                devType = NI6683.typeDict[self.dev_type.data()]
            except:
                emsg = 'Missing Dev Type'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                raise DevBAD_PARAMETER

            try:
	        self.fd =  NI6683.niLib.nisync_open_device(c_int(devType), c_int(boardId))
            except Exception as e:
                emsg = 'Cannot open device : %s'%(str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR
            self.termDict = {}
	    for termName in NI6683.termNameDict.keys():
                try:
                    self.termDict[termName] = NI6683.niLib.nisync_open_terminal(c_int(devType), c_int(boardId), c_int(NI6683.termNameDict[termName]), c_int(self.NISYNC_READ_NONBLOCKING))
                    print(termName + ': '+str(self.termDict[termName]))
                except Exception as e:
                    emsg = 'Cannot open terminal %s : %s'%(termName, str(e))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevCOMM_ERROR
        NI6683.ni6683Fds[self.nid] = self.fd
        NI6683.ni6683Dicts[self.nid] = self.termDict
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
                del NI6683.ni6683Fds[self.nid]
            except:
                emsg = 'Cannot close device ' + str(self.fd)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR

    
    def getAbsTime(self, relTime):
        try:
            return int((relTime - self.rel_start.data())*1000000000 + self.abs_start.data())
        except:
            emsg = 'Cannot convert relative time to absolute ' + str(self.fd)
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER

    def getRelTime(self, absTime):
        try:
            return float((absTime - self.abs_start.data())/1E9 + self.rel_start.data())
        except:
            emsg = 'Cannot convert absolute time to relative ' + str(self.fd)
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER

    def getStartEnd(self, termName):
        try:
            start = getattr(self, termName.lower()+'_start').data()
            if start <= 0:
                start = 0
            else:
                start = self.getAbsTime(start)
        except:
            start = 0
        try:
            end = getattr(self, termName.lower()+'_end').data()
            if end <= 0:
                end = sys.maxint
            else:
                end = self.getAbsTime(end)
        except:
            end = sys.maxint
        if start >= end:
            emsg = 'End time less than start time in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
        return start, end

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
        NI6683.ni6683RecorderDict[self.nid] = []
        for termName in NI6683.termNameDict.keys():
            mode = getattr(self, termName.lower()+'_mode').data()
            if mode == 'DISABLED':
                enabled = c_byte()
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value != 0:
                    print('DISABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable future events')
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
            elif mode == 'CLOCK':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, 'Cannot abort FTEs')
                startNs, endNs = self.getStartEnd(termName)
                try:
                     frequency = getattr(self, termName.lower()+'_frequency').data()
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
                if dutyCycle <= 0 or dutyCycle >= 100:
                   emsg = 'Invalid Duty Cycle in ' + termName
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevBAD_PARAMETER
                dutyCycleNs = int(periodNs * dutyCycle/100.)
                print('Generate Clock: '+str(startNs) + '  '+str(endNs)+'  ' + str(periodNs) + '  '+str(dutyCycleNs)) 
                status = NI6683.niLib.nisync_generate_clock_ns(c_int(self.termDict[termName]), c_ulonglong(startNs), 
                    c_ulonglong(endNs), c_ulonglong(periodNs), c_ulonglong(dutyCycleNs))
                if status != 0:
                   emsg = 'Error in replace clock for %s status = %d '%(termName, status)
                   Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                   raise DevCOMM_ERROR
            elif mode == 'HIGH PULSE':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_LOW))
                self.checkStatus(status, 'Cannot set terminal level')
                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termDict[termName]), c_ulonglong(startNs[i]), c_ubyte(self.NISYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot generate future event')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termDict[termName]), c_ulonglong(endNs[i]), c_ubyte(self.NISYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot generate future event')
            elif mode == 'LOW PULSE':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_HIGH))
                self.checkStatus(status, 'Cannot set terminal level')
                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termDict[termName]), c_ulonglong(startNs[i]), c_ubyte(self.NISYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot generate future event')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(self.termDict[termName]), c_ulonglong(endNs[i]), c_ubyte(self.NISYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot generate future event')
            elif mode == 'HIGH':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_HIGH))
                self.checkStatus(status, 'Cannot set terminal level')
            elif mode == 'LOW':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_LOW))
                self.checkStatus(status, 'Cannot set terminal level')
            elif mode == 'RECORDER RISING':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value != 0:
                    print('DISABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                print('ENABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                status = NI6683.niLib.nisync_enable_timestamp_trigger(c_int(self.termDict[termName]), 
                        c_int(self.NISYNC_EDGE_RISING), c_int(1))
                self.checkStatus(status, 'Cannot enable timestamp triggers')
                NI6683.ni6683RecorderDict[self.nid].append(termName)
            elif mode == 'RECORDER FALLING':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value != 0:
                    print('DISABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                print('ENABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                status = NI6683.niLib.nisync_enable_timestamp_trigger(c_int(self.termDict[termName]), 
                        c_int(self.NISYNC_EDGE_FALLING), c_int(1))
                self.checkStatus(status, 'Cannot enable timestamp triggers')
                print('ADESSO ZONTO ' + termName +' in ', NI6683.ni6683RecorderDict[self.nid])
                NI6683.ni6683RecorderDict[self.nid].append(termName)
            elif mode == 'RECORDER ANT':
                enabled = c_byte()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events')
                if enabled.value != 0:
                    print('DISABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable future events')
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]), 
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')
                print('ENABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                status = NI6683.niLib.nisync_enable_timestamp_trigger(c_int(self.termDict[termName]), 
                        c_int(self.NISYNC_EDGE_ANY), c_int(1))
                self.checkStatus(status, 'Cannot enable timestamp triggers')
                print('ADESSO ZONTO ' + termName +' in ', NI6683.ni6683RecorderDict[self.nid])
                NI6683.ni6683RecorderDict[self.nid].append(termName)
        return 1

    def soft_init(self):
        self.debugPrint('=================  PXI 6683 soft_init ===============')

        self.restoreInfo()
        currentTime = c_ulonglong(0)
        status = NI6683.niLib.nisync_get_time_ns(c_int(self.fd), byref(currentTime))
        self.checkStatus(status, 'Cannot get current time')
        self.abs_start.putData(Uint64(currentTime.value))
        return self.init()

        


    def start_store(self):

        self.debugPrint('=================  PXI 6683 start_store ===============')
        print('CICCIO')
        print ('RECORDER DEV NAMES:', NI6683.ni6683RecorderDict[self.nid]) 
        print('BOMBO')
        self.restoreInfo()
        worker = self.AsynchStore()
        NI6683.ni6683WorkerDict[self.nid] = worker
        worker.daemon = True
        worker.configure(self)
        worker.start()  
        return 1  
             
                
    def stop_store(self):
      self.debugPrint('================= PXI 6683 stop store ================')
      worker = NI6683.ni6683WorkerDict[self.nid]
      if worker.isAlive():
          self.debugPrint("PXI 6683 stop_worker")
          worker.stop()
          worker.join()
          return 1
                

    class AsynchStore(Thread):

        def configure(self, device):
            self.stopReq = False
            self.device = device
	    self.poll = select.poll()
	    self.nameDict = {}
            READ_ONLY = select.POLLIN | select.POLLPRI | select.POLLHUP | select.POLLERR

            for name in NI6683.ni6683RecorderDict[self.device.nid]:
                self.poll.register(NI6683.ni6683Dicts[self.device.nid][name], READ_ONLY)
                self.nameDict[NI6683.ni6683Dicts[self.device.nid][name]] = name
                print('REGISTRATO ', NI6683.ni6683Dicts[self.device.nid][name])

        def run(self):
            while not self.stopReq:
                readyFds = self.poll.poll(1000)
                for fdTuple in readyFds:
                    readyFd = fdTuple[0]
                    event = fdTuple[1]
                    print('EVENTO: ', fdTuple, select.EPOLLIN)
                    if event & select.EPOLLIN == 0:
                        print('NO DATA')
                    if event & select.EPOLLERR != 0:
                        print 'POLL ERROR!!'
                        return
                        continue
                    timestamp = c_ulonglong()
                    print('READY FD: ', readyFd)
                    status = NI6683.niLib.nisync_read_timestamps_ns(c_int(readyFd), byref(timestamp), c_int(1))
                    self.device.checkStatus(status, 'Cannot get current time')
                    termName = self.nameDict[readyFd]
                    print('ECCO EVENTO DA TERM ' + termName)
                    recorderNid = getattr(self.device, termName.lower()+'_raw_events')
                    eventRelTime = getRelTime(self, timestamp.value)
                    recorderNid.putRow(10, Float64(eventRelTime), Float64(eventRelTime))
                    try:
                        eventNameNid = getattr(self.device, termName.lower()+'_event_name')
                        eventName = eventDameNid.data()
                        print('SPEDISCO EVENTO: '+ eventName, timestamp.value)
                        Event.setevent(eventName, Uint64(timestamp.value))
                    except:
                        pass
          

        def stop(self):
            self.stopReq = True


