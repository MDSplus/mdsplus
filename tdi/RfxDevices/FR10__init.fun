public fun FR10__init(as_is _nid, optional _method)
{




    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _N_COMMENT = 4;

    private _N_CLOCK_SOURCE = 5;
    private _N_CLOCK_MODE = 6;
    private _N_FREQUENCY = 7;

    private _N_TRIG_SOURCE = 8;
    private _N_TRIG_MODE = 9;
    private _N_TRIG_EDGE = 10;

    private _N_PTS = 11;

    private _N_USE_TIME = 12;

    private _K_NODES_PER_CHANNEL = 6;
    private _N_CHANNEL_0= 13;
    private _N_CHAN_START_TIME = 1;
    private _N_CHAN_END_TIME = 2;
    private _N_CHAN_START_IDX = 3;
    private _N_CHAN_END_IDX = 4;
    private _N_CHAN_DATA = 5;

    private _N_INIT_ACTION = 109;
    private _N_STORE_ACTION = 110;

    private _2M = 2097152;
    private _INVALID = 10E20;



    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 		abort();
    }


    DevNodeCvt(_nid, _N_TRIG_EDGE, ['RISING', 'FALLING'], [0,1], _trig_edge = 0);

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);
	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
	}
    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_trig = 0);
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_clock = 0);
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
		_clk_div = 0;
    }
    else
    {
        DevNodeCvt(_nid, _N_FREQUENCY, [200E3,100E3,50E3,20E3,10E3,5E3,2E3,1E3,500,200,100],[1,2,4,10,20,40,100,200,400,1000,2000], _clk_div = 0);

        _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
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
    for(_i = 0; _i < 16; _i++)
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
        }
    }
    if(_pts > _2M)
    {
        DevLogErr(_nid, 'Too many samples. Truncated.');
        _pts = _2M;
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
	    _status = MdsValue('FR10HWInit(0, $1, $2, $3)', _board_id, _clk_div, _pts);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error Initializing FR10 device: CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = FR10HWInit(_nid, _board_id, _clk_div, _pts, _ext_trig, _trig_edge);
		if(_status == 0)
			abort();
	}
	return(1);
}
			



