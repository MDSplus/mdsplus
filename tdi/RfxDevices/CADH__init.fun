public fun CADH__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_CHANNELS = 3;
    private _N_CLOCK_MODE = 4;
    private _N_TRIG_SOURCE = 5;
    private _N_CLOCK_SOURCE = 6;
    private _N_FREQUENCY = 7;
    private _N_USE_TIME = 8;
    private _N_PTS = 9;
    private _K_NODES_PER_CHANNEL = 6;
    private _N_CHANNEL_0= 10;
    private _N_CHAN_START_TIME = 1;
    private _N_CHAN_END_TIME = 2;
    private _N_CHAN_START_IDX = 3;
    private _N_CHAN_END_IDX = 4;
    private _N_CHAN_DATA = 5;
    private _256K = 262144;
    private _64K = 65536;

    private _INVALID = 10E20;

    _data_invalid = 0;

     _name = DevNodeRef(_nid, _N_NAME);
    DevCamChk(_name, CamPiow(_name, 0,28, _dummy=0, 16),1,1); 

	_chans = 4;
    DevNodeCvt(_nid, _N_CHANNELS, [1,2,4], [0,1,2], _chans = 0);
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
	
	/* write(*, _clock_val); */
	
	/*_clk = 0;*/
	_clk_id = 0;
    }
    else
    {
        DevNodeCvt(_nid, _N_FREQUENCY, [500E3,250E3,125E3,50E3,10E3,5E3, 1E3,500],[1,2,3,4,5,6,7,8], _clk_id = 0);
        _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
        _clock_val = build_range(*,*,1./ _freq);
    	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
   }
    _control_reg = word(_clk_id) | (word(_chans) << 4);
    _status=DevCamChk(_name, CamPiow(_name, 2,16, _control_reg,24),1,*);
    _status = 1; 
    _trig=if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);
    if(_trig == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve trigger ");
 		abort();
     }
    _num_chans = data(DevNodeRef(_nid, _N_CHANNELS));
    _max_chan_samples = 65536/_num_chans;
    _pts = 0;
   DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
   _pts = 0;
    for(_i = 0; _i < _num_chans; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
        	if(_time_cvt)
        	{
				_curr_end = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_TIME)), _data_invalid = 1);
				if(_data_invalid)
				{
				    DevLogErr(_nid, "Cannot resolve end time"); 
				    abort();
				}
				if(_curr_end > 0)
	    			_curr_pts = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_end + _trig);
				else
	    			_curr_pts = - x_to_i(build_dim(build_window(0,*,_trig + _curr_end), _clock_val),  _trig);
				DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX, long(_curr_pts));
				_curr_start = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_TIME)), _data_invalid =
				1);
				if(_data_invalid)
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
				_curr_pts = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
			if(_curr_pts > _pts) _pts = long(_curr_pts);
        }
    }
    if(_pts > _max_chan_samples)
    {
        DevLogErr(_nid, 'Too many samples. Truncated.');
        _pts = _max_chan_samples;
    }
    if(_pts < 0)
    {
        DevLogErr(_nid, 'Negative PTS value');
        abort();
    }
    DevPut(_nid, _N_PTS, _pts);
    _status=DevCamChk(_name, CamPiow(_name, 1,16, _pts,24),1,*); 
    _status = DevCamChk(_name, CamPiow(_name, 0,11, _dummy=0, 16),1,1); 
    _status = DevCamChk(_name, CamPiow(_name, 0,25, _dummy=0, 16),1,1); 
    return (1);
}
