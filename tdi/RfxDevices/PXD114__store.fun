public fun PXD114__store(as_is _nid, optional _method)
{
 
	private _N_HEAD = 0;
	private _N_NAME = 1;
	private _N_COMMENT = 2;
	private _N_TRIG_SOURCE = 3;
	private _N_TRIG_LEVEL = 4;
	private _N_TRIG_EDGE = 5;
	private _N_TRIG_COUPL = 6;
	private _N_TRIG_IMP = 7;
	private _N_TRIG_TIME = 8;
	private _N_NUM_SEGMENTS = 9;
	private _N_SEGMENT_TIME = 10;
	private _N_SEG_TIME_OFS = 11;
	private _N_ACT_SEG_LEN = 12;
	private _N_ACT_SAMP_FRQ = 13;

	private _N_CHANNEL_0 = 14;

	private _K_NODES_PER_CHANNEL = 6;
	private _N_CHAN_INPUT_IMP = 1;
	private _N_CHAN_RANGE = 2;
	private _N_CHAN_OFFSET = 3;
	private _N_CHAN_COUPLING = 4;
	private _N_CHAN_DATA = 5;

	private _N_TRIGGERS = 38;

	private _INVALID = 10E20;
write(*, 'Start PDX114 STORE');
	_name = if_error(data(DevNodeRef(_nid, _N_NAME)), 'INVALID');
	if(_name == 'INVALID')
	{
    	DevLogErr(_nid, "Invalid board name");
		abort();
	}

	_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_TIME)), _INVALID);
	if(_trig_time == _INVALID)
	{
    	DevLogErr(_nid, "Invalid trigger time specification");
		abort();
	}

	_act_seg_len = if_error(data(DevNodeRef(_nid, _N_ACT_SEG_LEN)), _INVALID);
	if(_act_seg_len == _INVALID)
	{
    	DevLogErr(_nid, "Invalid segment length");
		abort();
	}

	_seg_time_ofs = if_error(data(DevNodeRef(_nid, _N_SEG_TIME_OFS)), _INVALID);
	if(_seg_time_ofs == _INVALID)
	{
    	DevLogErr(_nid, "Invalid segment offset");
		abort();
	}

	_act_samp_freq = if_error(data(DevNodeRef(_nid, _N_ACT_SAMP_FRQ)), _INVALID);
	if(_act_samp_freq == _INVALID)
	{
    	DevLogErr(_nid, "Invalid sampling frequency ");
		abort();
	}
	_num_segments = if_error(data(DevNodeRef(_nid, _N_NUM_SEGMENTS)), _INVALID);
	if(_num_segments == _INVALID)
	{
    	DevLogErr(_nid, "Invalid number of segments ");
		abort();
	}

    _seg1 = zero(_num_segments * _act_seg_len, 0W);
    _seg2 = zero(_num_segments * _act_seg_len, 0W);
    _seg3 = zero(_num_segments * _act_seg_len, 0W);
    _seg4 = zero(_num_segments * _act_seg_len, 0W);
   _trig_times = zero(_num_segments, FT_FLOAT(0));


    _handle = LECROY_PXD114->PXDOpen(_name);
	if(_handle == 0)
	{
    	DevLogErr(_nid, "Cannot open device "// _name);
		abort();
	}

    _status = LECROY_PXD114->PXDStore(
		long(_handle), 
		_act_seg_len,
		ref(_num_segments),
		ref(_trig_times), 
		ref(_seg1),
		ref(_seg2),
		ref(_seg3),
		ref(_seg4));


write(*, 'Act. seg. len: ', _act_seg_len);
write(*, 'Num segments: ', _num_segments);

	if(_status == -1)
	{
    	DevLogErr(_nid, "Not all triggers received yet");
		abort();
	}
	if(_status != 1)
	{
    	DevLogErr(_nid, "Error reading device samples");
		abort();
	}


	DevPut(_nid, _N_TRIGGERS, _trig_times);


	_period = 1./_act_samp_freq;
	_seg_time = _act_seg_len / FT_FLOAT(_act_samp_freq);

	_start_times = _trig_time + _seg_time_ofs;
	_end_times = _start_times + _seg_time;
	_periods = ft_float(_period);



	for(_s = 1; _s < _num_segments; _s++)
	{
		_start_time = _trig_time + _trig_times[_s] - _trig_times[0] + _seg_time_ofs;
		_end_time = _start_time + _seg_time;
		_start_times = [_start_times, _start_time];
		_end_times = [_end_times, _end_time];
		_periods = [_periods, _period];
	}

	_clock = make_range(_start_times, _end_times, _periods);
	_dim = make_dim(make_window(0, (_act_seg_len * _num_segments) - 1, ft_float(_trig_time)), _clock);

	_curr_range = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(0 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE)), _INVALID);
	if(_curr_range == _INVALID)
	{
    	DevLogErr(_nid, "Invalid range for channel 1");
		abort();
	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(0 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, 0, _curr_range/8192., word(_seg1), 0, _act_seg_len * _num_segments, _dim);
	if(! _status)
	{
		DevLogErr(_nid, 'Error writing data in pulse file');

	}

	_curr_range = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(1 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE)), _INVALID);
	if(_curr_range == _INVALID)
	{
    	DevLogErr(_nid, "Invalid range for channel 2");
		abort();
	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(1 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, 0, _curr_range/8192., word(_seg2), 0, _act_seg_len * _num_segments, _dim);
	if(! _status)
	{
		DevLogErr(_nid, 'Error writing data in pulse file');

	}


	_curr_range = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(2 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE)), _INVALID);
	if(_curr_range == _INVALID)
	{
    	DevLogErr(_nid, "Invalid range for channel 3");
		abort();
	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(2 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, 0, _curr_range/8192., word(_seg3), 0, _act_seg_len * _num_segments, _dim);
	if(! _status)
	{
		DevLogErr(_nid, 'Error writing data in pulse file');

	}

	_curr_range = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(3 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_RANGE)), _INVALID);
	if(_curr_range == _INVALID)
	{
    	DevLogErr(_nid, "Invalid range for channel 4");
		abort();
	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(3 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, 0, _curr_range/8192., word(_seg4), 0, _act_seg_len * _num_segments, _dim);
	if(! _status)
	{
		DevLogErr(_nid, 'Error writing data in pulse file');

	}
write(*, 'End PDX114 STORE');
    return(1);	

}
			



