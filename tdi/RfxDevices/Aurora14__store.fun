public fun Aurora14__store(as_is _nid, optional _method)
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
    _name = DevNodeRef(_nid, _N_NAME);

/* Get mode of operation and control register */
    DevNodeCvt(_nid, _N_OP_MODE, ['NORMAL', 'BURST'],[0,1], _burst_mode = 0);
    DevCamChk(_name, CamPiow(_name, 0,1, _control=0, 16),1,*);

/* Check if stopped, if not burst mode */
    if(_burst_mode == 0)
    {
    	_not_stopped = _control & 1;
    	if (_not_stopped != 0)
    	{
            DevLogErr(_nid, 'Module is not in STOP state');
            return (0);
    	}
    }
    else /*Burst mode, need to stop acquisition */
    	DevCamChk(_name, CamPiow(_name, 1,25, _dummy=0, 16),1,*);

/* When burst mode check for overflow */
    if(_burst_mode == 1 && ((_control & 2) != 0))
       DevLogErr(_nid, 'Memory overflow in BURST mode');
    if(_burst_mode == 1)
/* Stop digitizing in burst mode */
  	 DevCamChk(_name, CamPiow(_name, 1, 25, _dummy=0, 16),1,*);

/* Set CMD mode */
    DevCamChk(_name, CamPiow(_name, 1, 24, _dummy=0, 16),1,*);

/* Read ACD range configuration */
    DevCamChk(_name, CamPiow(_name, 5, 1, _range_reg=0, 24),1,*);
    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (DevLogErr(_nid, 'Cannot resolve trigger');abort();));
    _num_triggers = size(_trig);
    _clock = if_error(evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE)), (DevLogErr(_nid, 'Cannot resolve clock');abort();));
    _clock = execute('evaluate(`_clock)');
    DevCamChk(_name, CamPiow(_name, 3, 1, _pts=0, 24),1,*);

/* Read memory size */
    _status=DevCamChk(_name, CamPiow(_name, 4, 1, _status_reg = 0,16),1,*); 
    if(_status_reg & (1 << 15))
		_mem_size = _512K;
    else
		_mem_size = _128K;

/* Read current MAR */
    DevCamChk(_name, CamPiow(_name, 2, 1, _base_mar=0, 24),1,*);
    if(_mem_size == _512K)
		_pts_samples = _pts * 2048;
    else
		_pts_samples = _pts * 1024; 

/* Read range register */	
    _range_reg = 0;
    _status=DevCamChk(_name, CamPiow(_name, 5, 1, _range_reg,24),1,*);
    for(_chan = 0; _chan < 6; _chan++)
    {
		if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_chan *  _K_NODES_PER_CHANNEL))))
		{ 
			if(_burst_mode == 0)
			{
				_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));
				_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
			/*Check and correct memory overflow or wrong setting*/
				if(_end_idx > _pts_samples) _end_idx = _pts_samples;
				if(_end_idx  - _start_idx >  _mem_size) _start_idx = _pts_samples - _mem_size;
				if(_end_idx < _start_idx) _start_idx = _end_idx - 1;
				_samples = _end_idx - _start_idx + 1;
			}
			else
			{
				_samples = _num_triggers * _pts_samples;
				_start_idx = 0;
				_end_idx = _samples - 1;
			}
			if(_burst_mode == 1)
				_start_mar = 0;
			else
	    		_start_mar = _base_mar - (_pts_samples - _start_idx);
			if(_start_mar < 0)
			{
				if((_control & 2) != 0)
				DevLogErr(_nid, 'Memory did not recirculate');
				_start_mar = _start_mar + _mem_size;
			}
			_chan_reg = _chan;
			_status=DevCamChk(_name, CamPiow(_name, 6, 16, _chan_reg, 16),1,*);
			_status=DevCamChk(_name, CamPiow(_name, 2, 17, _start_mar, 24),1,*); 
			DevCamChk(_name, CamFstopw(_name, 0, 2, _samples, _data=0, 16), 1, *);
			if(_burst_mode == 1 && _num_triggers > 0)
			{
				_dt = slope_of(_clock);
				_start = _trig[0];
				_delta = _dt;
				for(_i = 1; _i < _num_triggers; _i++)
				{
					_delta = [_delta, _dt];
				}
				_end = _trig  + (_pts_samples - 1 ) * _dt;
				_dim = make_dim(make_window(_start_idx, _end_idx, _trig[0]), make_range(_trig, _end, _delta));
			}
			else
				_dim = make_dim(make_window(_start_idx, _end_idx, _trig), _clock);
	/*Define current range */
			if((_range_reg & (1 << (3 * _chan))) != 0)
				_range = 5;
			else
				_range = 2.5;
			if((_range_reg & (2 << (3 * _chan))) != 0)
				_range = 2 * _range;
			if((_range_reg & (4 << ( 3* _chan))) != 0)
			{
	  			_min = -_range;
				_max = _range;
			}
			else
			{
	  			_min = 0;
				_max = _range;
			}
			_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
			_status = DevPutSignal(_sig_nid,4096 * _min/(_max - _min), (_max - _min)/4096., word(_data), 0, _samples - 1, _dim);
		}
    }
    return(1);
}

