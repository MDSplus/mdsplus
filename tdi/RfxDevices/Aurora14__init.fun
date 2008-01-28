public fun Aurora14__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 61;
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_TRIG_MODE = 3;
    private _N_TRIG_SOURCE = 4;
    private _N_CLOCK_MODE = 5;
    private _N_CLOCK_SOURCE = 6;
    private _N_FREQUENCY = 7;
    private _N_OP_MODE = 8;
    private _N_USE_TIME = 9;
    private _N_PTS = 10;
    private _K_NODES_PER_CHANNEL = 8;
    private _N_CHANNEL_0= 11;
    private _N_CHAN_RANGE = 1;
    private _N_CHAN_POLARITY = 2;
    private _N_CHAN_START_TIME = 3;
    private _N_CHAN_END_TIME = 4;
    private _N_CHAN_START_IDX = 5;
    private _N_CHAN_END_IDX = 6;
    private _N_CHAN_DATA = 7;
  
    private _512K = 524288;
    private _128K = 131072;
	
	_error = 0;

    _name = DevNodeRef(_nid, _N_NAME);
/*Master Reset */
    DevCamChk(_name, CamPiow(_name, 1, 9, _dummy=0, 16),1,*); 

/*Set CMD mode */
    DevCamChk(_name, CamPiow(_name, 1, 24, _dummy=0, 16),1,*); 

/* Read status reg */
    _status_reg = 0;
    _status=DevCamChk(_name, CamPiow(_name, 4, 1, _status_reg,24),1,*); 
    if(_status_reg & (1 << 15))
	_mem_size = _512K;
    else
	_mem_size = _128K;

/* Clock stuff  */
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [1,0], _int_clock = 0);
    if(!_int_clock)
    {
        _clk = DevNodeRef(_nid, _N_CLOCK_SOURCE); 
		_clock_val = if_error(execute('`_clk'),(DevLogErr(_nid, "Cannot resolve clock"); abort();));
		_clk = 0;
    }
    else
    {
         DevNodeCvt(_nid, _N_FREQUENCY, [1E6,0.5E6,0.25E6,0.1E6,0.5E5,0.25E5,0.1E5],[0,1,2,3,4,5,6], _clk = 0);
         _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
         _clock_val = make_range(*,*,1./ _freq);
    	 DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
    }

/* Trigger stuff */
    DevNodeCvt(_nid, _N_TRIG_MODE, ['SOFTWARE', 'EXTERNAL'],[1,0], _int_trig = 0);
	
    if(_int_trig)
		_trig = 0.0;
    else
	{
		_trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE))  , _error = 1);

		if( _error )
		{
			DevLogErr(_nid, "Cannot resolve trigger"); 
			abort();
		}
	}


/* Read burst/normal mode */
    DevNodeCvt(_nid, _N_OP_MODE, ['NORMAL', 'BURST'],[1,3], _op_mode = 0);

/* Read ADC Range configuration for all channels */
    _range_reg = 0;
    _status=DevCamChk(_name, CamPiow(_name, 5, 1, _range_reg,24),1,*);
    for(_c = 0; _c < 6; _c++)
    {
	if((_range_reg & (1 << (3 * _c))) != 0)
	    _range = 5;
	else
	    _range = 2.5;
	if((_range_reg & (2 << (3 * _c))) != 0)
	    _range = 2 * _range;
		if((_range_reg & (4 << ( 3* _c))) != 0)
			_polarity = 'BIPOLAR';
		else
			_polarity = 'UNIPOLAR';
    	DevPut(_nid, _N_CHANNEL_0 + (_c * _K_NODES_PER_CHANNEL) + _N_CHAN_RANGE, _range);
    	DevPut(_nid, _N_CHANNEL_0 + (_c * _K_NODES_PER_CHANNEL) + _N_CHAN_POLARITY, _polarity);
    }
    DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
    _pts = 0;
    for(_i = 0; _i < 6; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
            if(_time_cvt)
            {
		_curr_end = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_TIME)),(DevLogErr(_nid, "Cannot resolve start time"); abort();));
		if(_curr_end > 0)
	    	    _curr_pts = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_end + _trig);
		else
	    	    _curr_pts = - x_to_i(build_dim(build_window(0,*,_trig + _curr_end), _clock_val),  _trig);


        DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX, long(_curr_pts));
		
		
		_curr_start = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_TIME)),(DevLogErr(_nid, "Cannot end time"); abort();));
		if(_curr_start > 0)
	    	    _curr_start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_start + _trig);
		else
	    	    _curr_start_idx =  - x_to_i(build_dim(build_window(0,*,_trig + _curr_start ), _clock_val),_trig);

        DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX, long(_curr_start_idx));


            }
            else 
		_curr_pts = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
	    if(_curr_pts > _pts) _pts = long(_curr_pts);
        }
    }
    _max_chan_samples = _mem_size;
    if(_pts > _max_chan_samples)
    {
        DevLogErr(_nid, 'Too many samples ('//_pts//'). Truncated.');
        _pts = _max_chan_samples;
    }

    if(_mem_size == _512K)
	_pts_block = 2048;
    else
	_pts_block = 1024;
	
    _kpts = _pts / _pts_block;
    if(_pts != (_kpts * _pts_block))
	_kpts = _kpts + 1;
    DevPut(_nid, _N_PTS, _kpts);
    _status=DevCamChk(_name, CamPiow(_name, 3, 17, _kpts,16),1,*); 

/*Write control register */
    _control_reg = 	/*(1 << 7) |(7 << 12) |*/ _clk | (_int_clock << 3) | (_op_mode << 8);
    _status=DevCamChk(_name, CamPiow(_name, 4, 17, _control_reg,24),1,*); 

/* Set DAQ Mode */
    _status=DevCamChk(_name, CamPiow(_name, 1,26, _dummy = 0,24),1,*); 

/*Start digitizing if not BURST mode */

    if(_op_mode != 3)
	_status = DevCamChk(_name, CamPiow(_name, 0,25, _dummy=0, 16),1,*); 
     return (1);
}


