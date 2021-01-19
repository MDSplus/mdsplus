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
from MDSplus import Device, Data, Uint64, Event, Float64
from MDSplus.mdsExceptions import DevCOMM_ERROR, DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, byref, c_ulonglong, c_char_p
import os


class SOFT_TRIGGER(Device):
    """Generation of Events (with absolute time) at trigger occurrences"""
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':REL_START', 'type': 'numeric', 'value': -5},
             {'path': ':ABS_START', 'type': 'numeric', 'value': 0},
             {'path': ':SYNC_DEVICE', 'type': 'TEXT', 'value': 'DEFAULT'},
             {'path': ':ADVANCE_TIME', 'type': 'numeric', 'value': 0}]
    for chanIdx in range(16):
        parts.append({'path': '.CHANNEL_'+str(chanIdx+1), 'type': 'structure'})
        parts.append({'path': '.CHANNEL_'+str(chanIdx+1) +
                      ':MODE', 'type': 'text', 'value': 'DISABLED'})
        parts.append({'path': '.CHANNEL_'+str(chanIdx+1) +
                      ':EVENT_TIME', 'type': 'numeric'})
        parts.append({'path': '.CHANNEL_'+str(chanIdx+1) +
                      ':EVENT_NAME', 'type': 'text'})

    parts.append({'path': ':INIT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','PON',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'stop',head))",
                  'options': ('no_write_shot',)})

    syncDict = {'SYS': 'sys-conf.xml', '6682': 'nisync-6682.xml',
                '6683': 'nisync-6683h.xml', 'DEFAULT': "tcn-default.xml"}
    tcnLib = None

    def debugPrint(self, msg="", obj=""):
        print(self.name + ":" + msg, obj)

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

    def initLib(self):
        if SOFT_TRIGGER.tcnLib is None:
            SOFT_TRIGGER.tcnLib = CDLL("libtcn.so")
            try:
                print(self.sync_device.data())
                syncDevice = self.syncDict[self.sync_device.data()]
            except:
                emsg = 'Invalid Sync device'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER

            print(os.environ['TCN_CONFIG_PATH']+'/'+syncDevice)
            retVal = SOFT_TRIGGER.tcnLib.tcn_register_device(
                c_char_p(os.environ['TCN_CONFIG_PATH']+'/'+syncDevice))
            if retVal != 0:
                emsg = 'Cannot register device %s status = %d ' % (syncDevice)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR

            retVal = SOFT_TRIGGER.tcnLib.tcn_init()
            if retVal != 0:
                emsg = 'Cannot initialize TCN'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR

    def init(self):
        self.debugPrint('=================  SOFT_TRIGGER init ===============')

        self.initLib()
        eventNames = []
        eventTimes = []
        try:
            advanceTime = self.advance_time.data()
        except:
            advanceTime = 0
        for chan in range(16):
            try:
                mode = getattr(self, 'channel_'+str(chan+1)+'_mode').data()
            except:
                emsg = 'Invalid Mode for channel '+str(chan+1)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER
            if mode == 'ENABLED':
                try:
                    eventName = getattr(
                        self, 'channel_'+str(chan+1)+'_event_name').data()
                except:
                    emsg = 'Invalid event name for channel '+str(chan+1)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevBAD_PARAMETER
                try:
                    eventTime = getattr(
                        self, 'channel_'+str(chan+1)+'_event_time').data()
                except:
                    emsg = 'Invalid event time for channel '+str(chan+1)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevBAD_PARAMETER
                eventNames.append(eventName)
                eventTimes.append(eventTime)

        if len(eventNames) > 0:
            worker = self.AsynchEvent()
            worker.daemon = True
            worker.configure(self, eventNames, eventTimes, advanceTime)
            worker.start()

    def soft_init(self):
        self.debugPrint(
            '=================  SOFT_TRIGGER soft_init ===============')

        self.initLib()
        currentTime = c_ulonglong(0)
        retval = SOFT_TRIGGER.tcnLib.tcn_get_time(byref(currentTime))
        if retval != 0:
            emsg = 'Cannot get current time'
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevCOMM_ERROR
        self.abs_start.putData(Uint64(currentTime.value))
        self.abs_start.putData(Uint64(currentTime.value))
        return self.init()

    def stop(self):
        self.debugPrint('=================  SOFT_TRIGGER stop ===============')
        if SOFT_TRIGGER.tcnLib is None:
            SOFT_TRIGGER.tcnLib = CDLL("libtcn.so")
        retval = SOFT_TRIGGER.tcnLib.tcn_wakeup()
        if retval != 0:
            emsg = 'Cannot wake TCN'
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
            raise DevCOMM_ERROR

    class AsynchEvent(Thread):

        def configure(self, device, eventNames, eventTimes, advanceTime):
            self.device = device
            self.advanceTime = advanceTime
            self.stopReq = False
            self.eventDict = {}
            for chan in range(len(eventNames)):
                try:
                    self.eventDict[eventTimes[chan]].append(eventNames[chan])
                except:
                    self.eventDict[eventTimes[chan]] = [eventNames[chan]]
            self.evTimes = list(set(eventTimes))
            self.evTimes.sort()
            print('EV TIMES: ', self.evTimes)

        def run(self):
            for evIdx in range(len(self.evTimes)):
                currAbsTime = self.device.getAbsTime(self.evTimes[evIdx])
                currAbsTime = currAbsTime - self.advanceTime * 1000000000
                retval = SOFT_TRIGGER.tcnLib.tcn_wait_until(
                    c_ulonglong(currAbsTime))
                if retval == 0:
                    for eventName in self.eventDict[self.evTimes[evIdx]]:
                        print('EVENT ' + eventName + ' AT ' +
                              str(self.evTimes[evIdx]))
                        Event.setevent(eventName, Float64(self.evTimes[evIdx]))
