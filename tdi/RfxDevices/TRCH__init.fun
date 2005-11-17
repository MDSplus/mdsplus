public fun TRCH__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_CHANNELS = 3;
    private _N_CHAN_OFFSET = 4;
    private _N_CLOCK_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_CLOCK_SOURCE = 7;
    private _N_FREQUENCY = 8;
    private _N_USE_TIME = 9;
    private _N_PTS = 10;

    private _K_NODES_PER_CHANNEL = 6;
    private _N_CHANNEL_0= 11;
    private _N_CHAN_START_TIME = 1;
    private _N_CHAN_END_TIME = 2;
    private _N_CHAN_START_IDX = 3;
    private _N_CHAN_END_IDX = 4;
    private _N_CHAN_DATA = 5;
    private _N_INIT_ACTION = 47;
    private _N_STORE_ACTION = 48;
    private _3M = 3145728;

    _name = DevNodeRef(_nid, _N_NAME);

    DevCamChk(_name, CamPiow(_name, 0,28, _dummy=0, 16),1,1); 

    DevNodeCvt(_nid, _N_CHANNELS, [3,6], [0,1], _chans = 0);
    _offset = word(DevNodeRef(_nid, _N_CHAN_OFFSET));
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_clock = 0);
    if(_ext_clock)
    {
        _clk = if_error(DevNodeRef(_nid, _N_CLOCK_SOURCE), (DevLogErr(_nid, "Cannot resolve clock"); abort();));
	_clock_val = execute('`_clk');
	_clk = 0;
    }
    else
    {
        DevNodeCvt(_nid, _N_FREQUENCY, [1E6, 5E5, 250E3,125E3,50E3,10E3,5E3],[1,2,3,4,5,6,7], _clk = 0);
        _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
        _clock_val = make_range(*,*,1./ _freq);
    	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
   }
    _control_reg = word(_clk) | (word(_chans) << 4) | (word(_offset) << 7);
    _status=DevCamChk(_name, CamPiow(_name, 2,16, _control_reg,16),1,*); 

    _trig=if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (DevLogErr(_nid, "Cannot resolve trigger"); abort();));
    _num_chans = data(DevNodeRef(_nid, _N_CHANNELS));
    _max_chan_samples = _3M/_num_chans;
    _pts = 0;
   DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
   _pts = 0;
    for(_i = 0; _i < _num_chans; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
        	if(_time_cvt)
        	{
				_curr_end = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_TIME));
				if(_curr_end > 0)
	    			_curr_pts = x_to_i(build_dim(build_window(0,*,d_float(_trig)), _clock_val), d_float(_curr_end + _trig));
				else
	    			_curr_pts = - x_to_i(build_dim(build_window(0,*,d_float(_trig + _curr_end)), _clock_val),  d_float(_trig));

				DevPut(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX, long(_curr_pts));
				_curr_start = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_TIME));
				if(_curr_start > 0)
	    			_curr_start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_start + _trig);
				else
	    			_curr_start_idx =  -x_to_i(build_dim(build_window(0,*,d_float(_trig + _curr_start) ), _clock_val),d_float(_trig));
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
