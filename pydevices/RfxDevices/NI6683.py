#!/usr/bin/env python
# -*- coding: iso-8859-15 -*-
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
@authors: Gabriele Manduchi & Luca Trevisan (Consorzio RFX Padova)
@copyright: 2023
@license: GNU GPL
"""
from MDSplus import Device, Data, Range, Uint64, Event, Float64, Float64Array, Tree
from MDSplus.mdsExceptions import DevCOMM_ERROR, DevBAD_PARAMETER, PyUNHANDLED_EXCEPTION
from threading import Thread
import threading
from ctypes import CDLL, Structure, c_int, byref, c_int8, c_uint8, c_uint32, c_uint64, c_char, get_errno
import os
import sys
import numpy as np
import select
from collections import OrderedDict
import errno
import time


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
        parts.append({'path':'.'+chanName+':RAW_EVENTS', 'type':'signal'})
        parts.append({'path':'.'+chanName+':REL_EVENTS', 'type':'numeric'})
        parts.append({'path':'.'+chanName+':EVENT_NAME', 'type':'text'})
    del(chanName)
    parts.append({'path':':DEV_TYPE', 'type':'text'})
    parts.append({'path':':INIT','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':TRIGGER','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PULSE_ON',10,None),Method(None,'trigger',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':TRIG_EVENT', 'type':'text'})

    for chanName in chanNames:
        parts.append({'path':'.'+chanName+':COMMENT', 'type':'text'})
    del(chanName)
    
    for chanName in chanNames:
        parts.append({'path':'.'+chanName+':CLOCK_SOURCE', 'type':'numeric'})  #added on 04/09/2025
    del(chanName)
    

    DEV_IS_OPEN = 1
    DEV_OPEN = 2
    ACQ_NOERROR = 0
    ACQ_ERROR = 1
    niLib = None
    niInterfaceLib = None
    ni6683Fds = {}
    ni6683Dicts = {}
    ni6683RecorderDict = {}
    ni6683WorkerDict = {}

    ni6683Modes = {}
    ni6683Frequencies = {}
    ni6683DutyCycles = {}
    ni6683StartClockNs = {}
    ni6683EndClockNs = {}
    ni6683StartPulseNs = {}
    ni6683EndPulseNs = {}
    ni6683TermStarts = {}
    ni6683TermEnds = {}
    ni6683PulseLengths = {}
    ni6683ModuleTriggerName = ""
    ni6683RelTime = 0
    ni6683AbsTime = 0


    NISYNC_TIME_IMMEDIATE_NANOS = 0
    NISYNC_LEVEL_LOW = 0
    NISYNC_LEVEL_HIGH = 1
    NISYNC_READ_BLOCKING = 0
    NISYNC_READ_NONBLOCKING = 1
    NISYNC_EDGE_RISING    = 0
    NISYNC_EDGE_FALLING    = 1
    NISYNC_EDGE_ANY = 2
    NISYNC_BACKPLANE_CLK = 2
    NISYNC_TIMEKEEPER_CLK = 3
    NISYNC_CLKOUT = 5
    NISYNC_BOARD_CLK	= 1
    eventTime = None
    device = None
    deviceNID = 0
    useInternalReference = False
    DEVMODE = 1

    class nisync_device_info(Structure):
        _fields_ = [("driver_version", c_char* 30),
                    ("model", c_char * 10),
                    ("name", c_char * 10),
                    ("state", c_int),
                    ("time_since_sync", c_uint64),
                    ("serial_number", c_uint32),
                    ("signature", c_uint32),
                    ("revision", c_uint32),
                    ("oldest_compatible_revision", c_uint32),
                    ("hardware_revision", c_uint32)]

    def thread_alive(self, thread):
        if getattr(thread, "is_alive", None):
            alive = thread.is_alive()
        elif getattr(thread, "isAlive", None):
            alive = thread.isAlive()
        else:
            print("Python version error")
            emsg = 'ERROR: Python version error'
            Data.execute('DevLogErr($1)', emsg)
            raise PyUNHANDLED_EXCEPTION
        return alive

    def reset_device(self):
        enabled = c_int8()
        activeEdge = c_int()
        decimationCount = c_int()

        print("RESETTING DEVICE...")

        for termName in NI6683.termNameDict.keys():
            status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
            self.checkStatus(status, "Cannot abort future time events when resetting the device ")

            status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
            self.checkStatus(status, 'Cannot inquire future events in "DISABLED" mode')
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

    def debugPrint(self, msg="", obj=""):
          print( self.name + ":" + msg, obj )

# saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        print("Saving " + str(self.fd))
        NI6683.ni6683Fds[self.nid] = self.fd

    def restoreInfo(self):
        if NI6683.niLib is None:
            NI6683.niLib = CDLL("libnisync.so", use_errno=True)
            NI6683.termNameDict = {'PFI0': 0,'PFI1':1,'PFI2':2,'PXI_TRIG0': 11,
               'PXI_TRIG1': 12,'PXI_TRIG2': 13,'PXI_TRIG3': 14,'PXI_TRIG4': 15,'PXI_TRIG5': 16,
               'PXI_TRIG6': 17,'PXI_TRIG7': 18}
            NI6683.typeDict = {'PXI6682': 0, 'PCI1588': 1,'PXI6683': 2,'PXI6683H': 3}
# loading the NiInterface library (/opt/mdsplus/device_support/national/NiInterface.cpp)
        if NI6683.niInterfaceLib is None:
            NI6683.niInterfaceLib = CDLL('libNiInterface.so')

# investigating the file descriptors to check if the device is already opened
        if self.nid in NI6683.ni6683Fds.keys():
            self.fd = NI6683.ni6683Fds[self.nid]
            self.termDict = NI6683.ni6683Dicts[self.nid]
            return self.DEV_IS_OPEN
        else: # if not opened, initialize the device by reading the information stored in the pulse file
            try:
                boardId = self.board_id.data()
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
                    print('Opening ' + termName + ' with FD: '+str(self.termDict[termName]))
                    if (self.termDict[termName] == -1): # if terminal is busy, raise an exception
                        emsg = 'Cannot open terminal ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevCOMM_ERROR
                except Exception as e:
                    emsg = 'Cannot open terminal %s : %s'%(termName, str(e))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevCOMM_ERROR
        NI6683.ni6683Fds[self.nid] = self.fd
        NI6683.ni6683Dicts[self.nid] = self.termDict
        return self.DEV_OPEN

# getAbsTime converts the passed relative time in absolute time 
    def getAbsTime(self, relTime):
        print ('DEBUG -> Abs time: ', NI6683.ni6683AbsTime, ' Reltime: ', relTime, ' RelStart: ', NI6683.ni6683RelTime)
        try:
            result = ((relTime - NI6683.ni6683RelTime)*1000000000 + NI6683.ni6683AbsTime)
            # print ('DEBUG -> Result: ', result)
            return result
        except:
            emsg = 'Cannot convert relative to absolute time' + str(self.fd)
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER

    # returns the start and end times of the terminal in the absolute time reference
    def getStartEnd(self, termName):
        termNameNid = termName+str(self.nid)
        try:
            start = NI6683.ni6683TermStarts[termNameNid]
            # if the start time of the selected terminal is less than the shot relative time -> error
            if start < NI6683.ni6683RelTime:
                emsg = 'Start time less than relative time in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            else:
                start = self.getAbsTime(start)
        except:
            print ("Exception in defining Start")
            start = self.getAbsTime(start)

        # retrieving the end time for the slected terminal
        try:
            end = NI6683.ni6683TermEnds[termNameNid]
            if end <= 0:
                end = sys.maxsize # maxint in python 2.*
            else:
                end = self.getAbsTime(end)
        except:
            end = sys.maxsize

        if start >= end:
            emsg = 'End time less than start time in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
        return start, end

    # returns startNs[] and endNs[] arrays containing the time slots of the programmed pulses
    def getStartPulse(self, termName):
        termNameNid = termName+str(self.nid)
        try:  
            # print(NI6683.ni6683TermStarts)
            start = NI6683.ni6683TermStarts[termNameNid]
        except:
            emsg = 'Error reading start time in ' + termName
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevBAD_PARAMETER
        if np.isscalar(start):
            start = [start]
        try:
            pulseLen = NI6683.ni6683PulseLengths[termNameNid]
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
            print ('DEBUG -> START PULSE in GetStartPulse: ', start[i])
            currStartNs = self.getAbsTime(start[i])
            startNs.append(currStartNs)
            endNs.append(currStartNs + int(pulseLen[i] * 1000000000))
        return startNs, endNs


    def checkStatus(self, status, message):
        if(status < 0):
            Data.execute('DevLogErr($1,$2)', self.getNid(), message + 'status: %d'%(status))
            raise DevCOMM_ERROR

    def NI6683_close(self, Fds):
        print("\nCLOSING NI6683...")
        print('FD to Close: ', Fds)
        for fd in Fds:
            #print("Closing FD", fd)
            os.close(fd)
        print("NI6683 CLOSED!")
        return 0
    
    def NI6683_stop(self, activeFds):
        print("\nSTOPPING NI6683...")
        status = NI6683.niLib.nisync_abort_all_ftes(self.fd)
        if status == -1:
            return -1

        for fd in activeFds:
            print("Disabling future events for FD %i" % fd)
            status = NI6683.niLib.nisync_disable_future_time_events(fd)
            errorNumber = get_errno()
            message = os.strerror(errorNumber)
            if status == -1:
                print("Tried disabling future events for FD %i, failed with the errno: %i = %s" % (fd, get_errno(), message))
                return -1
            os.close(fd)
        
        os.close(self.fd)
        print("NI6683 STOPPED!")
        return 0   

    #Stop worker process if it is running
    def worker_check(self):
        if NI6683.ni6683WorkerDict:
            worker = NI6683.ni6683WorkerDict[self.nid]
            self.fd = NI6683.ni6683Fds[self.nid]
            if self.thread_alive(worker):
                self.debugPrint("PXI 6683 stop_worker")
                worker.stop()
                worker.join()

    # saves the information contained in the pulse file in the module variables
    def init(self):
        self.debugPrint('=================  PXI 6683 init ===============')
        if (self.DEVMODE == 1):
            print("WARNING: Developer mode active!")
        self.restoreInfo()
        self.reset_device()

        NI6683.ni6683RecorderDict[self.nid] = []
        curr_nanos = c_uint64()
        status = NI6683.niLib.nisync_get_time_ns(self.fd, byref(curr_nanos))

        if (self.DEVMODE != 1):
            # Checking if the moduled is synchronized with the PTP network
            curr_state = c_int(0)
            NI6683.niLib.nisync_get_ptpd_state(self.fd, byref(curr_state))
            if (curr_state.value != 2):
                Data.execute('DevLogErr($1,$2)', self.getNid(), "ERROR: MODULE NON SYNCHRONIZED WITH THE PTP NETWORK! EXITING...")
                raise DevBAD_PARAMETER # DA METTERE A POSTO

        try:
            NI6683.ni6683AbsStart = self.abs_start.data() # Trying to read the curr time from the ABS_START field
        except:
            print (" !!!!!!!!!!!!!!! PROBLEM IN RECOVERING ABSOLUTE TIME, CONTINUING WITH MODULE INTERNAL TIME !!!!!!!!!!!!!!!")
            NI6683.useInternalReference = True

        if (not NI6683.useInternalReference):
            print ("ABS START RETRIEVED: %f, INTERNAL TIME: %f"%(NI6683.ni6683AbsStart, curr_nanos.value))
            if NI6683.ni6683AbsStart - curr_nanos.value < 0:
                Data.execute('DevLogErr($1,$2)', self.getNid(), "ERROR: CURRENT TIME GREATER THAN ABSOLUTE TIME")
                raise DevBAD_PARAMETER 
            

        # if the trigger event is defined, the module will wait for it to be triggered
        try:
            NI6683.ni6683ModuleTriggerName = self.trig_event.data()
        except:
            NI6683.ni6683ModuleTriggerName = None
        
        for termName in NI6683.termNameDict.keys():
            termNameNid = termName+str(self.nid)
            # reading the shot relative start time
            try:
                NI6683.relTime = self.rel_start.data()
            except:
                emsg = 'Invalid relative start'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            # retreaving the terminal behavior
            try:
                mode = getattr(self, termName.lower()+'_mode').data()
            except:
                emsg = 'Invalid mode in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            NI6683.ni6683Modes[termNameNid] = mode

            # storing the terminal name in the ni6683RecorderDict
            if (mode == 'RECORDER RISING' or mode == 'RECORDER FALLING' or mode == 'RECORDER ANY'):
                NI6683.ni6683RecorderDict[self.nid].append(termName)

            # LOW PULSE initialization at level high mode
            if (mode == 'LOW PULSE'):
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot abort future time events for the LOW PULSE behavior ")
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot enable future time events ")
                # setting the terminal level to 1 before the trigger
                NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_HIGH))

            # HIGH PULSE initialization at level high mode
            if (mode == 'HIGH PULSE'):
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot abort future time events for the LOW PULSE behavior ")
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot enable future time events ")
                # setting the terminal level to 0 before the trigger
                NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_LOW))

            # HIGH initialization at level high mode
            if (mode == 'HIGH'):
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot abort future time events for the LOW PULSE behavior ")
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot enable future time events ")
                # setting the terminal level to 0 before the trigger
                NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_HIGH))

            # LOW initialization at level high mode
            if (mode == 'LOW'):
                status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot abort future time events for the LOW PULSE behavior ")
                status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                self.checkStatus(status, "Cannot enable future time events ")
                # setting the terminal level to 0 before the trigger
                NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_LOW))

            # retrieving the terminal frequency    
            try:
                freq = getattr(self, termName.lower()+'_frequency').data()
            except:
                emsg = 'Invalid frequency in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            NI6683.ni6683Frequencies[termNameNid] = freq
            periodNs = int(1000000000./float(freq))
            
            # retrieving the terminal duty cycle
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
            NI6683.ni6683DutyCycles[termNameNid] = dutyCycleNs

            # retrieving the terminal end time
            try:
                NI6683.ni6683TermEnds[termNameNid] = getattr(self, termName.lower()+'_end').data()
            except:
                emsg = 'Invalid end in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            # retrieving the terminal pulse length
            try:
                NI6683.ni6683PulseLengths[termNameNid] = getattr(self, termName.lower()+'_pulse_len').data()
            except:
                emsg = 'Invalid pulse length in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            # retrieving the terminal start time
            try:
                NI6683.ni6683TermStarts[termNameNid] = getattr(self, termName.lower()+'_start').data()
            except:
                emsg = 'Invalid start in ' + termName
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            
            if (mode == 'CLOCK'):
                #Stores terminal start times, end times and freqencies
                rangeEnd = NI6683.ni6683TermEnds[termNameNid]
                if rangeEnd <= NI6683.ni6683TermStarts[termNameNid]:
                    rangeEnd = None
                clockSource = Range(NI6683.ni6683TermStarts[termNameNid], rangeEnd, 1./NI6683.ni6683Frequencies[termNameNid])
                getattr(self, termName.lower()+'_clock_source').putData(clockSource)

        self.saveInfo()
        
    # By using the information gained in the init() phase, the NI6683 triggers the future events associated to each terminal
    def trigger(self):
        self.debugPrint('=================  PXI 6683 trigger ===============')
        #ERROR CHECKING
        if not NI6683.ni6683Modes: #This is created by init, so if it doesn't exist init wasn't run
            print("ERROR: Init not run first")
            return -1
        self.restoreInfo()
        self.worker_check()

        if (NI6683.useInternalReference): # if abs_start not found on the pulse file, puts the current module time in the abs_start field
            deltaT = 200 # ms
            curr_nanos = c_uint64()
            status = NI6683.niLib.nisync_get_time_ns(self.fd, byref(curr_nanos))
            self.abs_start.putData(Uint64(curr_nanos.value + deltaT * 1000000)) 

        curr_nanos = c_uint64()
        status = NI6683.niLib.nisync_get_time_ns(self.fd, byref(curr_nanos))
        print ("DEBUG -> INTERNAL TIMING: ", curr_nanos.value)
        NI6683.ni6683AbsTime = self.abs_start.data()
        NI6683.ni6683RelTime = self.rel_start.data()
        
        for termName in NI6683.termNameDict.keys():
            termNameNid = termName+str(self.nid)
            mode = NI6683.ni6683Modes[termNameNid]
            freq = NI6683.ni6683Frequencies[termNameNid]

            # in the DISABLED mode:
            #   - the terminal future time evets are disabled
            #   - the terminal timestamp triggers are disabled
            if mode == 'DISABLED':
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()

                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events in "DISABLED" mode')
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

            # in the CLOCK mode:
            #   - the terminal future time evets are enabled
            #   - the terminal tries to generate a clok in the period [startNs, endNs]
            #   - if a clock is already present, the terminal tries to replace it
            elif mode == 'CLOCK':
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()
                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events in "CLOCK" mode')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')

                startNs, endNs = self.getStartEnd(termName)
                periodNs = int(1000000000./float(freq))
                dutyCycle = NI6683.ni6683DutyCycles[termNameNid]

                print('Generating Clock: '+ str(self.termDict[termName]) + '  ' + str(startNs) + '  '+str(endNs)+'  ' + str(periodNs) + '  '+str(NI6683.ni6683DutyCycles[termNameNid]))
                status = NI6683.niLib.nisync_generate_clock_ns(c_int(self.termDict[termName]), c_uint64(int(startNs)),
                    c_uint64(int(endNs)), c_uint64(periodNs), c_uint64(dutyCycle))


                if status != 0:
                    print(os.strerror(get_errno()))
                    print("Clock already present, replacing it...")
                    status = NI6683.niLib.nisync_replace_clock_ns(c_int(self.termDict[termName]), c_uint64(int(startNs)),
                        c_uint64(int(endNs)), c_uint64(periodNs), c_uint64(dutyCycle)) 
                    if status != 0:
                        emsg = 'Error in replace clock for %s status = %d '%(termName, status)
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevCOMM_ERROR

            # in the HIGH PULSE mode:
            #   - the terminal timestamp triggers are disabled (PERCHE? riguardare!!! TODO)
            #   - the terminal generates a pulse in the period [startNs, endNS] (QUI USA generateFTEs... provare con metodo pulse TODO)
            elif mode == 'HIGH PULSE':###############################################################
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()

                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]),
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')

                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    print ("DEBUG -> PULSE AT: ", startNs[i])
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(int(self.termDict[termName])), c_uint64(int(startNs[i])), c_uint8(self.NISYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot generate future event at pulse start')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(int(self.termDict[termName])), c_uint64(int(endNs[i])), c_uint8(self.NISYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot generate future event at pulse end')

            # in the LOW PULSE mode:
            #   - the terminal timestamp triggers are disabled (PERCHE? riguardare!!! TODO)
            #   - the terminal generates a pulse in the period [startNs, endNS] (QUI USA generateFTEs... provare con metodo pulse TODO)        
            elif mode == 'LOW PULSE':###################################################
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()
                
                status = NI6683.niLib.nisync_timestamp_trigger_configuration(c_int(self.termDict[termName]),
                    byref(enabled), byref(activeEdge), byref(decimationCount))
                self.checkStatus(status, 'Cannot inquire timestamp triggers')
                if enabled.value != 0:
                    print('DISABLE TIMESTAMP for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_disable_timestamp_trigger(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot disable timestamp triggers')

                startNs, endNs = self.getStartPulse(termName)
                for i in range(0,len(startNs)):
                    if endNs[i] <= startNs[i]:
                        emsg = 'Start Time greater than End Time for ' + termName
                        Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                        raise DevBAD_PARAMETER
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(int(self.termDict[termName])), c_uint64(int(startNs[i])), c_uint8(self.NISYNC_LEVEL_LOW))
                    self.checkStatus(status, 'Cannot generate future event at pulse start')
                    status = NI6683.niLib.nisync_generate_fte_ns(c_int(int(self.termDict[termName])), c_uint64(int(endNs[i])), c_uint8(self.NISYNC_LEVEL_HIGH))
                    self.checkStatus(status, 'Cannot generate future event at pulse end')
            
            # in the HIGH mode:
            #   - the terminal timestamp triggers are disabled (PERCHE? riguardare!!! TODO)
            #   - the terminal future time events are enabled
            #   - the terminal FTEs are aborted (SICURI? TODO)
            #   - the terminal level is set to 1
            elif mode == 'HIGH':
                enabled = c_int8()
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

                self.checkStatus(status, 'Cannot inquire future events in "HIGH" mode')

                #time.sleep(10) # Starts at 18s
                
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')

                #time.sleep(10) # Starts at 8s

                # status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                # self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_HIGH))
                self.checkStatus(status, 'Cannot set terminal level')


            # in the LOW mode:
            #   - the terminal timestamp triggers are disabled (PERCHE? riguardare!!! TODO)
            #   - the terminal future time events are enabled
            #   - the terminal FTEs are aborted (SICURI? TODO)
            #   - the terminal level is set to 0
            elif mode == 'LOW':
                enabled = c_int8()
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
                self.checkStatus(status, 'Cannot inquire future events in "LOW" mode')
                if enabled.value == 0:
                    print('ENABLE FUTURE EVENT for ' + termName + ' fd: '+ str(self.termDict[termName]))
                    status = NI6683.niLib.nisync_enable_future_time_events(c_int(self.termDict[termName]))
                    self.checkStatus(status, 'Cannot enable future events')

                # status = NI6683.niLib.nisync_abort_all_ftes(c_int(self.termDict[termName]))
                # self.checkStatus(status, 'Cannot abort FTEs')
                status = NI6683.niLib.nisync_set_terminal_level(c_int(self.termDict[termName]), c_int(self.NISYNC_LEVEL_LOW))
                self.checkStatus(status, 'Cannot set terminal level')
            
            # in the RECORDER RISING mode:
            #   - the terminal future time events are disabled
            #   - the terminal timestamp triggers are disabled (AH FORSE PERCHE non si sa prima se erano falling o rising)
            #   - the terminal timestamp triggers are enabled on the rising edge
            elif mode == 'RECORDER RISING':
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()

                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events in "RECORDER RISING" mode')
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
            
            # in the RECORDER FALLING mode:
            #   - the terminal future time events are disabled
            #   - the terminal timestamp triggers are disabled (AH FORSE PERCHE non si sa prima se erano falling o rising)
            #   - the terminal timestamp triggers are enabled on the falling edge
            elif mode == 'RECORDER FALLING':
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()

                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events in "RECORDER FALLING" mode')
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

            # in the RECORDER RISING mode:
            #   - the terminal future time events are disabled
            #   - the terminal timestamp triggers are disabled (AH FORSE PERCHE non si sa prima se erano falling o rising)
            #   - the terminal timestamp triggers are enabled for any edge
            elif mode == 'RECORDER ANY': 
                enabled = c_int8()
                activeEdge = c_int()
                decimationCount = c_int()

                status = NI6683.niLib.nisync_future_time_events_configuration(c_int(self.termDict[termName]), byref(enabled))
                self.checkStatus(status, 'Cannot inquire future events in "RECORDER ANY" mode')
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
        
        
        # starting the AsynchStore thread
        self.debugPrint('=================  PXI 6683 start AsynchStore ===============')
        worker = self.AsynchStore()
        NI6683.ni6683WorkerDict[self.nid] = worker
        worker.daemon = True
        worker.configure(self.copy())
        worker.start()

    # closes all the opened terminals (WITHOUT ERASING THE FUTURE TIME EVENTS)
    def closeInfo(self):
        self.debugPrint('================= PXI 6683 closing all terminals ================')
        self.restoreInfo()
        Fds = []
        
        self.termDict = NI6683.ni6683Dicts[self.nid]
        for termName in NI6683.termNameDict.keys():
            Fds.append(self.termDict[termName])
        status = self.NI6683_close(Fds)
        if status == -1:
            print("PROBLEM WHILE CLOSING...")

    # opens the used terminals in the last session, stops their FTEs and closes them again
    def stop(self):
        self.debugPrint('================= PXI 6683 stop ================')
        self.closeInfo()
        self.worker_check()

        if self.nid in NI6683.ni6683Fds.keys():
            self.fd = NI6683.ni6683Fds[self.nid]
            del(NI6683.ni6683Fds[self.nid])

        activeFds = []

        try:
            boardId = self.board_id.data()
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

        self.termDict = NI6683.ni6683Dicts[self.nid]
        for termName in NI6683.termNameDict.keys():
            mode = getattr(self, termName.lower()+'_mode').data()
            if mode != 'DISABLED':
                term = NI6683.niLib.nisync_open_terminal(c_int(devType), c_int(boardId), c_int(NI6683.termNameDict[termName]), c_int(self.NISYNC_READ_NONBLOCKING))
                print("Terminal open: ", term)
                activeFds.append(term)

        
        status = self.NI6683_stop(activeFds)
        if status == -1:
            print("PROBLEM WHILE STOPPING...")
        print("Cleaning up...") #-----------------------------Tim Robinson added code
        for fd in activeFds:
            try:
                os.close(fd)
                print("Closed terminal FD" , fd) 
            except OSError as e:
                #print ("Terminal FD", fd, " already closed")
                pass

        try:    
            os.close(self.fd)
            print("Closed Device FD" , self.fd)
        except OSError as e:
            #print("Device FD" , self.fd, " already closed")
            pass
    # AsynchStore inner class, it handles the MDSEvents geneneration triggered by hardware
    class AsynchStore(Thread):
        class nisync_timestamp_nanos(Structure):
            _fields_ = [("edge", c_uint8),
                        ("nanos", c_uint64)]

        def configure(self, device):
            self.stopReq = False
            self.device = device
            self.poll = select.poll()
            self.nameDict = {}
            # READ_ONLY = select.POLLIN | select.POLLPRI | select.POLLHUP | select.POLLERR
            READ_ONLY = select.POLLIN | select.POLLPRI | select.POLLHUP

            for name in NI6683.ni6683RecorderDict[self.device.nid]:
                self.poll.register(NI6683.ni6683Dicts[self.device.nid][name], READ_ONLY)
                self.nameDict[NI6683.ni6683Dicts[self.device.nid][name]] = name
                print('REGISTRATO ', NI6683.ni6683Dicts[self.device.nid][name])

        def getRelTime(self, absTime):
            try:
                # print("absTime: " + str(absTime) + " self.device.abs_start.data(): " + str(self.device.abs_start.data()) + " self.device.rel_start.data(): " +str(self.device.rel_start.data()) )
                return float(absTime - self.device.abs_start.data())/1E9 + self.device.rel_start.data()
            except:
                emsg = 'Cannot convert absolute time to relative ' + str(self.device.fd)
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), emsg)
                raise DevBAD_PARAMETER

        def run(self):
            self.device.setTree(Tree(self.device.getTree().name, self.device.getTree().shot))
            self.device = self.device.copy()

            while not self.stopReq:
                readyFds = self.poll.poll(1000)
                for fdTuple in readyFds:
                    readyFd = fdTuple[0]
                    event = fdTuple[1]
                    print('EVENT: ' + str(fdTuple) + str(select.EPOLLIN))
                    if (event & select.EPOLLIN) == 0:
                        print('NO DATA')
                        return
                    if (event & select.EPOLLERR) != 0:
                        print ('POLL ERROR!!')
                        return
                    timestamp = self.nisync_timestamp_nanos(0,0)
                    status = NI6683.niLib.nisync_read_timestamps_ns(c_int(readyFd), byref(timestamp), c_int(1))
                    self.device.checkStatus(status, 'Cannot get current time')
                    termName = self.nameDict[readyFd]
                    recorderNid = getattr(self.device, termName.lower()+'_raw_events')
                    
                    eventRelTime = self.getRelTime(timestamp.nanos)
                    
                    # Store this event as a single-sample segment
                    recorderNid.makeSegment(
                        Float64(eventRelTime),                # segment start time
                        Float64(eventRelTime),                # segment end time (same, single event)
                        Float64Array([eventRelTime]),         # dimension: actual event time(s)
                        Float64Array([timestamp.nanos/1e9])   # value: absolute time in seconds
                    )

                    print ("DEBUG -> TIMESTAMP: " + str(timestamp.nanos))
                    print ("DEBUG -> TIMESTAMP REL: " + str(eventRelTime))
                    try:
                        eventNameNid = getattr(self.device, termName.lower()+'_event_name')
                        eventName = eventNameNid.data()
                        # Event.setevent(eventName, Uint64(eventRelTime))
                        Event.setevent(eventName, Float64(eventRelTime))
                    except:
                        pass
                    
        def stop(self):
            self.stopReq = True
