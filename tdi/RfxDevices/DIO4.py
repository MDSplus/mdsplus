from MDSplus import mdsExceptions, Device, Data, Range, makeArray
from threading import Thread
from numpy import array
from ctypes import CDLL, byref, c_int
from time import sleep


class DIO4(Device):
    """INCAA DIO4 Timing Module"""
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

        parts.append({'path':'.CHANNEL_%d:OUT_EV1'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TERM'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:EDGE'%(i+1), 'type':'text', 'value':'RISING'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV2'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TERM'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:EDGE'%(i+1), 'type':'text', 'value':'RISING'})
    del(i)

    parts.append({'path':'.OUT_EV_SW', 'type':'structure'})
    parts.append({'path':'.OUT_EV_SW:NAME', 'type':'text'})
    parts.append({'path':'.OUT_EV_SW:CODE', 'type':'numeric'})
    parts.append({'path':'.OUT_EV_SW:TIME', 'type':'numeric', 'value':0})

    parts.append({'path':'.CHANNEL_1:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_2:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_3:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_4:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_5:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_6:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_7:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_8:TERM', 'type':'text', 'value':'NO'})

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'INIT',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'STORE',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':RESET_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','RESET',50,None),Method(None,'RESET',head))",
        'options':('no_write_shot',)})

    mainLib = None
    handles = {}
    workers = {}


# INIT
    def INIT(self):
        if Device.debug: print('INIT')

# Board ID
        try:
            boardId = self.board_id.data()
            if Device.debug: print('BOARD_ID: ' + str(boardId))
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
            if Device.debug: print('IP_ADDR: ' + ipAddr)
# Clock Source
        clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1, 'EXTERNAL':2}
        try:
            clockSource = clockSourceDict[self.clock_source.data()]
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK_SOURCE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if Device.debug: print('CLOCK_SOURCE: ' + self.clock_source.data() + ' - ID: ' + str(clockSource))
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
                synchEvent = self.synch_event.data()
                synchEvSize =Data.execute('size($1)', synchEvent)
                if Device.debug: print('synch event size: ' + str(synchEvSize))
                if synchEvSize == 1:
                    synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent))
                else:
                    for i in range(synchEvSize):
                        synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent[i]))
                    del i
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SYNCH_EVENT')
            raise mdsExceptions.TclFAILED_ESSENTIAL
# HW Init
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO4HWInit(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, clockSource, recStartEv, makeArray(synchEvents))
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW initialization')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO4HWInit(0, $1, $2, $3, $4)", boardId, clockSource, recStartEv, makeArray(synchEvents))
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW initialization')
                raise mdsExceptions.TclFAILED_ESSENTIAL
# Channels Setup
        channelMask = 0
        huge = Data.execute('HUGE(0)')


