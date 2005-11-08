public fun RFXGClock__init(as_is _nid, optional _method)
{
    private _N_HEAD      =     0;
    private _N_COMMENT   =     1;
    private _N_DECODER   =     2;
    private _N_GATE_CHAN =     3;
    private _N_CLOCK_CHAN=     4;
    private _N_TRIG_MODE =     5;
    private _N_EVENT     =     6;
    private _N_EXT_TRIG  =     7;
    private _N_DELAY     =     8;
    private _N_FREQUENCY =     9;
    private _N_DURATION  =     10;
    private _N_OUTPUT_MODE=    11;
    private _N_CLOCK     =     12;
    private _N_INIT_ACTION=    13;

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





    _invalid = 0;
    _decoder =  if_error(DevNodeRef(_nid, _N_DECODER), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve decoder');
	abort();
    }
    _decoder_nid = compile(getnci(getnci(_decoder, 'record'), 'fullpath'));
    
    _gate_chan = if_error(data(DevNodeRef(_nid, _N_GATE_CHAN)), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve clock channel');
	abort();
    }
    _clock_chan = if_error(data(DevNodeRef(_nid, _N_CLOCK_CHAN)), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve gate channel');
	abort();
    }

    if(_clock_chan < 1 || _clock_chan > 6)
    {
		DevLogErr(_nid, "Wrong clock channel number");
		abort();
    }
    if(_gate_chan < 1 || _gate_chan > 6)
    {
		DevLogErr(_nid, "Wrong gate channel number");
		abort();
    }

    _base_nid = if_error(getnci(_decoder_nid, 'nid_number'), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve decoder');
	abort();
    }
    _clock_chan_nid = if_error(_N_CHANNEL_0 + (_clock_chan - 1) *  _K_NODES_PER_CHANNEL, _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve clock channel');
	abort();
    }
    _clock_chan_ofs = _N_CHANNEL_0 + (_clock_chan - 1) *  _K_NODES_PER_CHANNEL;
    _gate_chan_nid = if_error(_N_CHANNEL_0 + (_gate_chan - 1) *  _K_NODES_PER_CHANNEL, _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve gate channel');
	abort();
    }
    _gate_chan_ofs = _N_CHANNEL_0 + (_gate_chan - 1) *  _K_NODES_PER_CHANNEL;

    if_error(DevNodeCvt(_nid, _N_OUTPUT_MODE, ['SINGLE SWITCH: TOGGLE', 'SINGLE SWITCH: HIGH PULSES','SINGLE SWITCH: LOW PULSES',
		'DOUBLE SWITCH: TOGGLE','DOUBLE SWITCH: HIGH PULSES','DOUBLE SWITCH: LOW PULSES'],
		[0,1,2,3,4,5], _output_mode = 0), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve output mode');
	abort();
    }

    if_error(DevNodeCvt(_nid, _N_TRIG_MODE, ['EVENT', 'TRIGGER RISING','TRIGGER FALLING','SOFTWARE'],
		[0,1,2,3], _trigger_mode = 0), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve trigger mode');
	abort();
    }


    if(_trigger_mode == 0)
    {
    	if_error(_event = data(DevNodeRef(_nid, _N_EVENT)), _invalid = 1);
	if(_invalid)
	{
	    DevLogErr(_nid, 'Cannot resolve event');
	    abort();
	}
   	_event_num = TimingDecodeEvent(_event);
   	if(_event_num == 0)
	    DevLogErr(_nid, "Invalid Event name");
    }
    else
		_event_num = 0;
    if(_event_num != 0)
		_event_time = TimingGetEventTime(_event);
    else
    {
		_event_time = if_error(data(DevNodeRef(_nid, _N_EXT_TRIG)), _invalid = 1);
		if(_invalid)
		{
			DevLogErr(_nid, 'Cannot resolve trigger time');
			abort();
		}
	}
     _delay = if_error(data(DevNodeRef(_nid, _N_DELAY)), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve delay');
	abort();
    }
    if(_delay < 0 || _delay >655.35)
    { 
		DevLogErr(_nid, "Invalid Delay: "//_delay);
		_delay = 0;
    }
    if(_output_mode >= 3)
    {
	_duration = if_error(data(DevNodeRef(_nid, _N_DURATION)), _invalid = 1);
	if(_invalid)
	{
	    DevLogErr(_nid, 'Cannot resolve duration');
	    abort();
	}
    	if(_duration < 0 || _duration > 655.35)
    	{ 
			DevLogErr(_nid, "Invalid Duration: "//_duration);
			_duration = 0;
    	}
    }
    else
	_duration = 0;

    _frequency = if_error(data(DevNodeRef(_nid, _N_FREQUENCY)), _invalid = 1);
    if(_invalid)
    {
    	DevLogErr(_nid, 'Cannot resolve frequency ');
	abort();
    }
    if(_frequency > 500E3 || _frequency < 100./65535)
    {
		DevLogErr(_nid, 'Invalid frequency');
		abort();
    }


   _curr_period = 1E-6;
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



    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_EVENTS, _event_num); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_LOAD, _load); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_HOLD, _hold); 
    _trigger_modes = ['TRIGGER RISING', 'TRIGGER RISING', 'TRIGGER FALLING', 'NONE'];
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_GATING, trim(_trigger_modes[_trigger_mode])); 
    _clock_sources = ['1MHz', '100KHz', '10KHz', '1KHz', '100Hz'];		
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_CLOCK_SOURCE, trim(_clock_sources[_clock_source])); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_CLOCK_EDGE, 'RISING'); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_SPECIAL_GATE, 'DISABLED'); 
    if(_output_mode >= 3)
	DevPut(_base_nid, _gate_chan_nid + _N_CHAN_DOUBLE_LOAD, 'ENABLED'); 
    else
	DevPut(_base_nid, _gate_chan_nid + _N_CHAN_DOUBLE_LOAD, 'DISABLED'); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_REPEAT_COUNT, 'DISABLED'); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_COUNT_MODE, 'BINARY'); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_COUNT_DIR, 'DESCENDING');
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_OUTPUT_MODE, 'TOGGLE: INITIAL LOW'); 




	_period = 1./_frequency;
    _curr_period = 1E-6;
    _clock_source = 0;
    while(_curr_period * 65534 < _period)
    {
		_clock_source++;
  		_curr_period = _curr_period * 10;
    }
    _load = long(_period/(_curr_period*2) + 0.5);
    _hold = long(_period/_curr_period - _load + 0.5);

    _effective_period = _curr_period * (_load + _hold);

    if(_load == 0)
    {
		_load++;
		_hold--;
    }
    if(_hold == 0)
    {
		_hold++;
		_load--;
    }
    if((_output_mode != 0)&&(_output_mode != 3))
    {
		_load = _load * 2;
		_hold = _hold * 2;
    }



    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_EVENTS, 0); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_LOAD, _load); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_HOLD, _hold); 
     DevPut(_base_nid, _clock_chan_nid + _N_CHAN_GATING, 'GATE N'); 
    _clock_sources = ['1MHz', '100KHz', '10KHz', '1KHz', '100Hz'];		
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_CLOCK_SOURCE, trim(_clock_sources[_clock_source])); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_CLOCK_EDGE, 'RISING'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_SPECIAL_GATE, 'DISABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_DOUBLE_LOAD, 'ENABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_REPEAT_COUNT, 'ENABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_COUNT_MODE, 'BINARY'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_COUNT_DIR, 'DESCENDING');

    _output_modes = ['TOGGLE: INITIAL LOW', 'HIGH PULSES', 'LOW PULSES', 'TOGGLE: INITIAL LOW',
		'HIGH PULSES', 'LOW PULSES'];
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_OUTPUT_MODE, trim(_output_modes[_output_mode])); 
	

    if(_event_num != 0)
    {
		_trigger_time = TimingGetEventTime(_event);
		DevPut(_nid, _N_EXT_TRIG, _event_time); 
    }
	else
		_trigger_time = if_error(data(DevNodeRef(_nid, _N_EXT_TRIG)), (DevLogErr(_nid, 'Cannot resolve trigger time');abort();));  

        
    if(_output_mode >=3)
		_axis = compile('MAKE_RANGE('//_trigger_time//'+'//_delay//','// 
			_trigger_time//'+'//_delay//'+'//_duration//','// _effective_period//')');
	else
		_axis = compile('MAKE_RANGE('//_trigger_time//'+'//_delay//',HUGE(0.),'// _effective_period//')');

   write(*, _axis);
	DevPut(_nid, _N_CLOCK, _axis); 
    return (1);
}
