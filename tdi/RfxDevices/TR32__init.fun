public fun TR32__init(as_is _nid, optional _method)
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


	_board_id = if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
	if(_board_id == _INVALID)
	{
    		DevLogErr(_nid, "Invalid Board ID specification");
		abort();
	}

	DevNodeCvt(_nid, _N_TRIG_EDGE, ['RISING', 'FALLING'], [0, 1], _trig_edge = 0);
	DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0, 1], _remote = 0);
	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    		DevLogErr(_nid, "Invalid Crate IP specification");
 		    	abort();
		}
	}

	DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0, 1], _ext_trig = 0);

	DevNodeCvt(_nid, _N_CK_TERMINATION, ['ENABLED', 'DISABLED'], [1, 0], _clock_term = 0);

	DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0, 1], _ext_clock = 0);
    if(_ext_clock)
    {
		_status = 1;
        _clk = if_error(DevNodeRef(_nid, _N_CLOCK_SOURCE), _status = 0);
		if(_status == 0)
		{
	        DevLogErr(_nid, "Cannot resolve clock"); 
			abort();
  		}
		_clock_val = execute('`_clk');
		_clk_div = if_error(DevNodeRef(_nid, _N_CK_RESAMPLING), _INVALID);
		if(_clk_div == _INVALID)
		{
    		DevLogErr(_nid, "Cannot resolve clock resampling in external clock");
 			abort();
		}
		if(_clk_div > 1)
		{
			_clock_val = make_range(,,slope_of(_clock_val)*_clk_div);
		}

    }
    else
    {
        DevNodeCvt(_nid, _N_INT_FREQUENCY, [3E6,1.5E6,1E6,500E3,200E3,100E3,50E3,20E3,10E3,5E3,2E3,1E3,500],/*[1,2,3,6,15,30,60,150,300,600,1500, 3000,6000], _clk_div = 0);*/
[2,4,6,12,30,60,120,300,600,1200,3000, 6000,12000], _clk_div = 0);

        _freq = data(DevNodeRef(_nid, _N_INT_FREQUENCY));

        _clock_val = make_range(*,*,1./ _freq);
    	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
	}
    _trig=if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);
    if(_trig == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve trigger ");
 		abort();
    }

    DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
    _pts = 0;
    _ranges = [];
    for(_i = 0; _i < 4; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
        	if(_time_cvt)
        	{
				_curr_end = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_TIME)), _INVALID);
				if(_curr_end == _INVALID)
				{
					DevLogErr(_nid, "Cannot resolve end time"); 
					abort();
				}
				if(_curr_end > 0)
	    			_curr_pts = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_end + _trig);
				else
	    			_curr_pts = - x_to_i(build_dim(build_window(0,*,_trig + _curr_end), _clock_val),  _trig);
				DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX, long(_curr_pts));

				_curr_start = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_TIME)), _INVALID);
				if(_curr_start == _INVALID)
				{
					DevLogErr(_nid, "Cannot resolve start time"); 
					abort();
				}
				if(_curr_start > 0)
	    			_curr_start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_start + _trig);
				else
	    			_curr_start_idx =  - x_to_i(build_dim(build_window(0,*,_trig + _curr_start ), _clock_val),_trig);
				DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX, long(_curr_start_idx));
        	}
            else 
			{
				_curr_pts = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX)), _INVALID);	
				if(_curr_pts == _INVALID)
				{
					DevLogErr(_nid, "Cannot resolve end idx"); 
					abort();
				}
			}
			if(_curr_pts > _pts) _pts = long(_curr_pts);

			DevNodeCvt(_nid,  _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE, 
				[0.125,0.15625,0.25,0.3125,0.5,0.625,1,1.25,2,2.5,4,5,8,10],
				[0x130,0x030,0x120,0x020,0x110,0x010,0x100,0x000,0x112,0x012,0x113,0x013,0x103,0x003],
				 _range = 3);
			_ranges = [_ranges, _range];

        }
	else
	{
		_range = 0x003;
		_ranges = [_ranges, _range];
	}
    }
    if(_pts > _64M)
    {
        DevLogErr(_nid, 'Too many samples. Truncated.');
        _pts = _64M;
    }
    if(_pts < 0)
    {
        DevLogErr(_nid, 'Negative PTS value');
        abort();
    }
    DevPut(_nid, _N_PTS, _pts);


	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('TR32HWInit(0, $1, $2, $3, $4, $5, $6, $7, $8)', _board_id, _ext_clock,  _clk_div, _pts, _ext_trig, _trig_edge, _clock_term, _ranges);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error Initializing TR32 device: seet CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = TR32HWInit(_nid, _board_id, _ext_clock, _clk_div, _pts, _ext_trig, _trig_edge, _clock_term, _ranges);
		if(_status == 0)
			abort();
	}
	return(1);
}
			



