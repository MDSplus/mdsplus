public fun RFXClock__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_TARGET = 2;
    private _N_CHANNEL = 3;
    private _N_FREQUENCY = 4;
    private _N_DUTY_CYCLE = 5;
    private _N_CLOCK = 6;
    private _N_INIT_ACTION= 7;
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

write(*, "RFX Clock init");

    _decoder =  if_error(DevNodeRef(_nid, _N_TARGET), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));
    _decoder_nid = compile(getnci(getnci(_decoder, 'record'), 'fullpath'));
    _base_nid = getnci(_decoder_nid, 'nid_number');
    _channel = if_error(data(DevNodeRef(_nid, _N_CHANNEL)), (DevLogErr(_nid, 'Cannot resolve channel');abort();));
    if(_channel < 0 || _channel > 6)
    {
		DevLogErr(_nid, "Wrong channel number");
		abort(0);
    }
    _frequency = if_error(data(DevNodeRef(_nid, _N_FREQUENCY)), (DevLogErr(_nid, 'Cannot resolve frequency');abort();));
    if(_frequency < 100./65535) _frequency = 100./65535;
    if(_channel == 0)
    {
		if(_frequency > 1E6) _frequency = 1E6;
    }
    else
    {
		if(_frequency > 5E5) _frequency = 5E5;
    }
    if(_channel > 0)
    {
    	_duty_cycle = if_error(data(DevNodeRef(_nid, _N_DUTY_CYCLE)), (DevLogErr(_nid, 'Cannot resolve duty cycle');abort();));
    	if(_duty_cycle < 0) _duty_cycle = 0;
    	if(_duty_cycle > 100) _duty_cycle = 100;
    	_period = 1./_frequency;
    	_curr_period = 1E-6;
    	_clock_source = 0;
    	for(_clock_source = 0; (_curr_period * 65534 < _period); _clock_source = _clock_source + 1)
    	{
	    _curr_period = _curr_period * 10;
	}
    	_load_plus_hold = long(_period/_curr_period + 0.5);
	_effective_period = _curr_period * _load_plus_hold;
    	_load = long(_load_plus_hold * _duty_cycle/100. + 0.5);
    	_hold = _load_plus_hold - _load;
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
    	_effective_period = _curr_period * _load_plus_hold;
    	_chan_ofs = _N_CHANNEL_0 + (_channel - 1) *  _K_NODES_PER_CHANNEL;
	/*TreeShr->TreeTurnOn(val(DevHead(_decoder_nid) + _chan_ofs));*/
	_chan_nid = _N_CHANNEL_0 + (_channel - 1) *  _K_NODES_PER_CHANNEL;
	DevPut(_base_nid, _chan_nid + _N_CHAN_EVENTS, 0); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_LOAD, _load); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_HOLD, _hold); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_GATING, 'NONE'); 
	_clock_sources = ['1MHz', '100KHz', '10KHz', '1KHz', '100Hz'];
	DevPut(_base_nid, _chan_nid + _N_CHAN_CLOCK_SOURCE, trim(_clock_sources[_clock_source])); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_CLOCK_EDGE, 'RISING'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_SPECIAL_GATE, 'DISABLED'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_DOUBLE_LOAD, 'ENABLED'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_REPEAT_COUNT, 'ENABLED'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_COUNT_MODE, 'BINARY'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_COUNT_DIR, 'DESCENDING'); 
	DevPut(_base_nid, _chan_nid + _N_CHAN_OUTPUT_MODE, 'TOGGLE: INITIAL LOW'); 
/*	write(*,  make_range(*,*,_effective_period));*/
	DevPut(_nid, _N_CLOCK, make_range(*,*,_effective_period));
    }
    else /*Channel == 0 */
    {
    	_period = 1./_frequency;
    	_curr_period = 1E-6;
    	_clock_source = 0;
    	for(_clock_source = 0; (_curr_period * 16 < _period); _clock_source++)
 	    _curr_period = _curr_period * 10;
	_load = long(_period/_curr_period + 0.5);
	_effective_period = _curr_period * _load;;
	_clock_divides = ['1MHz','100KHz','10KHz','1KHz','100Hz'];
	DevPut(_base_nid, _N_CLOCK_SOURCE, trim(_clock_divides[_clock_source])); 
	DevPut(_base_nid, _N_CLOCK_DIVIDE, _load); 
	DevPut(_nid, _N_CLOCK, make_range(*,*,_effective_period));
   }

   return (1);
}

