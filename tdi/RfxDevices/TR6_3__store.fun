public fun TR6_3__store(as_is _nid, optional _method)
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

	private _INVALID = 1E20;

    _name = DevNodeRef(_nid, _N_NAME);

/* Get mode of operation and control register */
    DevNodeCvt(_nid, _N_OP_MODE, ['NORMAL', 'BURST'],[0,1], _burst = 0);

/* Read register 1 and 2 */
   DevCamChk(_name, CamPiow(_name, 0,0, _status1=0, 16),1,*);
   DevCamChk(_name, CamPiow(_name, 1,0, _status2=0, 16),1,*);

/* Check STOP state */
	if(_burst)
	{
		DevCamChk(_name, CamPiow(_name, 0,27, _w=0, 16),*,*);
		if (!CamQ())
    	{
            DevLogErr(_nid, 'Module is not in STOP state');
            abort();
    	}
		DevCamChk(_name, CamPiow(_name, 2,25, _w=0, 16),*,*);
	}
	else
	{
		if(((_status2 >> 6) & 1)!= 0)
    	{
            DevLogErr(_nid, 'Module is not in STOP state');
            abort();
    	}
	}

/* Read and store channel range and polarity for channels 1-4 */
    DevCamChk(_name, CamPiow(_name, 2, 0, _w=0, 16),1,*); 
	for(_i = 0; _i < 4; _i++)
	{
		_range_code = ((_w >> ((_i * 3) + 1)) & 3);
		if(_range_code == 0)
			_range = 5.;
		else if (_range_code == 1)
			_range = 10.;
		else
			_range = 20.;

		DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE, _range);

		_polarity_code =  ((_w >> (_i * 3)) & 1);
		if(_polarity_code)
			DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_POLARITY, "BIPOLAR");
		else
			DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_POLARITY, "UNIPOLAR");


	}

/* Read and store channel range for channels 5 and 6 */
    DevCamChk(_name, CamPiow(_name, 3, 0, _w=0, 16),1,*); 
	for(_i = 4; _i < 6; _i++)
	{
		_range_code = ((_w >> (((_i - 4) * 3) + 1)) & 3);
		if(_range_code == 0)
			_range = 5.;
		else if (_range_code == 1)
			_range = 10.;
		else
			_range = 20.;

		DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE, _range);

		_bipolar =  ((_w >> ((_i - 4) * 3)) & 1);
		if(_bipolar)
			DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_POLARITY, "BIPOLAR");
		else
			DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_POLARITY, "UNIPOLAR");

	}

/* Get trigger time */
    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), 0);
    _num_triggers = size(_trig);

    _pts_samples = DevNodeRef(_nid, _N_PTS); 
    _pre_samples = DevNodeRef(_nid, _N_PRE_SAMPLES); 

/* Get Clock */
	_clock = if_error(evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE)), _INVALID);
/*	if(data(_clock) == _INVALID)
	{
		DevLogErr(_nid, 'Cannot resolve clock');
		abort();
	}*/
    _clock = execute('evaluate(`_clock)');

    for(_chan = 0; _chan < 6; _chan++)
    {
		if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_chan *  _K_NODES_PER_CHANNEL))))
		{ 
		/* Enable Readout */
			DevCamChk(_name, CamPiow(_name, 0, 17, _w=0, 16),1,*); 

			if(_burst == 0)
			{
				_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));
				_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
			/*Check and correct memory overflow or wrong setting*/
				if(_end_idx > _pts_samples) _end_idx = _pts_samples;

				if(_end_idx < _start_idx) _start_idx = _end_idx - 1;
				_samples = _pts_samples + _pre_samples + 1;
			}
			else
			{
				_samples = _num_triggers * _pts_samples;
				_start_idx = 0;
				_end_idx = _samples - 1;
			}

			DevCamChk(_name, CamQstopw(_name, _chan, 2, _samples, _data=0, 16), 1, *);
			if(_burst == 1 && _num_triggers > 0)
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
				_dim = make_dim(make_window(- _pre_samples, _end_idx, _trig), _clock);

			if(_bipolar)
			{
	  			_min = -_range/2.;
				_max = _range/2.;
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

