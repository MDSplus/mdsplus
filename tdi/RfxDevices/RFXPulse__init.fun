public fun RFXPulse__init(as_is _nid, optional _method)
{
    private _N_HEAD      =      0;
    private _N_COMMENT   =      1;
    private _N_TARGET    =      2;
    private _N_CHANNEL   =      3;
    private _N_TRIGGER_MODE =   4;
    private _N_EVENT_LIST    =  5;
    private _N_EXT_TRIGGER   =  6;
    private _N_DELAY       =  7;
    private _N_DURATION       =  8;
    private _N_EFFECTIVE_DELAY     =  9;
    private _N_EFFECTIVE_DURATION  =  10;
    private _N_CORRECTION    =  11;
    private _N_TRIGGER_1     =  12;
    private _N_TRIGGER_2     =  13;
    private _N_OUTPUT_MODE   =  14;
    private _N_INIT_ACTION   =  15;

    private _K_NODES_PER_CHANNEL = 14;
    private _N_CHANNEL_0= 9;
    private _N_CHAN_EVENTS = 1;
    private _N_CHAN_LOAD = 2;
    private _N_CHAN_HOLD = 3;
    private _N_CHAN_GATING = 4;
    private _N_CHAN_CLOCK_SOURCE = 5;
    private _N_CHAN_CLOCK_EDGE = 6;
    private _N_CHAN_SPECIAL_GATE = 7;
    private _N_CHAN_DOUBLE_LOAD = 8;
    private _N_CHAN_REPEAT_COUNT = 9;
    private _N_CHAN_COUNT_MODE = 10;
    private _N_CHAN_COUNT_DIR = 11;
    private _N_CHAN_OUTPUT_MODE = 12;
    private _N_CHAN_CLOCK = 13;
    private _N_CLOCK_DIVIDE = 3;
    private _N_CLOCK_SOURCE = 4;
    private _N_SYNCHRONIZE = 6;
    private _N_START_EVENT = 7;

    _decoder =  if_error(DevNodeRef(_nid, _N_TARGET), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));
    _decoder_nid = compile(getnci(getnci(_decoder, 'record'), 'fullpath'));
    _channel = if_error(data(DevNodeRef(_nid, _N_CHANNEL)), (DevLogErr(_nid, 'Cannot resolve channel');abort();));
    if(_channel < 1 || _channel > 6)
    {
		DevLogErr(_nid, "Wrong channel number");
		abort();
    }
    _base_nid = if_error(getnci(_decoder_nid, 'nid_number'), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));
    _chan_nid = if_error(_N_CHANNEL_0 + (_channel - 1) *  _K_NODES_PER_CHANNEL, (DevLogErr(_nid, 'Cannot resolve channel');abort();));
    _chan_ofs = _N_CHANNEL_0 + (_channel - 1) *  _K_NODES_PER_CHANNEL;
    if_error(DevNodeCvt(_nid, _N_OUTPUT_MODE, ['HIGH PULSE', 'LOW PULSE','SINGLE TOGGLE: INITIAL HIGH',
		'SINGLE TOGGLE: INITIAL LOW','DOUBLE TOGGLE: INITIAL HIGH','DOUBLE TOGGLE: INITIAL LOW'],
		[0,1,2,3,4,5], _output_mode = 0), (DevLogErr(_nid, 'Cannot resolve output mode');abort();));
    if_error(DevNodeCvt(_nid, _N_TRIGGER_MODE, ['EVENT', 'TRIGGER RISING','TRIGGER FALLING', 'SOFTWARE'],
		[0,1,2,3], _trigger_mode = 0), (DevLogErr(_nid, 'Cannot resolve trigger mode');abort();));
    if(_trigger_mode == 0)
    {
    	if_error(_event = data(DevNodeRef(_nid, _N_EVENT_LIST)), (DevLogErr(_nid, 'Cannot resolve event');abort();));
    		_event_num = TimingDecodeEvent(_event);
    	if(_event_num == 0)
			DevLogErr(_nid, "Invalid Event name");
    }
    else
		_event_num = 0;
    if(_event_num != 0)
    {
		_event_time = TimingGetEventTime(_event);
		DevPut(_nid, _N_EXT_TRIGGER, _event_time); 
    }
    _delay = if_error(data(DevNodeRef(_nid, _N_DELAY)), (DevLogErr(_nid, 'Cannot resolve delay');abort();));
    if(_delay < 0 || _delay >655.35)
    { 
		DevLogErr(_nid, "Invalid Delay: "//_delay);
		_delay = 0;
    }
    if(_output_mode > 3)
    {
		_duration = if_error(data(DevNodeRef(_nid, _N_DURATION)), (DevLogErr(_nid, 'Cannot resolve duration');abort();));
    	if(_duration < 0 || _duration > 655.35)
    	{ 
			DevLogErr(_nid, "Invalid Duration: "//_duration);
			_duration = 0;
    	}
    }
    else
		_duration = 0;
    _curr_period = 1D-6;
    if(_delay > _duration)
		_max_delay = _delay;
    else
		_max_delay = _duration;
    _clock_source = 0;
    while(_curr_period * 65534 < _max_delay)
    {
		_clock_source++;
  		_curr_period = _curr_period * 10;
    }
    _load = long(_delay / _curr_period + 0.5);
    _hold = long(_duration / _curr_period + 0.5);
    if(_load < 3) _load = 3;
    if(_hold < 3) _hold = 3;
    _effective_delay = _load * _curr_period;
    if(_output_mode <= 2)
		_effective_duration = _curr_period;
    else
		_effective_duration = _hold * _curr_period;
    DevPut(_nid, _N_EFFECTIVE_DELAY, f_float(_effective_delay)); 
    DevPut(_nid, _N_EFFECTIVE_DURATION, f_float(_effective_duration)); 
	if(_trigger_mode > 0)
    	DevPut(_nid, _N_CORRECTION, 0); 
    else
    {
   		DevNodeCvt(_decoder_nid, _N_SYNCHRONIZE, ['ENABLED', 'DISABLED'],[1,0], _synchronize = 0);
		if(_synchronize == 1)
		{
    			_start_event = if_error(long(data(DevNodeRef(_decoder_nid, _N_START_EVENT))), (DevLogErr(_nid, 'Cannot resolve start event');abort();));
			_start_time = TimingGetEventTime(TimingEncodeEvent(_start_event));
		}
		else
		{
			_start_event = 0;
			_start_time = 0;
		}
		if(_start_time == 0)
    		DevPut(_nid, _N_CORRECTION, 0); 
		else
		{
			if(_start_event == _event_num)
			{
		  		DevPut(_nid, _N_CORRECTION, _curr_period); 
			}
			else
			{
				_trig_path = getnci(DevNodeRef(_nid, _N_EXT_TRIGGER), 'FULLPATH');
				_corr_expr = _trig_path // ' + 1E-8 - ' // _start_time // ' - ' //
				f_float(_curr_period) // ' * AINT((' //_trig_path // ' - '// _start_time //
				'+ 1E-8)/' // f_float(_curr_period) // ') - ' // f_float(_curr_period);
  	    			DevPut(_nid, _N_CORRECTION, compile(_corr_expr));
			}
		}
    }
    DevPut(_base_nid, _chan_nid + _N_CHAN_EVENTS, _event_num); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_LOAD, _load); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_HOLD, _hold); 
    _trigger_modes = ['TRIGGER RISING', 'TRIGGER RISING', 'TRIGGER FALLING', 'NONE'];
    DevPut(_base_nid, _chan_nid + _N_CHAN_GATING, trim(_trigger_modes[_trigger_mode])); 
    _clock_sources = ['1MHz', '100KHz', '10KHz', '1KHz', '100Hz'];		
    DevPut(_base_nid, _chan_nid + _N_CHAN_CLOCK_SOURCE, trim(_clock_sources[_clock_source])); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_CLOCK_EDGE, 'RISING'); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_SPECIAL_GATE, 'DISABLED'); 
    if(_output_mode >= 4)
	DevPut(_base_nid, _chan_nid + _N_CHAN_DOUBLE_LOAD, 'ENABLED'); 
    else
	DevPut(_base_nid, _chan_nid + _N_CHAN_DOUBLE_LOAD, 'DISABLED'); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_REPEAT_COUNT, 'DISABLED'); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_COUNT_MODE, 'BINARY'); 
    DevPut(_base_nid, _chan_nid + _N_CHAN_COUNT_DIR, 'DESCENDING');
    _output_modes = ['HIGH PULSES', 'LOW PULSES', 'TOGGLE: INITIAL HIGH', 'TOGGLE: INITIAL LOW',
		'TOGGLE: INITIAL HIGH', 'TOGGLE: INITIAL LOW'];
    DevPut(_base_nid, _chan_nid + _N_CHAN_OUTPUT_MODE, trim(_output_modes[_output_mode])); 
    return (1);
}
