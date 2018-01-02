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

from MDSplus import mdsExceptions, Device, Data, Range, makeArray
from threading import Thread
from numpy import array
from ctypes import CDLL, byref, c_int
from time import sleep
try:
    DIO4 = __import__('DIO4', globals(), level=1).DIO4
except:
    DIO4 = __import__('DIO4', globals()).DIO4

class DIO2_ENCDEC(Device):
    """INCAA DIO2 Decoder/Encoder channels Timing Module"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':SW_MODE', 'type':'text', 'value':'LOCAL'},
        {'path':':IP_ADDR', 'type':'text'},
        {'path':':COMMENT', 'type':'text'},
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'INTERNAL'},
        {'path':':REC_START_EV', 'type':'text','state':'0'},
        {'path':':REC_EVENTS', 'type':'text'},
        {'path':':REC_TIMES', 'type':'numeric'},
        {'path':':SYNCH', 'type':'text', 'value':'NO'},
        {'path':':SYNCH_EVENT', 'type':'text'}]


    for i in range(8):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:FUNCTION'%(i+1), 'type':'text', 'value':'CLOCK'})
        parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i+1), 'type':'text', 'value':'SOFTWARE'})
        parts.append({'path':'.CHANNEL_%d:EVENT'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d:CYCLIC'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d:DELAY'%(i+1), 'type':'numeric', 'value':0.})
        parts.append({'path':'.CHANNEL_%d:DURATION'%(i+1), 'type':'numeric', 'value':1.})
        parts.append({'path':'.CHANNEL_%d:FREQ_1'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:FREQ_2'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_1'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_2'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:DUTY_CYCLE'%(i+1), 'type':'numeric', 'value':50})
        parts.append({'path':'.CHANNEL_%d:TRIGGER'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:CLOCK'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_1'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_2'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:COMMENT'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d:TERMINATION'%(i+1), 'type':'text', 'value':'NO'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV1'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TERMINATION'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:EDGE'%(i+1), 'type':'text', 'value':'RISING'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV2'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TERMINATION'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:EDGE'%(i+1), 'type':'text', 'value':'RISING'})
    del(i)

    parts.append({'path':'.OUT_EV_SW', 'type':'structure'})
    parts.append({'path':'.OUT_EV_SW:NAME', 'type':'text'})
    parts.append({'path':'.OUT_EV_SW:CODE', 'type':'numeric'})
    parts.append({'path':'.OUT_EV_SW:TIME', 'type':'numeric', 'value':0})

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':RESET_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','RESET',50,None),Method(None,'reset',head))",
        'options':('no_write_shot',)})

    mainLib = None
    handles = {}
    workers = {}


# INIT
    def init(self):
        print('INIT')

# Board ID
        try:
            boardId = self.board_id.data()
            print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# Software Mode
        try:
            swMode = self.sw_mode.data()
            print('swMode: ' + str(swMode))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            print('IP_ADDR: ' + ipAddr)
# Clock Source
        #clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1, 'EXTERNAL':2}
        clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1}
        try:
            clockSource = clockSourceDict[self.clock_source.data()]
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK_SOURCE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        print('CLOCK_SOURCE: ' + self.clock_source.data() + ' - ID: ' + str(clockSource))
# Recorder Event
        recStartEv = 0
        if getattr(self, 'rec_start_ev').isOn():
            try:
                recStartEv = Data.execute('TimingDecodeEvent($1)', self.rec_start_ev)
            except:
                #recStartEv = -1
                recStartEv = 0
        print('REC_START_EV: ' + str(recStartEv))
# Synch events
        synchEvents = []
        try:
            synch = self.synch.data()
            print('synch: ' + synch)
            if synch == 'YES':
                synchFlag = 1
                synchEvent = self.synch_event.data()
                synchEvSize =Data.execute('size($1)', synchEvent)
                print('synch event size: ' + str(synchEvSize))
                if synchEvSize == 1:
                    synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent))
                else:
                    for i in range(synchEvSize):
                        synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent[i]))
                    del i
            else:
                synchFlag = 0
                synchEvents.append(-1)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SYNCH_EVENT')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# HW Init
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO2_ENCDECHWInit(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, clockSource, recStartEv, makeArray(synchEvents))
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW initialization')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO2_ENCDECHWInit(0, $1, $2, $3, $4)", boardId, clockSource, recStartEv, makeArray(synchEvents))
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW initialization')
                raise mdsExceptions.TclFAILED_ESSENTIAL

# SW EVENT Configuration Check
        huge = float( Data.execute('HUGE(0.)') )
        if getattr(self, 'out_ev_sw').isOn():
            print('SW EVENT IS ON')
            try:
                evName = self.out_ev_sw_name.data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            if evCode != 0:
                setattr(self,'out_ev_sw_code', evCode)
            else:
                try:
                    evCode = int( self.out_ev_sw_code.data() )
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
                print('Leggo evTime: ' , nodePath)
                #evTime = getattr(self, 'out_ev_sw_time').data()
                evTime = float( self.out_ev_sw_time.data() )
                print('evTime: ', evTime)
            except:
                print('Perche va in exception')
                evTime = huge
            if evTime == huge:
                print('Perche non va')
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software event')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            #print 'NAME: ' + evName
            #print 'PATH: ' + nodePath
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            print('SW EVENT IS OFF')

# Channels Setup
        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                print('Channel %d ON'%(c+1))
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                print('FUNCTION: ' + function)

                if function != 'ENCODER':
                    channelMask = channelMask | (1 << c)
                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO2_ENCDECHWSetIOConnectionChan(0, $1, $2)", $1,$2)', boardId, c);
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW IO chanel setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO2_ENCDECHWSetIOConnectionChan(0, $1, $2)", boardId, c)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW IO chanel setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL

#Clock Generation
                if function == 'CLOCK':
                    try :
                        frequency = float( getattr(self,'channel_%d_freq_1'%(c+1)).data() )
                        #print 'FREQ: ' + str(frequency)
                        if frequency <= 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid clock frequency parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        dutyCycle = float( getattr(self,'channel_%d_duty_cycle'%(c+1)).data() )
                        #print 'Duty Cycle: ' + str(dutyCycle)
                        if dutyCycle <= 0 or dutyCycle > 100 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid clock duty cycle parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        #Channel termination flags on DIO2 not yet implemented
                        #terminationDict = {'NO':0, 'YES':1}
                        #termination = getattr(self, 'channel_%d_termination'%(c+1)).data()
                        #terminationCode = terminationDict[termination]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    if swMode == 'REMOTE':
                        #status = Data.execute('MdsValue("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4, $5)', boardId, c, frequency, dutyCycle, terminationCode)
                        status = Data.execute('MdsValue("DIO2HWSetClockChan(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, c, frequency, dutyCycle)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW clock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        #status = Data.execute("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", boardId, c, frequency, dutyCycle, terminationCode)
                        status = Data.execute("DIO2HWSetClockChan(0, $1, $2, $3, $4)", boardId, c, frequency, dutyCycle)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                    period = int((1. / frequency) / 1E-7 + 0.5) * 1E-7;
                    #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE("+*+","+*+","+str(period)+")'))
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period))
                    #self.clock.putData(Range(None, None, period))
#Trigger Generation
                elif function == 'PULSE':
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                print('PULSE event: ', event)
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    #print 'eventTime: ' + str(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                    try:
                                        eventTime = float( getattr(self,'channel_%d_trigger'%(c+1)).data() )
                                    except:
                                        Data.execute('DevLogErr($1, $2)', self.getNid(), 'Cannot associate a time to event ' + str(event) + ' for channel %d'%(c+1))
                                        raise mdsExceptions.TclFAILED_ESSENTIAL

                                print('PULSE eventCodes: ' + str(eventCodes))
                                getattr(self,'channel_%d_trigger'%(c+1)).data()
                            except:
                                Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid event for pulse channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                        cyclicDict = {'NO':0, 'YES':1}
                        levelDict = {'LOW':0, 'HIGH':1}
                        try:
                             cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
                             #print cyclic
                        except:
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid pulse cyclic parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        try:
                             initLev1 = levelDict[getattr(self,'channel_%d_init_lev_1'%(c+1)).data()]
                             #print initLev1
                        except:
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid pulse Lev1 parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        try:
                            initLev2 = levelDict[getattr(self,'channel_%d_init_lev_2'%(c+1)).data()]
                            #print initLev2
                        except:
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid pulse Lev2 parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        #print duration
                        if duration < 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid pulse duration parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL


                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        #print delay
                        if delay < 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid pulse delay parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        #Channel termination flags on DIO2 not yet implemented
                        #terminationDict = {'NO':0, 'YES':1}
                        #termination = getattr(self, 'channel_%d_termination'%(c+1)).data()
                        #terminationCode = terminationDict[termination]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Pulse parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO2HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)", $1,$2,$3,$4,$5,$6,$7,$8,$9)', boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes))
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW pulse setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO2HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)",boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes))
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW pulse setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path
                        #print trigger_1
                        #print trigger_2
                        getattr(self, 'channel_%d_trigger_1'%(c+1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2'%(c+1)).putData(Data.compile(trigger_2))

                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write Pulse parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
#Gated Clock Generation
                elif function == 'GCLOCK':
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                print('event: ' + str(event))
                                print('event size: ' + str(eventSize))
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))

                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot configure GClock channel with more than one event: channel %d'%(c+1))
                                    raise mdsExceptions.TclFAILED_ESSENTIAL
                                    """
                                    for i in range(eventSize):
                                        print eA[i]
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                    try:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    except:
                                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot associate a time to event ' + str(event) + ' for channel %d'%(c+1))
                                        raise mdsExceptions.TclFAILED_ESSENTIAL
                                    """
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                        frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        if frequency <= 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid gated clock frequency parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        print('duration: ', duration)
                        if duration < 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid gated clock duration parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL


                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        if delay < 0 :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid gated clock delay parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        if dutyCycle < 0 or dutyCycle > 100  :
                            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid gated clock duty cycle parameter for channel %d'%(c+1))
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                        #Cycling function non yet implemented gated Clock
                        #cyclicDict = {'NO':0, 'YES':1}
                        #cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]

                        #Channel termination flag on DIO2 not yet implemented
                        #terminationDict = {'NO':0, 'YES':1}
                        #termination = getattr(self, 'channel_%d_termination'%(c+1)).data()
                        #terminationCode = terminationDict[termination]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Gated Clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    print(boardId, c, trigModeCode, frequency, delay, duration, eventCodes[0])

                    if swMode == 'REMOTE':
                        #status = Data.execute('MdsValue("DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)", $1,$2,$3,$4,$5,$6,$7,$8)', boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle)
                        status = Data.execute('MdsValue("DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7)", $1,$2,$3,$4,$5,$6,$7)', boardId, c, trigModeCode, frequency, delay, duration, eventCodes[0])
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        #status = Data.execute("DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)",boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle)
                        status = Data.execute("DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7)",boardId, c, trigModeCode, frequency, delay, duration, eventCodes[0])

                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW GClock setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path

                        #trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        #trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()

                        period = int((1. / frequency) / 1E-7 + 0.5) * 1E-7;
                        #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE('+trig1+','+trig2+','+str(period)+')'))
                        getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(Data.compile(trigger_1), Data.compile(trigger_2), period))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write GCLOCK parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
#Dual speed Clock Generation
                elif function == 'DCLOCK':
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                #eventSize = Data.execute('size($1)', event)
                                #print eventSize
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    #print eventTime
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot configure DClock channel with more than one event: channel %d'%(c+1))
                                    raise mdsExceptions.TclFAILED_ESSENTIAL
                                    """
                                    for i in range(eventSize):
                                        print eA[i]
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                    try:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    except:
                                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot associate a time to event ' + event + ' for channel %d'%(c+1))
                                        raise mdsExceptions.TclFAILED_ESSENTIAL
                                    """
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data()
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        #Channel termination flag on DIO2 not yet implemented
                        #terminationDict = {'NO':0, 'YES':1}
                        #termination = getattr(self, 'channel_%d_termination'%(c+1)).data()
                        #terminationCode = terminationDict[termination]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Dual Speed Clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    if swMode == 'REMOTE':
                        #status = Data.execute('MdsValue("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)", $1,$2,$3,$4,$5,$6,$7,$8,$9)', boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), terminationCode)
                        status = Data.execute('MdsValue("DIO2HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)", $1,$2,$3,$4,$5,$6,$7,$8)', boardId, c, trigModeCode, freq1, freq2, delay, duration, eventCodes[0])
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        #status = Data.execute("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)",boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), terminationCode)
                        status = Data.execute("DIO2HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)",boardId, c, trigModeCode, freq1, freq2, delay, duration, eventCodes[0])
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW DClock setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path
                        getattr(self, 'channel_%d_trigger_1'%(c+1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2'%(c+1)).putData(Data.compile(trigger_2))

                        period1 = int((1. / freq1) / 1E-7 + 0.5) * 1E-7
                        period2 = int((1. / freq2) / 1E-7 + 0.5) * 1E-7

                        trig1_path = getattr(self,'channel_%d_trigger_1'%(c+1)).getFullPath()
                        trig2_path = getattr(self,'channel_%d_trigger_2'%(c+1)).getFullPath()
                        setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE([-1E6,'+trig1_path+','+trig2_path+'],['+trig1_path+','+trig2_path+',1E6],['+str(period1)+','+str(period2)+','+str(period1)+'])'))


                        #trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        #trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE([-1E6,'+trig1+','+trig2+'],['+trig1+','+trig2+',1E6],['+str(period1)+','+str(period2)+','+str(period1)+'])'))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write DCLOCK parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                elif function == 'ENCODER':
                    for e in range(2):
                        if getattr(self, 'channel_%d_out_ev%d'%(c+1, e+1)).isOn():
                            try:
                                evName = getattr(self, 'channel_%d_out_ev%d_name'%(c+1, e+1)).data()
                            except:
                                evName = ''
                            if evName != '':
                                evCode = Data.execute('TimingDecodeEvent($1)', evName)
                            else:
                                evCode = 0
                            print('evCode: ' + str(evCode))
                            if evCode != 0:
                                nodePath = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).getFullPath()
                                setattr(self,'channel_%d_out_ev%d_code'%(c+1, e+1), evCode)
                            else:
                                try:
                                    evCode = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).data()
                                except:
                                    evCode = 0
                            if evCode == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for channel %d'%(c+1+e))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                            try:
                                evTime = float(getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).data())
                            except:
                                evTime = float(huge)
                            print(" 1 event time ", evTime)
                            print(" 2 huge ", huge)
                            if evTime == huge:
                                print("3 Perche entra qui")
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for channel %d'%(c+1+e))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            nodePath = getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).getFullPath()
                            print('evName: ', evName)
                            status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
                            if status == -1:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register event time')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            terminationDict = {'NO':0, 'YES':1}
                            try:
                                nodePath = getattr(self, 'channel_%d_out_ev%d_termination'%(c+1, e+1)).getFullPath()
                                #print nodePath
                                termination = getattr(self, 'channel_%d_out_ev%d_termination'%(c+1, e+1)).data()
                                terminationCode = terminationDict[termination]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event termination specification for channel %d'%(c+1+e))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                            #Event Edge flags non yet implemented in DIO2 Channel
                            #evEdgeDict = {'FALLING':0, 'RISING':1}
                            #try:
                            #    evEdge = getattr(self, 'channel_%d_out_ev%d_edge'%(c+1, e+1)).data()
                            #    evEdgeCode = evEdgeDict[evEdge]
                            #except:
                            #    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event edge specification for channel %d'%(c+1))
                            #    raise mdsExceptions.TclFAILED_ESSENTIAL
                        else:
                            #Also if channel is off channel input configuration must be done
                            print('Event channel %d OFF'%(2*c+1+e))
                            terminationCode = 0
                            evCode = 0;
                        print('Event channel: ' + str(c))
                        realChannel = 2*c+1+e
                        print('Event real channel: ' + str(realChannel))
                        print('Event code: ' + str(evCode))
                        if swMode == 'REMOTE':
                            status = Data.execute('MdsValue("DIO2_ENCDECHWInitChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4,$5)', boardId, clockSource, realChannel, evCode, terminationCode)
                            if status == 0:
                                Data.execute('MdsDisconnect()')
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW set event setup.')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        else:
                            status = Data.execute("DIO2_ENCDECHWInitChan(0, $1, $2, $3, $4, $5)", boardId, clockSource, realChannel, evCode, terminationCode)
                            if status == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW set event setup')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                #If Channel is OFF for DIO2 module compatibility with DIO2 driver odd channel is configure
                #as input even as output
                if swMode == 'REMOTE':
                    status = Data.execute('MdsValue("DIO2_ENCDECHWSetIOConnectionChan(0, $1, $2)", $1,$2)', boardId, c);
                    if status == 0:
                        Data.execute('MdsDisconnect()')
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW IO chanel setup. See CPCI console for details')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                else:
                    status = Data.execute("DIO2_ENCDECHWSetIOConnectionChan(0, $1, $2)", boardId, c)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW IO chanel setup')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                print('Channel %d OFF'%(c+1))
            """
                External clock source sytnchronization not yet implemented in DIO2 Module
                elif function == 'CLOCK SOURCE + CLOCK':
                    try :
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data() # Frequenza del clock esterno
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data() # Frequenza del clock da generare
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid CLOCK SOURCE + CLOCK parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetExternalClockChan(0, $1, $2, $3, $4, $5, $6)", $1,$2,$3,$4,$5,$6)', boardId, c, freq1, freq2, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW clock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWSetExternalClockChan(0, $1, $2, $3, $4, $5, $6)", boardId, c, freq1, freq2, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                # DEVO SCRIVERE ANCHE freq1 DA QUALCHE PARTE ??????
                    period = long((1. / freq2) / 1E-7 + 0.5) * 1E-7;
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period))
                """
# End Channels Setup
        print("END CHANNEL SETUP")
        if swMode == 'REMOTE':
            #status = Data.execute('MdsValue("DIO4HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, synchFlag)
            status = Data.execute('MdsValue("DIO2HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, synchFlag)
            Data.execute('MdsDisconnect()')
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO2 device. See CPCI console for details')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            #print " Start DIO2", boardId, channelMask, synchFlag
            #status = Data.execute('MdsValue("DIO4HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, synchFlag)
            status = Data.execute("DIO2HWStartChan(0, $1, $2, $3)", boardId, channelMask, synchFlag)
            #print " End Start DIO2----"
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO2 Device')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        print("===== End py INIT ")

        return



# reset
    def reset(self):
        print('reset')
# Board ID
        try:
            boardId = self.board_id.data()
            #print 'BOARD_ID: ' + str(boardId)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            #print 'IP_ADDR: ' + ipAddr
#HW Reset
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO2HWReset($1)", $1)', boardId)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr', self.nid, 'Cannot execute remote HW reset. See CPCI console for details')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO2HWReset($1)", boardId)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW reset')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        if swMode == 'REMOTE':
            Data.execute('MdsDisconnect()')
        return



    def store(self):
        print('store')

        if not getattr(self, 'rec_start_ev').isOn():
            Data.execute('DevLogErr($1, $2)', self.nid, 'Events recording is not enabled')
            raise mdsExceptions.TclFAILED_ESSENTIAL

# Board ID
        try:
            boardId = self.board_id.data()
            #print 'BOARD_ID: ' + str(boardId)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                raise mdsExceptions.TclFAILED_ESSENTIAL
#HW
        recEvents = []
        recTimes = []

        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                try:
                    recEvents = Data.execute('MdsValue("DIO2HWGetRecEvents(0, $1)", $1)', boardId)
                    recTimes = Data.execute('MdsValue("_DIO2_rec_times")')
                    recEventNum  = Data.execute('MdsValue("size(_DIO2_rec_times)")')
                except:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO2HWGetRecEvents')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            try:
                recEvents = Data.execute("DIO2HWGetRecEvents(0, $1)", boardId)
                try:
                    recEventNum  = Data.execute("size(_DIO2_rec_times)")
                except:
                    recEventNum  = 0
                print("NUM   ", recEventNum)
                print("EVENTS ",recEvents)
                recTimes = Data.execute("_DIO2_rec_times")
                print("TIMES ", recTimes)
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO2HWGetRecEvents')
                raise mdsExceptions.TclFAILED_ESSENTIAL;

        print("recEventNum ", recEventNum)

        if recEventNum > 0 :
            self.rec_events.putData(recEvents)

            try:
                recStartEv = getattr(self, 'rec_start_ev').data()
            except:
                recStartEv = ''
            if recStartEv != '':
                recStartTime = Data.execute('TimingGetEventTime($1)', recStartEv)
            else:
                recStartTime = 0
            print("rec_times ", recStartTime + recStartTime)
            self.rec_times.putData(recTimes + recStartTime)

        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                #print 'FUNCTION: ' + function
                if function == 'PULSE' or function == 'GCLOCK' or function == 'DCLOCK':
                    try:
                        if swMode == 'REMOTE':
                            phases = Data.execute('MdsValue("DIO2HWGetPhaseCount(0, $1, $2)", $1,$2)', boardId, c)
                        else:
                            phases = Data.execute("DIO2HWGetPhaseCount(0, $1, $2)",boardId, c)
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot get phase count')
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    #print phases
                    trigPath = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                    trig1 = trigPath + '+' + phases[0]
                    trig2 = trigPath + '+' + phases[1]
                    try:
                        setattr(self, 'channel_%d_trig_1'%(c+1), Data.compile(trig1))
                        setattr(self, 'channel_%d_trig_2'%(c+1), Data.compile(trig2))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write trigger parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            Data.execute('MdsDisconnect()')
        return

    def trigger(self):
        print('trigger')
# Board ID
        try:
            boardId = self.board_id.data()
            #print 'BOARD_ID: ' + str(boardId)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1}
        try:
            clockSourceDict[self.clock_source.data()]
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK_SOURCE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        #print 'CLOCK_SOURCE: ' + self.clock_source.data() + ' - ID: ' + str(clockSource)


#HW TRIGGER
        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if (function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK'):
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeDict[trigMode]
                        if trigMode == 'SOFTWARE':
                            channelMask = channelMask | (1 << c)
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid trigger mode')
                        raise mdsExceptions.TclFAILED_ESSENTIAL

        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO2_ENCDECHWTrigger(0, $1, $2)", $1, $2)', boardId, channelMask)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr', self.nid, 'Cannot execute remote trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO2_ENCDECHWTrigger(0, $1, $2)", boardId, channelMask)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute trigger')
                raise mdsExceptions.TclFAILED_ESSENTIAL

#SW EVENT parameter check execute in init action too
        huge = Data.execute('HUGE(0.)')
        if getattr(self, 'out_ev_sw').isOn():
            try:
                evName = getattr(self, 'out_ev_sw_name').data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            if evCode != 0:
                setattr(self,'out_ev_sw_code', evCode)
            else:
                try:
                    evCode = getattr(self, 'out_ev_sw_code').data()
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                evTime = getattr(self, 'out_ev_sw_time').data()
                print("evTime: ",evTime)
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software event')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            if swMode == 'REMOTE':
                status = Data.execute('MdsValue("DIO2_ENCDECHWEventTrigger(0, $1, $2)", $1,$2)', boardId, evCode)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                status = Data.execute("DIO2_ENCDECHWEventTrigger(0, $1, $2)", boardId, evCode)
                if status == 0:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
        """
        Il canale non capisco perche debba essere generato un veneto software con codice 0
        else:
            print 'Sofware Event OFF'
            evCode = 0;
            if swMode == 'REMOTE':
                status = Data.execute('MdsValue("DIO2_ENCDECHWEventTrigger(0, $1, $2)", $1,$2)', boardId, evCode)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                #print "nodePath: ",nodePath , evCode
                status = Data.execute("DIO2_ENCDECHWEventTrigger(0, $1, $2)", boardId, evCode)
                if status == 0:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
        """

        if swMode == 'REMOTE':
            Data.execute('MdsDisconnect()')
        return



###################################################
    def saveInfo(self):
        DIO4.handles[self.nid] = self.handle

    def restoreInfo(self):
        if DIO4.mainLib is None:
            DIO4.mainLib = CDLL("libDIO4.so")
            print('carico la libreria')

        if self.nid in DIO4.handles.keys():
            self.handle = DIO4.handles[self.nid]
            print('RESTORE INFO HANDLE TROVATO')
        else:
            print('RESTORE INFO HANDLE NON TROVATO')
            try:
                boardId = self.board_id.data()
                print(boardId)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid BOARD_ID')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                DIO4.mainLib.DIO4_InitLibrary()
                status = DIO4.mainLib.DIO4_Open(c_int(boardId), byref(c_int(self.handle)))
                print(status)
                print(self.handle)
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return

    def removeInfo(self):
        del(DIO4.handles[self.nid])

    def start_store(self):
        print('START STORE')
        self.restoreInfo()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.configure(self, self.handle)
        self.saveWorker()
        self.worker.start()
        return

    def stop_store(self):
        print('STOP STORE')
        self.restoreWorker()
        self.worker.stop()
        return

    class AsynchStore(Thread):
        def __init__(self):
            self.stopReq = False

        def configure(self, device, handle):
            self.device = device
            self.handle = handle

        def run(self):
            while not self.stopReq:
                print('RUN')
                sleep(5)

        def stop(self):
            self.stopReq = True

###################################Worker Management
    def saveWorker(self):
        DIO4.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in DIO4.workers.keys():
            self.worker = DIO4.workers[self.nid]
        else:
            print('Cannot restore worker!!')
########################AsynchStore class
