
public fun CADH__store(as_is _nid, optional _method)
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
    private _64K = 65536;


    _name = DevNodeRef(_nid, _N_NAME);
    DevCamChk(_name, CamPiow(_name, 2,0, _control=0, 16),1,1);
    _not_stopped = ((_control >> 13) &3);
    if (_not_stopped != 0)
    {
        DevLogErr(_nid, 'Module is not in STOP state');
        abort();
    }
	_buffer_size = _64K;
    _num_chans = data(DevNodeRef(_nid, _N_CHANNELS));
    _trig = data(DevNodeRef(_nid, _N_TRIG_SOURCE));
    _clock = evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE));
    _clock = execute('evaluate(`_clock)');
    _pts = data(DevNodeRef(_nid, _N_PTS));
    if(_pts == 0)
    {
        DevLogErr(_nid, 'PTS = 0. No data acquired');
        abort();
    }
       
    
    DevCamChk(_name, CamPiow(_name, 0,0,_base_mar=0, 24),1,1);
    for(_i = 0; _i < _num_chans; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
			_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
			_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
	/*Check and correct memory overflow or wron setting*/
			if(_end_idx > _pts) _end_idx = _pts;
			_max_samples = _buffer_size / _num_chans;

			if(_end_idx  - _start_idx >  _max_samples) _start_idx = _pts - _max_samples;
			if(_end_idx < _start_idx) _start_idx = _end_idx - 1;
	/* Compute MAR */
	    	_mar = _base_mar - _num_chans * (_pts - _start_idx) + _i;
			if(_mar < 0) _mar = _mar + _64K;
    			DevCamChk(_name, CamPiow(_name, 0,16,_mar, 24),1,1);
/*			DevCamChk(_name, CamQstopw(_name, 0, 2, _end_idx - _start_idx, _data=0, 16), 1, *); */
			DevCamChk(_name, CamFstopw(_name, 0, 2, _end_idx - _start_idx, _data=0, 16), 1, *);
			
			
/* Convert clock to double 
			_clock_converted = 1;
			_clock_conv = if_error(make_range(begin_of(_clock), end_of(_clock), data(ft_float(slope_of(_clock)))),
				_clock_converted = 0);
			if(!_clock_converted) 
*/			    _clock_conv = _clock;	

			
			

			_dim = make_dim(make_window(_start_idx, _end_idx - 1, _trig), _clock_conv);
			_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;

			_status = DevPutSignal1(_sig_nid, 0, 10/2048., word(_data), 0, _end_idx - _start_idx - 1, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');
				abort();

			}
		}
    }
    return(1);
}


