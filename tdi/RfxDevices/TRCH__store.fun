public fun TRCH__store(as_is _nid, optional _method)
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
    private _1M = 1048576;
    private _500K = 524288;

    _name = DevNodeRef(_nid, _N_NAME);

    DevCamChk(_name, CamPiow(_name, 2,0, _control=0, 16),1,1);
    _not_stopped = ((_control >> 13) &3);
    if (_not_stopped != 0)
    {
        DevLogErr(_nid, 'Module is not in STOP state');
        abort();
    }
    _num_chans = data(DevNodeRef(_nid, _N_CHANNELS));
    if(_num_chans < 6)  _num_chans = 3;
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

 /* 
    _base_mar = _base_mar & 0xffffff;
 */

/* TACON */
    _ticks = long((_trig + 5. - 200E-6)/slope_of(_clock) + 0.5);
/*  _ticks = long((_trig + 5.D0)/slope_of(_clock) + 0.5);*/
    _ticks = 2*(_ticks + _pts);
    _ticks = mod(_ticks, _1M);



    write(*, 'MAR STIMATO: ', _ticks);
    write(*, 'MAR LETTO: ', _base_mar);

/*
    _base_mar = _ticks;
*/


    if( _base_mar >  _1M )
    {
/*
	if( _ticks < 65535 )
	   _base_mar = ( _base_mar & 0xFFFF );
	else
*/
	   _base_mar = ( _ticks & 0xFF0000 ) + ( _base_mar & 0xFFFF );

      	write(*, 'MAR CORRETTO: ', _base_mar);
    }	

/* FINE TACON */

    write(*, "PTS = ", _pts);


    for(_i = 0; _i < _num_chans; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
			_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
			_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
	/*Check and correct memory overflow or wrong setting*/
			if(_end_idx > _pts) _end_idx = _pts;
			if(_num_chans == 6)
				_max_samples = _500K;
			else
				_max_samples = _1M;
			if(_end_idx  - _start_idx >  _max_samples) _start_idx = _pts - _max_samples;
			if(_end_idx < _start_idx) _start_idx = _end_idx - 1;
	/* Compute MAR */
			if(_num_chans == 6)
				_mar = _base_mar - 2*(_pts - _start_idx);
			else
				_mar = _base_mar - (_pts - _start_idx);
			if(_mar < 0) _mar = _mar + _1M;
			if(_num_chans == 3)
				_mar = _mar |(_i   << 20);
			else
				_mar = (_mar + mod(_i, 2))|((_i /2)<<20);

    			DevCamChk(_name, CamPiow(_name, 0,16,_mar, 24),1,1);
			

	/*		DevCamChk(_name, CamFStopw(_name, 0, 2, _end_idx - _start_idx, _data=0, 16), 1, *); */
			DevCamChk(_name, CamQstopw(_name, 0, 2, _end_idx - _start_idx, _data=0, 16), 1, *);
			
/* Convert clock to double */
/*			_clock_converted = 1;
			_clock_conv = if_error(make_range(begin_of(_clock), end_of(_clock), data(ft_float(slope_of(_clock)))),
				_clock_converted = 0);
			if(!_clock_converted) */
			    _clock_conv = _clock;	

			
			
			_dim = make_dim(make_window(_start_idx, _end_idx - 1, d_float(_trig)), _clock_conv);
			_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
			_status = DevPutSignal1(_sig_nid, 0, 10/2048., word(_data), 0, _end_idx - _start_idx - 1, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');
        			abort();
            }
		}
    }
/*    DevCamChk(_name, CamPiow(_name, 0,16,_base_mar, 24),1,1);*/
    return(1);
}
