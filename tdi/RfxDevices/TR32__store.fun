public fun TR32__store(as_is _nid, optional _method)
{
 
 	private _N_HEAD = 0;
	private _N_BOARD_ID = 1;
	private _N_SW_MODE = 2;
	private _N_IP_ADDR = 3;
	private _N_COMMENT = 4;
	private _N_CLOCK_MODE = 5;
	private _N_TRIG_MODE = 6;
	private _N_TRIG_SOURCE = 7;
	private _N_CLOCK_SOURCE = 8;
	private _N_INT_FREQUENCY = 9;
	private _N_CK_RESAMPLING = 10;
	private _N_CK_TERMINATION = 11;
	private _N_USE_TIME = 12;
	private _N_PTS = 13;
	private _N_TRIG_EDGE = 14;
	private _N_CHANNEL_0= 15;

	private _K_NODES_PER_CHANNEL = 7;
	private _N_CHAN_RANGE = 1;
	private _N_CHAN_START_TIME = 2;
	private _N_CHAN_END_TIME = 3;
	private _N_CHAN_START_IDX = 4;
	private _N_CHAN_END_IDX = 5;
	private _N_CHAN_DATA = 6;

	private _64M = 67108864;
	private _INVALID = 10E20;


 
	_max_samples = _64M;

     _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 		abort();
    }

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	}

    _trig = data(DevNodeRef(_nid, _N_TRIG_SOURCE));
    _clock = evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE));
    _clock = execute('evaluate(`_clock)');
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0, 1], _ext_clock = 0);
    if(_ext_clock)
    {
    	_clk_div = if_error(DevNodeRef(_nid, _N_CK_RESAMPLING), _INVALID);
    	if(_clk_div != _INVALID && _clk_div != 1 )
	{
	    _clock = make_range(,,slope_of(_clock)*_clk_div);
	}
     }
    _pts = data(DevNodeRef(_nid, _N_PTS));

	if(_remote)
	{
	    _handle = MdsValue('TR32HWStartStore(0, $1, $2)', _board_id, _pts);
		if(_handle == -1)
		{
			DevLogErr(_nid, 'TR32 device not in STOP state. Board ID: '//_board_id);
			abort();
		}
	}
	else
	{
		_handle = TR32HWStartStore(_nid, _board_id, _pts);
		if(_handle == -1)
		{
			DevLogErr(_nid, 'TR32 device not in STOP state. Board ID: '//_board_id);
			abort();
		}
	}
    
	for(_i = 0; _i < 4; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
			_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
			_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
	/*Check and correct memory overflow or wrong setting*/
			if(_end_idx > _pts) _end_idx = _pts;
			if(_end_idx  - _start_idx >  _max_samples) _start_idx = _pts - _max_samples;
			if(_end_idx < _start_idx) _start_idx = _end_idx - 1;

	/* Read data */
			if(_remote)
			{
				_data = MdsValue('TR32HWReadChan($1, $2, $3, $4, $5)', _handle, (_i + 1), _start_idx, _end_idx, _pts);	
			}
			else
			{
				_data = TR32HWReadChan(_handle, _i + 1, _start_idx, _end_idx, _pts);					  
			}						

			_range = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE));	


	/* Build signal */
			_dim = make_dim(make_window(_start_idx, _end_idx - 1, _trig), _clock);
			_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;

			_status = DevPutSignal(_sig_nid, 0, _range/32768., word(_data), 0, _end_idx - _start_idx - 1, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');

			}
		}
    }


	if(_remote)
	{
		MdsValue('TR32HWClose($1)', _handle);
		MdsDisconnect();
	}
	else
		TR32HWClose(_handle);

    return(1);
}


