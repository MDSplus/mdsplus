public fun TR6_3__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 62;
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
    private _N_PRE_SAMPLES = 11;
    private _K_NODES_PER_CHANNEL = 8;
    private _N_CHANNEL_0= 12;
    private _N_CHAN_RANGE = 1;
    private _N_CHAN_POLARITY = 2;
    private _N_CHAN_START_TIME = 3;
    private _N_CHAN_END_TIME = 4;
    private _N_CHAN_START_IDX = 5;
    private _N_CHAN_END_IDX = 6;
    private _N_CHAN_DATA = 7;
  
    private _512K = 524288;
    private _128K = 131072;

    _name = DevNodeRef(_nid, _N_NAME);
/* Abort previous work */
    DevCamChk(_name, CamPiow(_name, 2, 25, _dummy=0, 16),1,*); 

/* Read status register */
    DevCamChk(_name, CamPiow(_name, 3, 0, _status=0, 16),1,*); 

    if((_status & (1 << 11)) != 0)
	_mem_size = _512K;
    else
	_mem_size = _128K;


/* Read burst/normal mode */
    DevNodeCvt(_nid, _N_OP_MODE, ['NORMAL', 'BURST'],[0,1], _burst= 0);


	_control_w1 = 0;
	_control_w2 = 0;

	if(_burst)
		_control_w2 = 8;
	else
		_control_w2 = 0;


/* Trigger stuff */
    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'],[1,0], _int_trig = 0);
    if(_int_trig == 0)
		_trig = 0;
    else
		_trig=if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (DevLogErr(_nid, "Cannot resolve trigger"); abort();));
			
/* Clock stuff  */

    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [1,0], _int_clock = 0);
    if(_int_clock)
    {
         DevNodeCvt(_nid, _N_FREQUENCY, [3E6,2E6,1E6,1E5],[0,1,2,3], _clk_code = 0);
		_control_w2 = _control_w2 | _clk_code;
         _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
 

        _clock_val = make_range(*,*,1./ _freq);
    	 DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
	_clock_ok = 1;
    }
    else
    {
		_control_w2 = _control_w2 | (1 << 2);
        _clk = DevNodeRef(_nid, _N_CLOCK_SOURCE); 
	_clock_ok = 1;
		_clock_val = if_error(execute('`_clk'),(DevLogErr(_nid, "Cannot resolve clock"); _clock_ok = 0;));
   }
   if(!_clock_ok)
   	abort();
    DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
    _pts = 0;
	_pre_samples = 0;
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
				{
	    			_curr_start_idx =  - x_to_i(build_dim(build_window(0,*,_trig + _curr_start ), _clock_val),_trig);
					if( (- _curr_start_idx) > _pre_samples)
						_pre_samples = - _curr_start_idx;
				}
				DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX, long(_curr_start_idx));
            }
            else
			{ 
				_curr_pts = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
				_curr_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
				if( (- _curr_start_idx) > _pre_samples)
					_pre_samples = - _curr_start_idx;
			}
			if(_curr_pts > _pts) _pts = long(_curr_pts);
        }
    }

	/* If BURST mode we need to define the memory portion used for each segment */
	if(_burst) 
	{
		if(_pre_samples > 0)
		{
			DevLogErr(_nid, "Pre trigger not supported in BURST mode");
			abort();
		}			
		_act_eights = 0;
		while((_act_eights * _mem_size / 8) < _pts)
			_act_eights++;
		_control_w1 = _control_w1 | ((_act_eights - 1) << 3);
		_pts = _act_eights * _mem_size /8;
	}
	else /* Otherwise define active memory as mem_size, and adjust pre_trigger samples */
	{
		_control_w1 = _control_w1 | (7 << 3);
		_pre_eights = 0;
		while((_pre_eights * _mem_size / 8) < _pre_samples)
		{
			_pre_eights++;
		}
		_control_w1  = _control_w1 | _pre_eights;
		_pre_samples = _pre_eights * _mem_size / 8;
	}

    DevPut(_nid, _N_PTS, _pts);
    DevPut(_nid, _N_PRE_SAMPLES, _pre_samples);


/* Write control words */
/*write(*, _control_w1, _control_w2);*/

    DevCamChk(_name, CamPiow(_name, 0, 16, _control_w1, 16),1,*); 
    DevCamChk(_name, CamPiow(_name, 1, 16, _control_w2, 16),1,*); 

/* Arm module */
    DevCamChk(_name, CamPiow(_name, 0, 9, _control_w2, 16),1,*); 

     return (1);
}









