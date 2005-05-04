public fun RFXDClock__init(as_is _nid, optional _method)
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
    private _N_FREQUENCY1=     9;
    private _N_FREQUENCY2=     10;
    private _N_DURATION  =     11;
    private _N_OUTPUT_MODE=    12;
    private _N_CLOCK     =     13;
    private _N_INIT_ACTION=    14;

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

    private _LARGE_TIME = 100.;



    _decoder =  if_error(DevNodeRef(_nid, _N_DECODER), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));
    _decoder_nid = compile(getnci(getnci(_decoder, 'record'), 'fullpath'));
    _gate_chan = if_error(data(DevNodeRef(_nid, _N_GATE_CHAN)), (DevLogErr(_nid, 'Cannot resolve clock channel');abort();));
    _clock_chan = if_error(data(DevNodeRef(_nid, _N_CLOCK_CHAN)), (DevLogErr(_nid, 'Cannot resolve gate channel');abort();));

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

    _base_nid = if_error(getnci(_decoder_nid, 'nid_number'), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));
    _clock_chan_nid = if_error(_N_CHANNEL_0 + (_clock_chan - 1) *  _K_NODES_PER_CHANNEL, (DevLogErr(_nid, 'Cannot resolve clock channel');abort();));
    _clock_chan_ofs = _N_CHANNEL_0 + (_clock_chan - 1) *  _K_NODES_PER_CHANNEL;
    _gate_chan_nid = if_error(_N_CHANNEL_0 + (_gate_chan - 1) *  _K_NODES_PER_CHANNEL, (DevLogErr(_nid, 'Cannot resolve gate channel');abort();));
    _gate_chan_ofs = _N_CHANNEL_0 + (_gate_chan - 1) *  _K_NODES_PER_CHANNEL;

    if_error(DevNodeCvt(_nid, _N_OUTPUT_MODE, ['SINGLE SWITCH: TOGGLE', 'SINGLE SWITCH: HIGH PULSES','SINGLE SWITCH: LOW PULSES',
		'DOUBLE SWITCH: TOGGLE','DOUBLE SWITCH: HIGH PULSES','DOUBLE SWITCH: LOW PULSES'],
		[0,1,2,3,4,5], _output_mode = 0), (DevLogErr(_nid, 'Cannot resolve output mode');abort();));

    if_error(DevNodeCvt(_nid, _N_TRIG_MODE, ['EVENT', 'TRIGGER RISING','TRIGGER FALLING'],
		[0,1,2], _trigger_mode = 0), (DevLogErr(_nid, 'Cannot resolve trigger mode');abort();));

    if(_trigger_mode == 0)
    {
    	if_error(_event = data(DevNodeRef(_nid, _N_EVENT)), (DevLogErr(_nid, 'Cannot resolve event');abort();));
    	_event_num = TimingDecodeEVent(_event);
    	if(_event_num == 0)
	    DevLogErr(_nid, "Invalid Event name");
    }
    else
		_event_num = 0;
    if(_event_num != 0)
		_event_time = TimingGetEventTime(_event);
    else
		_event_time = if_error(data(DevNodeRef(_nid, _N_EXT_TRIG)), (DevLogErr(_nid, 'Cannot resolve trigger time');abort();));

    _delay = if_error(data(DevNodeRef(_nid, _N_DELAY)), (DevLogErr(_nid, 'Cannot resolve delay');abort();));
    if(_delay < 0 || _delay >655.35)
    { 
		DevLogErr(_nid, "Invalid Delay: "//_delay);
		_delay = 0;
	}
    if(_output_mode >= 3)
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

    _frequency1 = if_error(data(DevNodeRef(_nid, _N_FREQUENCY1)), (DevLogErr(_nid, 'Cannot resolve frequency 1');abort();));
    _frequency2 = if_error(data(DevNodeRef(_nid, _N_FREQUENCY2)), (DevLogErr(_nid, 'Cannot resolve frequency 2');abort();));

    if(_frequency1 > 500E3 || _frequency1 < 100./65535)
    {
		DevLogErr(_nid, 'Invalid frequency 1');
		abort();
    }
	if(_frequency2 > 500E3 || _frequency2 < 100./65535)
    {
		DevLogErr(_nid, 'Invalid frequency 2');
		abort();
    }
	_curr_period = 1E-6;
   	 if(_delay > _duration)
		_max_delay = _delay;
    else
		_max_delay = _duration;
    _clock_source = 0;
    
    _multiplier = 1;
    while(_curr_period * 65534 < _max_delay)
    {
		_clock_source++;
		_multiplier = _multiplier * 10;
  		_curr_period = _curr_period * 10;
    }
    _curr_period = 1D-6 * _multiplier;
    
    _load = long(_delay / _curr_period + 0.5);
    _hold = long(_duration / _curr_period + 0.5);
    if(_load < 3) _load = 3;
    if(_hold < 3) _hold = 3;
    _effective_delay = _load * _curr_period;
    _effective_duration = _hold * _curr_period;
    _gate_period = _curr_period;

    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_EVENTS, _event_num); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_LOAD, _load); 
    DevPut(_base_nid, _gate_chan_nid + _N_CHAN_HOLD, _hold); 
    _trigger_modes = ['TRIGGER RISING', 'TRIGGER RISING', 'TRIGGER FALLING'];
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
    _period1 = 1./_frequency1;
    _period2 = 1./_frequency2;
    if(_period1 > _period2)
		_period = _period1;
    else
		_period = _period2;
    _curr_period = 1D-6;
    _clock_source = 0;
    
    _multiplier = 1;
    while(_curr_period * 65534 < _period)
    {
		_clock_source++;
		_multiplier = _multiplier * 10;
  		_curr_period = _curr_period * 10;
    }
    
    _curr_period = 1D-6 * _multiplier;
    
    if(_output_mode == 0 || _output_mode == 3)
    {
		_load = long(_period1 / (_curr_period * 2) + 0.5);
		_hold = long(_period2 / (_curr_period * 2) + 0.5);
		_effective_period1 = _curr_period * _load * 2;
		_effective_period2 = _curr_period * _hold * 2;
    }
    else
    {
		_load = long(_period1 / _curr_period + 0.5);
		_hold = long(_period2 / _curr_period + 0.5);
		_effective_period1 = _curr_period * _load;
		_effective_period2 = _curr_period * _hold;
    }


    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_EVENTS, 0); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_LOAD, _load); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_HOLD, _hold); 
    _trigger_modes = ['TRIGGER RISING', 'TRIGGER RISING', 'TRIGGER FALLING', 'NONE'];
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_GATING, 'NONE'); 
    _clock_sources = ['1MHz', '100KHz', '10KHz', '1KHz', '100Hz'];		
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_CLOCK_SOURCE, trim(_clock_sources[_clock_source])); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_CLOCK_EDGE, 'RISING'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_SPECIAL_GATE, 'ENABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_DOUBLE_LOAD, 'ENABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_REPEAT_COUNT, 'ENABLED'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_COUNT_MODE, 'BINARY'); 
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_COUNT_DIR, 'DESCENDING');

    _output_modes = ['TOGGLE: INITIAL HIGH', 'HIGH PULSES', 'LOW PULSES', 'TOGGLE: INITIAL HIGH',
		'HIGH PULSES', 'LOW PULSES'];
    DevPut(_base_nid, _clock_chan_nid + _N_CHAN_OUTPUT_MODE, trim(_output_modes[_output_mode])); 
	

    if(_event_num != 0)
    {
		_trigger_time = TimingGetEventTime(_event);
		DevPut(_nid, _N_EXT_TRIG, _event_time); 
    }
	else
		_trigger_time = if_error(data(DevNodeRef(_nid, _N_EXT_TRIG)), (DevLogErr(_nid, 'Cannot resolve trigger time');abort();));  

    _real_delay = _effective_delay;
    _real_duration = _effective_duration;

    DevNodeCvt(_decoder_nid, _N_SYNCHRONIZE, ['ENABLED', 'DISABLED'],[1,0], _synchronize = 0);
    if(_event_num != 0 && _synchronize == 1)
    {
    	_start_event = if_error(long(data(DevNodeRef(_decoder_nid, _N_START_EVENT))), (DevLogErr(_nid, 'Cannot resolve start event');abort();));


		_start_time = TimingGetEventTime(TimingEncodeEvent(_start_event));
		_interval = _trigger_time - _start_time;
		if(_start_event == _event_num)
/*			_correction =  _gate_period;
*/			_correction =  0;
		else
			_correction = _interval - long((_interval + 1D-7)/_gate_period) * _gate_period - _gate_period;
		_effective_delay = _effective_delay - _correction;
		if((_output_mode == 0) || (_output_mode == 3))
		{
			_semi_periods = (_interval + _effective_delay + 1D-7)/(_effective_period1/2.);
			_int_semi_periods = long(_semi_periods);
			_real_delay = _int_semi_periods * _effective_period1/2. + _effective_period1/2. - _interval;
		}
		else
		{
			_semi_periods = (_interval + _effective_delay + 1D-7)/_effective_period1;
			_int_semi_periods = long(_semi_periods);
			_real_delay = _int_semi_periods * _effective_period1 + _effective_period1 - _interval;
		}
/*	_real_duration = _effective_delay + _effective_duration - _real_delay;
*/
		_real_duration = _real_duration - _correction - _effective_period1/2.;


	}
    if(_effective_period1 < 1./148D3)
		_real_delay = _real_delay - _effective_period1/2.;
    if(_effective_period2 < 1./148D3)
		_real_duration = _real_duration - _effective_period2/2.;

    _real_duration_r = _real_duration - _effective_period2/2.;
    _real_delay_r = _real_delay - _effective_period1/2.;


    if(_output_mode >=3)
    {
/*		_axis = compile('MAKE_RANGE([-100.,'//f_float(_trigger_time)//'+'//f_float(_real_delay)//','// 
		f_float(_trigger_time)//'+'//f_float(_real_delay)//'+'//f_float(_real_duration)//'],['//f_float(_trigger_time)//'+'//f_float(_real_delay_r)//','//
			f_float(_trigger_time)//'+'//f_float(_real_delay)//'+'//f_float(_real_duration_r)//', `_LARGE_TIME],['//
			f_float(_effective_period1)//','//f_float(_effective_period2)//','//f_float(_effective_period1)//'])');
*/
		_axis1 = compile('MAKE_RANGE([-100.,'//f_float(_trigger_time)//'+'//f_float(_real_delay)//','// 
		f_float(_trigger_time)//'+'//f_float(_real_delay)//'+'//f_float(_real_duration)//'],['//f_float(_trigger_time)//'+'//f_float(_real_delay_r)//','//
			f_float(_trigger_time)//'+'//f_float(_real_delay)//'+'//f_float(_real_duration_r)//', `_LARGE_TIME],['//
			ft_float(_effective_period1)//','//ft_float(_effective_period2)//','//ft_float(_effective_period1)//'])');

	}
	else
	{
/*		_axis = compile('MAKE_RANGE([-100., '//f_float(_trigger_time)//'+'//f_float(_real_delay)//'],['//
			f_float(_trigger_time)//'+'//f_float(_real_delay_r)//',100],['//
			f_float(_effective_period1)//','//f_float(_effective_period2)//'])');
*/		_axis1 = compile('MAKE_RANGE([-100., '//f_float(_trigger_time)//'+'//f_float(_real_delay)//'],['//
			f_float(_trigger_time)//'+'//f_float(_real_delay_r)//',100],['//
			f_float(_effective_period1)//','//f_float(_effective_period2)//'])');
	}

	DevPut(_nid, _N_CLOCK, _axis1); 

    return (1);
}
