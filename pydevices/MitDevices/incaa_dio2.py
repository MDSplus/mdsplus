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

class INCAA_DIO2(MDSplus.Device):
    """
    INCAA_DIO2 Timer device

    This is a rewrite of the DIO2 device in RfxDevices

    Nodes:

        BOARD_ID     - board number assigned by DIO2 Device Driver [0-n]
        SW_MODE      - either 'LOCAL or REMOTE'
        IP_ADDR      - if SW_MODE is REMOTE ipaddress[:port] to mdsconnect to if remote
        COMMENT      - user comment
        CLOCK_SRC    - HIGHWAY or INTERNAL
        REC_START_EV - name of event(s) to record times of - UNIMPLEMENTED

        8 Channels for the outputs
        .CHANNEL_[1-8]   - STRUCTURE node containing knobs and outputs for channel
            FUNCTION     - Channel function one of 'PULSE', 'CLOCK', 'GGLOCK', 'DCLOCK'
            TRIG_MODE    - trigger mode for this channel one of EVENT, RISING, FALLING, SOFTWARE
            EVENT        - name of the event(s) that will fire this channel
            CYCLIC       - should this channel repeat - text YES / NO
            DELAY        - Delay before channel starts
            DURATION     - Duration if channel is pulse
            FREQUENCY_1  - Clock frequency in Hz
            FREQUENCY_2  - Clock frequency in Hz (if dual speed clock - DCLOCK)
            INIT_LEVEL_1 - starting level for this channel 0-low, 1-high
            INIT_LEVEL_2 - transition to level for this channel 0-low, 1-high
            DUTY_CYCLE   - Duty cycle of clock if 1 speed clock (1 - 100)
            TRIGGER      - Time to assign to this channel's event
            CLOCK        - clock stored if this is channel is in CLOCK_MODE
            TRIGGER_1    - Time of the first transition of this channel if PULSE
            TRIGGER_2    - Time of the second transition of this channel if PULSE
            COMMENT      - Comment on this channel

        REC_EVENTS   - name(s) of events whose times to record
        REC_TIMES    - time(s) of those recorded events
        SYNC         - Syncronize the start of the devices with external event YES/NO
        SYNC_EVENT   - name of events(s) to start the onboard scalars
        INIT_ACTION  - action to arm board
        STORE_ACTION - action to store board - UNIMPLEMENTED

    event codes (0-127) are defined in incaa_event_table.py, sample in /usr/local/mdsplus/tdi/MitDevices
    debug - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """

    parts=[
        {'path':':BOARD_ID','type':'numeric', 'value': 0, 'options':('no_write_shot',),
            'help' : "board number assigned by DIO2 Device Driver [0-n]"},
        {'path':':SW_MODE','type':'text', 'value': 'REMOTE', 'options':('no_write_shot',),
            'help': 'connect to external host[:port] to address hardware LOCAL / REMOTE' },
        {'path':':IP_ADDR','type':'text', 'options':('no_write_shot',),
            'help': 'ipaddress[:port] to mdsconnect to if remote' },
        {'path':':COMMENT','type':'text', 'options':('no_write_shot',),
            'help': 'user comment' },
        {'path':':CLOCK_SRC','type':'text', 'options':('no_write_shot',),
            'help':  'HIGHWAY or INTERNAL'},
        {'path':':REC_START_EV','type':'text', 'options':('no_write_shot',),
            'help': 'name of event to record event times from' },
        ]

    for i in range(8):
        parts.append({'path':'.CHANNEL_%1.1d'%(i+1,),'type':'STRUCTURE',
            'help': 'STRUCTURE node containing knobs and outputs for channel' })
        parts.append({'path':'.CHANNEL_%1.1d:FUNCTION'%(i+1,),'type':'TEXT', 'value':'CLOCK', 'options':('no_write_shot'),
            'help': 'one of PULSE, CLOCK, GGLOCK, DCLOCK' })
        parts.append({'path':'.CHANNEL_%1.1d:TRIG_MODE'%(i+1,),'type':'TEXT', 'value':'EVENT', 'options':('no_write_shot'),
            'help': 'trigger mode for this channel one of EVENT, RISING, FALLING, SOFTWARE' })
        parts.append({'path':'.CHANNEL_%1.1d:EVENT'%(i+1,),'type':'TEXT', 'options':('no_write_shot'),
            'help': 'name of events(s) to start this channel' })
        parts.append({'path':'.CHANNEL_%1.1d:CYCLIC'%(i+1,),'type':'TEXT', 'value': 'NO', 'options':('no_write_shot'),
            'help': 'should this channel repeat NO / YES' })
        parts.append({'path':'.CHANNEL_%1.1d:DELAY'%(i+1,),'type':'NUMERIC', 'value': 1E-6, 'options':('no_write_shot'),
            'help': 'delay in seconds if channel is pulse' })
        parts.append({'path':'.CHANNEL_%1.1d:DURATION'%(i+1,),'type':'NUMERIC', 'value': 5E-6, 'options':('no_write_shot'),
            'help':'duration in Seconds  if channel is pulse'   })
        parts.append({'path':'.CHANNEL_%1.1d:FREQUENCY_1'%(i+1,),'type':'NUMERIC', 'value': 1000, 'options':('no_write_shot'),
            'help': 'Clock frequency in Hz'  })
        parts.append({'path':'.CHANNEL_%1.1d:FREQUENCY_2'%(i+1,),'type':'NUMERIC', 'value': 1000, 'options':('no_write_shot'),
            'help': 'Clock frequency in Hz (if dual speed clock - DCLOCK)' })
        parts.append({'path':'.CHANNEL_%1.1d:INIT_LEVEL_1'%(i+1,),'type':'NUMERIC', 'value': 0, 'options':('no_write_shot'),
            'help': 'starting level for this channel 0-low, 1-high' })
        parts.append({'path':'.CHANNEL_%1.1d:INIT_LEVEL_2'%(i+1,),'type':'NUMERIC', 'value': 1, 'options':('no_write_shot'),
            'help': 'transition to level for this channel 0-low, 1-high' })
        parts.append({'path':'.CHANNEL_%1.1d:DUTY_CYCLE'%(i+1,),'type':'NUMERIC', 'value': 50, 'options':('no_write_shot'),
            'help': 'Duty cycle of clock if 1 speed clock (1 - 100)' })
        parts.append({'path':'.CHANNEL_%1.1d:TRIGGER'%(i+1,),'type':'NUMERIC', 'value': 0, 'options':('no_write_shot'),
            'help': "Time to assign to this channel's event" })
        parts.append({'path':'.CHANNEL_%1.1d:CLOCK'%(i+1,),'type':'NUMERIC',
            'valueExpr': 'Data.compile("* : * : 1.D0/$", head.channel_%1.1d_frequency_1)'%(i+1,),
            'options':('no_write_shot'),
            'help': 'Stored clock (RANGE) if this channel is one of the CLOCK types' })
        parts.append({'path':'.CHANNEL_%1.1d:TRIGGER_1'%(i+1,),'type':'NUMERIC', 
            'valueExpr': 'Data.compile("$ + $", head.channel_%1.1d_trigger, head.channel_%1.1d_delay)'%(i+1, i+1),
            'options':('no_write_shot'),
            'help': 'Time of the first transition of this channel if PULSE'  })
        parts.append({'path':'.CHANNEL_%1.1d:TRIGGER_2'%(i+1,),'type':'NUMERIC',
            'valueExpr': 'Data.compile("$ + $", head.channel_%1.1d_trigger_1, head.channel_%1.1d_duration)'%(i+1, i+1),
            'options':('no_write_shot'),
            'help': 'Time of the second transition of this channel if PULSE' })
        parts.append({'path':'.CHANNEL_%1.1d:COMMENT'%(i+1,),'type':'TEXT', 'options':('no_write_shot'),
            'help': 'comment for this channel' })
    del i
    parts.append({'path':':REC_EVENTS','type':'TEXT', 'value': 'NO', 'options':('no_write_shot'),
        'help': 'Names of events whose time to record' })
    parts.append({'path':':REC_TIMES','type':'NUMERIC', 'options':('no_write_model', 'write_once'),
        'help': 'TIME(s) of recorded events'  })
    parts.append({'path':':SYNCH','type':'TEXT', 'value': 'NO', 'options':('no_write_shot'),
        'help': 'Should this device syncrhonize on an event' })

    parts.append( {'path':':SYNCH_EVENT','type':'text', 'options':('no_write_shot',),
            'help': 'name of events(s) to start the onboard scalars' })
    parts.append( {'path':':INIT_ACTION','type':'action',
            'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head,'auto'))",
            'options':('no_write_shot',),
            'help': 'action to arm board'})
    parts.append( {'path':':STORE_ACTION','type':'action',
            'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
            'options':('no_write_shot',),
            'help': 'action to store board - UNIMPLEMENTED' })

    debug=None

    local_remote = ('LOCAL', 'REMOTE')
    clock_sources = ('EXTERNAL', 'INTERNAL')
    functions = ('CLOCK','PULSE','DCLOCK', 'GCLOCK')
    off_on = ('OFF', 'ON')
    no_yes = ('NO', 'YES')

    @property
    def _board_id(self):
        try:
            board_id = int(self.board_id)
            if board_id < 0 :
                raise MDSplus.DevBAD_PARAMETER('must be a positive integer')
        except Exception as e:
            e.args =  ('DIO2 board ID', + e.args)
            raise
        return board_id

    @property
    def _clock_source(self):
        try:
            clock_source = self.clock_source.data()
        except:
            clock_source = 'INTERNAL'

        if clock_source == 'HIGHWAY':
            return 1
        else:
            return 0

    @property
    def host_ip(self):
        host = str(self.ip_addr.data())
        return host

    @property
    def _local_remote(self):
       try:
           mode_str = str(self.sw_mode.data())
           mode = self.local_remote.index(mode_str.upper())
       except Exception as e:
           raise
       return mode

    @property
    def _synch(self):
        ny = str(self.synch.data().upper())
        return self.no_yes.index(ny)

    def mds_connect(self):
        io_mode = self._local_remote
        self.connection = None
        if io_mode:
            self.connection = MDSplus.Connection(self.host_ip)

    def mds_value(self, *args):
        if self.connection:
            return self.connection.get(*args)
        else:
            return MDSplus.Data.evaluate(*args)

    def event_lookup(self, node):
       evtab = MDSplus.Data.compile('incaa_event_table()').data() 
       print(evtab)
       ev = []
       try:
           event_string = str(node.data())
           strings = event_string.split(',')
           for name in strings:
             ev.append(evtab[name])
       except:
           pass
       if len(ev) == 0:
           return 0
       elif len(ev) == 1:
           return ev[0]
       else:
           return ev

    class _chan(object):
        def __init__(self, head, name, debug):
            self.name = name
            self.head = head
            self.debug = debug

        def chan_prop(self, name):
            return self.head.__getattr__(self.name + '_' + name)

        @property 
        def function(self):
            try:
                function = str(self.chan_prop('function').data())
                if function not in ('PULSE', 'CLOCK', 'GGLOCK', 'DCLOCK'):
                    raise MDSplus.DevBAD_PARAMETER('%s must be one of PULSE, CLOCK, GGLOCK, DCLOCK', str(self.chan_prop('function')))
            except Exception as e:
                raise
            return function
 
        @property
        def frequency_1(self):
            try:
                freq = float(self.chan_prop('frequency_1').data())
                if freq < 0 :
                    raise MDSplus.DevBAD_PARAMETER('%s must be a positive floating value' % str(self.chan_prop('frequency_1')))
            except Exception as e:
                raise
            return freq

        @property
        def frequency_2(self):
            try:
                freq = float(self.chan_prop('frequency_2').data())
                if freq < 0 :
                    raise MDSplus.DevBAD_PARAMETER('%s must be a positive floating value' % str(self.chan_prop('frequency_2')))
            except Exception as e:
                raise
            return freq

        @property
        def duty_cycle(self):
            try:
                duty_cycle = int(self.chan_prop('duty_cycle').data())
                if duty_cycle < 1 or duty_cycle > 100:
                    raise MDSplus.DevBAD_PARAMETER( '%s must be a positive integer value between 1 and 100' % str(self.chan_prop('duty_cycle')))
            except Exception as e:
                raise
            return duty_cycle

        @property
        def cyclic(self):
            if self.chan_prop('cyclic').on:
                return 0
            else:
                return 1

        @property
        def init_level_1(self):
            try:
                level = int(self.chan_prop('init_level_1').data())
                if level not in (0,1):
                    raise MDSplus.DevBAD_PARAMETER('%s must be 0 or 1' % str(self.chan_prop('init_level_1')))
            except Exception as e:
                raise
            return level

        @property
        def init_level_2(self):
            try:
                level = int(self.chan_prop('init_level_2').data())
                if level not in (0,1):
                    raise MDSplus.DevBAD_PARAMETER('%s must be 0 or 1' % str(self.chan_prop('init_level_2')))
            except Exception as e:
                raise
            return level

        @property
        def delay(self):
            try:
                delay = float(self.chan_prop('delay').data())
                if delay <= 0.0: 
                    raise MDSplus.DevBAD_PARAMETER('%s must be >=0' % self.chan_prop('delay'))
            except Exception as e:
                raise
            return delay

        @property
        def duration(self):
            try:
                duration = float(self.chan_prop('duration').data())
                if duration < 0.0: 
                    raise MDSplus.DevBAD_PARAMETER('%s must be >0' % str(self.chan_prop('duration')))
            except MDSplus.DevBAD_PARAMETER as e:
                raise
            except Exception as e:
                raise MDSplus.DevBAD_PARAMETER('%s must be >0 %s' % (str(self.chan_prop('duration')), e,))
            return duration

        @property
        def trigger(self):
            modes = ['SOFTWARE', 'EVENT', 'RISING', 'FALLING']
            mode = modes.index(self.chan_prop('trig_mode').data())
            evtab = MDSplus.Data.compile('incaa_event_table()').data() 
            events = []
            if mode == 1:
                event_str = self.chan.EVENT.data()
                event_names = event_str.split(',')
                if self.debug:
                    print('getting events for %s got %s' %(self.name, event_str,))
                for ev in event_names:
                    events.append(evtab[ev])
            if len(events) == 0:
                events = 0
            elif len(events) == events[1]:
                events = events[0]
            if self.debug:
                print('\treturning events', events)
            return (mode, events)
 
    def init(self):
        import os
        print('this is the one in /mdsplus')
        if self.debug == None:
            self.debug = os.getenv("DEBUG_DEVICES")
        if self.debug:
            print('starting init of incaa_dio2')
            print(os.getenv("PYTHOPNPATH"))
        board_id = self._board_id
        clock_source = self._clock_source
        rec_event_number = self.event_lookup(self.rec_start_ev)
        sync_event_numbers = 0
        if self._synch:
            sync_event_numbers = self.event_lookup(self.synch_event)
        if self.debug:
            print('got the rec_start_ev and the sync_event', rec_event_number, sync_event_numbers)
        self.mds_connect()
        if self.debug:
            print('DIO2HWInit($1, $2, $3, $4, $5)', self.nid_number, board_id, clock_source, rec_event_number, sync_event_numbers)
        self.mds_value('DIO2HWInit($1, $2, $3, $4, $5)', self.head.nid_number, board_id, clock_source, rec_event_number, sync_event_numbers)

        for channel in range(8):
            c_name = 'channel_%1.1d' % (channel+1)
            chan = self.__getattr__(c_name)
            if chan.on:
                _chan = self._chan(self.head, c_name, self.debug)
                function = _chan.function
                if self.debug:
                    print('function for %s is %s' % (str(chan), function))
                if function == 'PULSE':
                    trig_mode, trig_event = _chan.trigger
                    cyclic = _chan.cyclic
                    init_level_1 = _chan.init_level_1
                    init_level_2 = _chan.init_level_2
                    delay = _chan.delay
                    duration = _chan.duration
                    if self.debug:
                        print('DIO2HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)', board_id, channel, trig_mode, cyclic,
                                                init_level_1, init_level_2, delay, duration, trig_event)
                    self.mds_value('DIO2HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)', board_id, channel, trig_mode, cyclic,
                                                init_level_1, init_level_2, delay, duration, trig_event)
                elif function == 'CLOCK':
                    frequency = _chan.frequency_1
                    duty_cycle = _chan.duty_cycle
                    if self.debug:
                        print('DIO2HWSetClockChan(0, $1, $2, $3, $4)', board_id, channel, frequency, duty_cycle)
                    self.mds_value('DIO2HWSetClockChan(0, $1, $2, $3, $4)', board_id, channel, frequency, duty_cycle)
                    _chan.chan_prop('clock').record = MDSplus.Range(None, None, 1.0/frequency)
                elif function == 'GCLOCK':
                    trig_mode, trig_event = _chan.trigger
                    frequency = _chan.frequency_1
                    period = long((1. / frequency) / 1E-7 + 0.5) * 1E-7
                    delay = _chan.delay
                    duration = _chan.duration
                    if self.debug:
                        print(self, 'DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7)', board_id, channel, trig_mode, frequency,
                                  delay, duration, trig_event)
                    self.mds_value(self, 'DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7)', board_id, channel, trig_mode, frequency,
                                  delay, duration, trig_event);
                    _chan.chan_prop('clock').record = MDSplus.Range(MDSplus.Add(chan.TRIGGER,chan.DELAY), 
                                                                    MDSplus.Add(MDSplus.Add(chan.TRIGGER,chan.DELAY),chan.DURATION), 
                                                                    period)
                elif function == 'DCLOCK':
                    trig_mode, trig_event = self.get_trigger(chan)
                    frequency_1 = _chan.frequency_1
                    period_1 = long((1. / frequency_1) / 1E-7 + 0.5) * 1E-7
                    frequency_2 = _chan.frequency_2 
                    period_2 = long((1. / frequency_2) / 1E-7 + 0.5) * 1E-7
                    delay = _chan.delay
                    duration = _chan.duration
                    if self.debug:
                        print(self, 'DIO2HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)', board_id, channel, trig_mode,
                                  frequency_1, frequency_2, delay, duration, event)
                    self.mds_value(self, 'DIO2HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)', board_id, channel, trig_mode,
                                  frequency_1, frequency_2, delay, duration, event)
                    _chan.chan_prop('clock').record = MDSplus.Range([-1E-6, chan.TRIGGER_1, chan.TRIGGER_2], 
                                                                    [ chan.TRIGGER_1, chan.TRIGGER_2, 1E6],
                                                                    [periond_1, period_2, period_1]) 
    INIT=init

    def store(self):
        raise MDSplus.DevUNSUPPORTED_METHOD
    STORE=store

# Note trigger method different than original
#
# Triggers all channels that are either software or event
#
    def trigger(self):
        channel = 0
        board_id = self._board_id

        for channel in range(8):
            c_name = 'channel_%1.1d' % (channel+1)
            chan = self.__getattr__(c_name)
            if chan.on:
                _chan = self._chan(self.head, c_name, self.debug)
                function = _chan.function
                if self.debug:
                    print('function for %s is %s' % (str(chan), function))
                if function == 'PULSE':
                    trig_mode, trig_event = _chan.trigger
                    if trig_mode in (0,3):
                       channel_mask = channel_mask | (1 << channel)
 
            if self.debug:
                print(self, 'DIO2HWTrigger(0, $1, $2)', board_id, channeli_mask)
                self.mds_value(self, 'DIO2HWTrigger(0, $1, $2)', board_id, channel_mask)
    TRIGGER=trigger