# SW EVENT Configuration Check
        huge = float( Data.execute('HUGE(0.)') )
        if getattr(self, 'out_ev_sw').isOn():
            if Device.debug: print('SW EVENT IS ON')
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
                    evCode = int( self.out_ev_sw_code.data() )
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                if Device.debug: print('LEGGO evTime')
                evTime = getattr(self, 'out_ev_sw_time').data()
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            if Device.debug: print('NAME: ' + evName)
            if Device.debug: print('PATH: ' + nodePath)
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if Device.debug: print('FATTO')
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            if Device.debug: print('SW EVENT IS OFF')






        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if Device.debug: print('FUNCTION: ' + function)
                if function == 'CLOCK': #Clock Generation
                    try :
                        frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        print('FREQ: ' + str(frequency))
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()

                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL


                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4, $5)', boardId, c, frequency, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW clock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", boardId, c, frequency, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL

                    period = int((1. / frequency) / 1E-7 + 0.5) * 1E-7;
                    #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE("+*+","+*+","+str(period)+")'))
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period))

                    #self.clock.putData(Range(None, None, period))

                elif function == 'PULSE':   #Trigger Generation
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eventSize = Data.execute('size($1)', event)
                                print('event: ' + str(event))
                                print('event size: ' + str(eventSize))
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    if Device.debug: print('eventTime: ' + str(eventTime))
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                if Device.debug: print('eventCodes: ' + str(eventCodes))
                                getattr(self,'channel_%d_trigger'%(c+1)).data()
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'error events')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        cyclicDict = {'NO':0, 'YES':1}
                        levelDict = {'LOW':0, 'HIGH':1}
                        cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
                        if Device.debug: print(cyclic)
                        initLev1 = levelDict[getattr(self,'channel_%d_init_lev_1'%(c+1)).data()]
                        if Device.debug: print(initLev1)
                        initLev2 = levelDict[getattr(self,'channel_%d_init_lev_2'%(c+1)).data()]
                        if Device.debug: print(initLev2)
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        if Device.debug: print(duration)
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        if Device.debug: print(delay)
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]


                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Pulse parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL


                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9,$10)", $1,$2,$3,$4,$5,$6,$7,$8,$9,$10)', boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW pulse setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW pulse setup')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path
                        if Device.debug: print(trigger_1)
                        if Device.debug: print(trigger_2)
                        getattr(self, 'channel_%d_trigger_1'%(c+1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2'%(c+1)).putData(Data.compile(trigger_2))

                        #setattr(self, 'channel_%d_trigger_1'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()))
                        #setattr(self, 'channel_%d_trigger_2'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write Pulse parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL



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
                                #eventSize = Data.execute('size($1)', event)
                                if Device.debug: print(eventSize)
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        if Device.debug: print(eA[i])
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        if Device.debug: print('duration: ' + str(duration))
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        cyclicDict = {'NO':0, 'YES':1}
                        cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]


                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Gated Clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)", $1,$2,$3,$4,$5,$6,$7, $8, $9, $10)', boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle, cyclic, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle, cyclic, evTermCode)
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
                                print(eventSize)
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data()
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        print(eA[i])
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data()
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Dual Speed Clock parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)", $1,$2,$3,$4,$5,$6,$7,$8,$9)', boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)",boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), evTermCode)
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
                            if Device.debug: print('evCode: ' + str(evCode))
                            if evCode != 0:
                                nodePath = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).getFullPath()
                                if Device.debug: print(nodePath)
                                setattr(self,'channel_%d_out_ev%d_code'%(c+1, e+1), evCode)
                            else:
                                try:
                                    evCode = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).data()
                                except:
                                    evCode = 0
                            if evCode == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                            try:
                                evTime = getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).data()
                            except:
                                evTime = huge
                            if evTime == huge:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            nodePath = getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).getFullPath()
                            status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
                            if status == -1:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register event time')
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            evTermDict = {'NO':0, 'YES':1}
                            try:
                                evTerm = getattr(self, 'channel_%d_out_ev%d_term'%(c+1, e+1)).data()
                                evTermCode = evTermDict[evTerm]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event termination specification for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL
                            evEdgeDict = {'FALLING':0, 'RISING':1}
                            try:
                                evEdge = getattr(self, 'channel_%d_out_ev%d_edge'%(c+1, e+1)).data()
                                evEdgeCode = evEdgeDict[evEdge]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event edge specification for channel %d'%(c+1))
                                raise mdsExceptions.TclFAILED_ESSENTIAL

                            if Device.debug: print('channel: ' + str(c))
                            realChannel = 2*c+1+e
                            if Device.debug: print('real channel: ' + str(realChannel))
                            if Device.debug: print('code: ' + str(evCode))
                            if swMode == 'REMOTE':
                                #status = Data.execute('MdsValue("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4,$5)', boardId, c, evCode, evTermCode, evEdgeCode)
                                status = Data.execute('MdsValue("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4,$5)', boardId, realChannel, evCode, evTermCode, evEdgeCode)
                                if status == 0:
                                    Data.execute('MdsDisconnect()')
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW set event setup.')
                                    raise mdsExceptions.TclFAILED_ESSENTIAL
                            else:
                                #status = Data.execute("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", boardId, c, evCode, evTermCode, evEdgeCode)
                                status = Data.execute("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", boardId, realChannel, evCode, evTermCode, evEdgeCode)
                                if status == 0:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW set event setup')
                                    raise mdsExceptions.TclFAILED_ESSENTIAL



                        else:
                            if Device.debug: print('OFF')

                elif function == 'CLOCK SOURCE + CLOCK':
                    try :
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data() # Frequenza del clock esterno
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data() # Frequenza del clock da generare
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK SOURCE + CLOCK parameters for channel %d'%(c+1))
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
                    period = int((1. / freq2) / 1E-7 + 0.5) * 1E-7;
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period))


