public fun MPBDecoder__init(as_is _nid, optional _method)

{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_CLOCK_DIVIDE = 3;
    private _N_CLOCK_SOURCE = 4;
    private _N_CLOCK = 5;
    private _N_SYNCHRONIZE = 6;
    private _N_START_EVENT = 7;
    private _N_INIT_ACTION= 8;

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

    _name = DevNodeRef(_nid, _N_NAME);
    _found = DevNodeCvt(_nid, _N_CLOCK_SOURCE, ['EVENT 1', 'EVENT 2','EVENT 3','EVENT 4','EVENT 5', '1MHz', '100KHz', '10KHz', '1KHz', '100Hz'],[6,7,8,9,10,11,12,13,14,15], _clock_source = 0);
    _clock_divide = long(data(DevNodeRef(_nid, _N_CLOCK_DIVIDE)));
    if(_clock_divide < 1 && _clock_divide > 16)
	DevLogErr(_nid, "Wrong clock division value");
    if(_clock_divide == 16) 
	_clock_divide = 0;
    _master_reg = (1 << 15) | (1 << 14) | (1 << 13) | (_clock_divide << 8) | (_clock_source << 4);
    _found = DevNodeCvt(_nid, _N_SYNCHRONIZE, ['ENABLED', 'DISABLED'],[1,0], _synchronize = 0);

/* Disable pointer sequence */
    _w = 0xffe8;	
    _status=DevCamChk(_name, CamPiow(_name, 0,16, _w, 16), 1,*); 

/* Disable Ferquency Scaler */
    if(_synchronize)
    {	
		_w = 0;
   		_status=DevCamChk(_name, CamPiow(_name, 2, 16, _w,16), 1,*); 
   		_status=DevCamChk(_name, CamPiow(_name, 2, 0, _w,16), 1,*); 
    }
    else
    {
		_w =  word(1 << 5);
   		_status=DevCamChk(_name, CamPiow(_name, 2, 16, _w, 16),1,*); 
        _status=DevCamChk(_name, CamPiow(_name, 2, 0, _w, 16),1,*); 
    }

/*Set data pointer to master register */
    _w = word(23);
   _status=DevCamChk(_name, CamPiow(_name, 0,16,_w,16), 1,*); 

/*Set 16 bit transfer */
    _w = word(239); 
    _status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 

/*Write Master register */
    _w =  word(_master_reg);
    _status=DevCamChk(_name, CamPiow(_name, 1, 16, _w, 16),1,*); 

    _chan_flags = 0;
    _tot_events = 0;
    _set_low_mask = 0;
    for(_chan = 0; _chan < 5; _chan++)
    {
		_chan_nid = _N_CHANNEL_0 + (_chan * _K_NODES_PER_CHANNEL);
		if(DevIsOn(DevNodeRef(_nid,  _chan_nid)))
		{
			_chan_flags = _chan_flags | (1 << _chan);
	/*Write event codes associated with channel */
			_datalen = getnci(DevNodeRef(_nid, _chan_nid + _N_CHAN_EVENTS), 'length');
			if(_datalen == 0) /* No events defined */
				_num_events = 0;
			else
			{
	    		_event_codes =  long(data(DevNodeRef(_nid, _chan_nid + _N_CHAN_EVENTS)));
	    		_num_events = size(_event_codes);
			}
			for(_i = 0; _i < _num_events; _i++)
			{
				if(_event_codes[_i] > 0)
				{
					_w = word(_event_codes[_i] | (1 << (7 + _chan)));
    				_status=DevCamChk(_name, CamPiow(_name, _tot_events,17, _w, 16),1,*);
					_tot_events++;
				}
			}
			_load = long(data(DevNodeRef(_nid, _chan_nid + _N_CHAN_LOAD)));
			if(_load < 0 || _load > 65535)
				DevLogErr(_nid, "Wrong load value: "//_load);
			_hold = long(data(DevNodeRef(_nid, _chan_nid + _N_CHAN_HOLD)));
			if(_hold < 0 || _hold > 65535)
				DevLogErr(_nid, "Wrong hold value: "//_hold);
	/*Disarm counters */
			_w = (6 << 5) | (1 << _chan) | 0xff00;	
   			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 
	/* Write load and hold registers */
			_w = (_chan + 1) | (1 << 3) | 0xff00;
   			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 
   			_status=DevCamChk(_name, CamPiow(_name, 1, 16, _load, 16), 1,*); 
			_w = (_chan + 1) | (2 << 3);
			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 
 			_status=DevCamChk(_name, CamPiow(_name, 1, 16, _hold, 16), 1,*); 

	/* Load counter */
			_w = (2 << 5) | (1 << _chan) | 0xff00;
			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_GATING, 
				['NONE','TCN-1','GATE N+1','GATE N-1','GATE N','TRIGGER RISING','TRIGGER FALLING'],
				[0,1,2,3,4,6,7], _chan_gating = 0);
    		DevNodeCvt(_nid, _chan_nid + _N_CHAN_CLOCK_SOURCE, 
				['TCN-1','1MHz', '100KHz','10KHz','1KHz','100Hz','EVENT 1','EVENT 2','EVENT 3','EVENT 4','EVENT 5'],
				[0,1,2,3,4,5,6,7,8,9,10], _chan_clock_source = 0);
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_CLOCK_EDGE, 
				['RISING', 'FALLING'], [0,1], _chan_clock_edge = 0);
   			DevNodeCvt(_nid, _chan_nid + _N_CHAN_SPECIAL_GATE, 
				['DISABLED','ENABLED'], [0,1], _chan_special_gate = 0);
   			DevNodeCvt(_nid, _chan_nid + _N_CHAN_DOUBLE_LOAD, 
				['DISABLED','ENABLED'], [0,1], _chan_double_load = 0);
   			DevNodeCvt(_nid, _chan_nid + _N_CHAN_REPEAT_COUNT, 
				['DISABLED','ENABLED'], [0,1], _chan_repeat_count = 0);
   			DevNodeCvt(_nid, _chan_nid + _N_CHAN_COUNT_MODE, 
				['BINARY','BCD'], [0,1], _chan_count_mode = 0);
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_COUNT_DIR, 
				['DESCENDING', 'ASCENDING'], [0,1], _chan_count_direction = 0);
   			DevNodeCvt(_nid, _chan_nid + _N_CHAN_OUTPUT_MODE, 
				['ALWAYS HIGH','LOW PULSES','TOGGLE: INITIAL HIGH','TOGGLE: INITIAL LOW','ALWAYS LOW','HIGH PULSES'],
	 			[0,1,2,3,4,5], _chan_output_mode = 0);

			if(_chan_output_mode == 3)
			{
				_chan_output_mode--;
				_set_low_mask = _set_low_mask | (1 << _chan);
			}
			_chan_mode = _chan_output_mode | (_chan_count_direction<<3)|(_chan_count_mode<<4)|(_chan_repeat_count<<5)|
			(_chan_double_load<<6)|(_chan_special_gate<<7)|(_chan_clock_source<<8)|(_chan_clock_edge<<12)|(_chan_gating<<13);
			_w = word((_chan + 1)|0xFF00);
    		_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16), 1,*); 
  			_status=DevCamChk(_name, CamPiow(_name, 1, 16, _chan_mode, 16), 1,*); 
		}
    }
    if(_synchronize)
    {
    	_start_event = long(data(DevNodeRef(_nid, _N_START_EVENT)));
		_w = _start_event | (1 << 12);
    	_status=DevCamChk(_name, CamPiow(_name, _tot_events,17, _w, 16),1,*);
		_tot_events++;
    }
    while(_tot_events < 16)
    {
		_w = 0;
    	_status=DevCamChk(_name, CamPiow(_name, _tot_events,17, _w, 16),1,*);
		_tot_events++;
    }	    
/*Enable code recognizer if any event */
    if(_tot_events > 0)
    {
		_w = 0;
    	_status=DevCamChk(_name, CamPiow(_name, 0, 26, _w, 16),1,*);
    }
/*Disarm All channels */
   _w = word(((6<<5)|31)|0xFF00);
   _status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16),1,*);

/*Se Ouptut low for channels defined as initial low level */
    for(_chan = 0; _chan < 5; _chan++)
    {
		if(((_set_low_mask & (1 << _chan))!= 0)|| ((_chan_flags & (1 << _chan))== 0))
		{
			_w = (_chan + 1) |(29<<3) | 0xFF00;
 			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16),1,*);
		
		}
		else
		{
			_w = (_chan + 1) |(28<<3) | 0xFF00;
   			_status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16),1,*);
		
		}
    }

/*Arm seleced channels */
    _w = (1 << 5) | _chan_flags | 0xFF00;
    _status=DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16),1,*);
    return (1);
}