# End Channels Setup


                if swMode == 'REMOTE':
                    status = Data.execute('MdsValue("DIO4HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, synch)
                    Data.execute('MdsDisconnect()')
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO4 device. See CPCI console for details')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                else:
                    status = Data.execute("DIO4HWStartChan(0, $1, $2, $3)",boardId, channelMask, synch)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO4 Device')
                        raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1



# RESET
    def RESET(self):
        print('--RESET')
# Board ID
        try:
            boardId = self.board_id.data()
            if Device.debug: print('BOARD_ID: ' + str(boardId))
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
            if Device.debug: print('IP_ADDR: ' + ipAddr)
#HW Reset
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO4HWReset(0, $1)", $1)', boardId)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr', self.nid, 'Cannot execute remote HW reset. See CPCI console for details')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO4HWReset(0, $1)", boardId)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW reset')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1



    def STORE(self):
        if Device.debug: print('STORE')
# Board ID
        try:
            boardId = self.board_id.data()
            if Device.debug: print('BOARD_ID: ' + str(boardId))
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
                status = Data.execute('MdsValue("DIO4HWGetRecEvents(0, $1)", $1)', boardId)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO4HWGetRecEvents')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            status = Data.execute("DIO4HWGetRecEvents(0, $1)", boardId)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO4HWGetRecEvents')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        recEvents = Data.execute("_DIO4_rec_events")
        recTimes = Data.execute("_DIO4_rec_times")
        recEventNum  = Data.execute("size(_DIO4_rec_times)")
        if Device.debug: print("EVENTS ",recEvents)
        if Device.debug: print("TIMES ", recTimes)
        if Device.debug: print("NUM   ", recEventNum)

        #if recEvents[0] != -1:
        if recEventNum > 0 :
            #setattr(self,'rec_events', recEvents)
            print("rec_data ", self.rec_events.getFullPath())
            #self.rec_events.putData(recEvents)

            """
            try:
                recStartEv = getattr(self, 'rec_start_ev').data()
            except:
                recStartEv = ''
            if recStartEv != '':
                recStartTime = Data.execute('TimingGetEventTime($1)', recStartEv)
            else:
                recStartTime = 0
            setattr(self,'rec_times', recTimes + recStartTime)
            """
            if Device.debug: print("rec_times ", self.rec_times.getFullPath())
            #self.rec_times.putData(recTimes + recStartTime)

        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if Device.debug: print('FUNCTION: ' + function)
                if function == 'PULSE' or function == 'GCLOCK' or function == 'DCLOCK':
                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWGetPhaseCount(0, $1, $2)", $1,$2)', boardId, channelMask)
                        Data.execute('MdsDisconnect()')
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot get phase count')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    else:
                        status = Data.execute("DIO4HWGetPhaseCount(0, $1, $2)",boardId, channelMask)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot get phase count')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                    phases = Data.execute("_DIO4_phases")
                    print(phases)
                    trigPath = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                    trig1 = trigPath + '+' + phases[0]
                    trig2 = trigPath + '+' + phases[1]
                    try:
                        setattr(self, 'channel_%d_trig_1'%(c+1), Data.compile(trig1))
                        setattr(self, 'channel_%d_trig_2'%(c+1), Data.compile(trig2))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write trigger parameters for channel %d'%(c+1))
                        raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1


    def TRIGGER(self):
        if Device.debug: print('TRIGGER')
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
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                raise mdsExceptions.TclFAILED_ESSENTIAL
#HW TRIGGER
        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if (function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK'):
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        if trigMode not in trigModeDict.keys():
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid trigger mode')
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                        if trigMode == 'SOFTWARE':
                            if swMode == 'REMOTE':
                                status = Data.execute('MdsConnect("'+ ipAddr + '")')
                                if status > 0:
                                    status = Data.execute('MdsValue("DIO4HWTrigger(0, $1, $2)", $1, $2)', boardId, channelMask)
                                    if status == 0:
                                        Data.execute('MdsDisconnect()')
                                        Data.execute('DevLogErr', self.nid, 'Cannot execute remote trigger')
                                        raise mdsExceptions.TclFAILED_ESSENTIAL
                                else:
                                    Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                                    raise mdsExceptions.TclFAILED_ESSENTIAL
                            else:
                                status = Data.execute("DIO4HWTrigger(0, $1, $2)", boardId, channelMask)
                                if status == 0:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute trigger')
                                    raise mdsExceptions.TclFAILED_ESSENTIAL
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Error setting trigger mode')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
#SW EVENT
        huge = Data.execute('HUGE(0)')
        if getattr(self, 'out_ev_sw').isOn():
            try:
                evName = getattr(self, 'out_ev_sw_name').data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            print "SW Event ",evCode 
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
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        
            if swMode == 'REMOTE':
                status = Data.execute('MdsValue("DIO4HWEventTrigger(0, $1, $2)", $1,$2)', boardId, evCode)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            else:
                status = Data.execute("DIO4HWEventTrigger(0, $1, $2)", boardId, evCode)
                if status == 0:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                    raise mdsExceptions.TclFAILED_ESSENTIAL        
        else:
            print('SW EVENT OFF')

        """
        if getattr(self, 'out_ev_sw').isOn():
            try:
                if Device.debug: print('SW CHANNEL IS ON')
                evCode = getattr(self, 'out_ev_sw_code').data()
                if Device.debug: print(evCode)
                if swMode == 'REMOTE':
                    status = Data.execute('MdsValue("DIO4HWEventTrigger(0, $1, $2)", $1,$2)', boardId, evCode)
                    if status == 0:
                        Data.execute('MdsDisconnect()')
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                else:
                    status = Data.execute("DIO4HWEventTrigger(0, $1, $2)", boardId, evCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                        raise mdsExceptions.TclFAILED_ESSENTIAL

            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event Code specification for software channel')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        """

        return 1


###################################################
    def saveInfo(self):
        DIO4.handles[self.nid] = self.handle

    def restoreInfo(self):
        if DIO4.mainLib is None:
            DIO4.mainLib = CDLL("libDIO4.so")
            print('carico la libreria')

        if self.nid in DIO4.handles.keys():
            self.handle = DIO4.handles[self.nid]
            if Device.debug: print('RESTORE INFO HANDLE TROVATO')
        else:
            print('RESTORE INFO HANDLE NON TROVATO')
            try:
                boardId = self.board_id.data()
                if Device.debug: print(boardId)
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Invalid BOARD_ID')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                DIO4.mainLib.DIO4_InitLibrary()
                status = DIO4.mainLib.DIO4_Open(c_int(boardId), byref(c_int(self.handle)))
                print(status)
                print(self.handle)
            except:
                Data.execute('DevLogErr($1,$2)', self.nid, 'Cannot open device')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

    def removeInfo(self):
        del(DIO4.handles[self.nid])

    def start_store(self):
        if Device.debug: print('START STORE')
        self.restoreInfo()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.configure(self, self.handle)
        self.saveWorker()
        self.worker.start()
        return

    def stop_store(self):
        if Device.debug: print('STOP STORE')
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
                if Device.debug: print('RUN')
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
