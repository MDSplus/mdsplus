public fun CAENV1731__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 61;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_ADDRESS = 2;
    private _N_TRIG_MODE = 3;
    private _N_TRIG_SOFT = 4;
    private _N_TRIG_EXT = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_INPUT_OFS_NO_USE = 7;
    private _N_CLOCK_MODE = 8;
    private _N_CLOCK_SOURCE = 9;
    private _N_MONITOR_MODE = 10;
    private _N_MONITOR_LEV =11;
    private _N_NUM_SEGMENTS = 12;
    private _N_USE_TIME = 13;
    private _N_PTS = 14;
    private _N_START_IDX = 15;
    private _N_END_IDX = 16;
    private _N_START_TIME = 17;
    private _N_END_TIME = 18;
    private _N_BOARD_ID = 19;
    private _K_NODES_PER_CHANNEL = 7;
    private _N_CHANNEL_0= 20;
    private _N_CHAN_STATE = 1;
    private _N_CHAN_TRIG_STATE = 2;
    private _N_CHAN_TRIG_TRESH_LEV = 3;
    private _N_CHAN_TRIG_TRESH = 4;
    private _N_CHAN_OFFSET = 5;
    private _N_CHAN_DATA = 6;
  

    private _INVALID = 10E20;
     
    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 	abort();
    }
/* Initialize Library if the first time */
/* OLD     _handle = if_error(_handle, public _handle = CAENVME_Init(_board_id)); */
    if(_board_id == 0)
    {
		_handle_0 = if_error(_handle_0, public _handle_0 = CAENVME_Init(_board_id));
		_handle = _handle_0;
    } else if(_board_id ==  1)
    {
		_handle_1 = if_error(_handle_1, public _handle_1 = CAENVME_Init(_board_id));
		_handle = _handle_1;
    } else if(_board_id ==  2)
    {
		_handle_2 = if_error(_handle_2, public _handle_2 = CAENVME_Init(_board_id));
		_handle = _handle_2;
    } else if(_board_id ==  3)
    {
		_handle_3 = if_error(_handle_3, public _handle_3 = CAENVME_Init(_board_id));
		_handle = _handle_3;
    } else if(_board_id ==  4)
    {
		_handle_4 = if_error(_handle_4, public _handle_4 = CAENVME_Init(_board_id));
		_handle = _handle_4;
    } else 
    {
    	DevLogErr(_nid, "Board ID not supported: "// _board_id);
 	abort();
    }


   if(_handle == -1)
    {
    	DevLogErr(_nid, "Cannot Initialize VME Interface");
 	abort();
    }

    _vme_address=if_error(data(DevNodeRef(_nid, _N_VME_ADDRESS)), _INVALID);
    if(_vme_address == _INVALID)
    {
    	DevLogErr(_nid, "Invalid VME Address");
 	abort();
    }
    _clock = evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE));
    _clock = execute('evaluate(`_clock)');



    _dt = slope_of(_clock);

    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);
    if(_trig == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve trigger time");
 	abort();
    }

    _start_idx = if_error(data(DevNodeRef(_nid, _N_START_IDX)), _INVALID);
    if(_start_idx == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve start idx");
 	abort();
    }
    _end_idx = if_error(data(DevNodeRef(_nid, _N_END_IDX)), _INVALID);
    if(_end_idx == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve end idx");
 	abort();
    }

    _pts = if_error(data(DevNodeRef(_nid, _N_PTS)), _INVALID);
    if(_pts == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve PTS");
 	abort();
    }

/* Stop device */
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8100, 0L);
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error stopping device');
 	abort();
    }

/* Read number of buffers */
   
    _status = CAENVME_ReadCycle(_handle, _vme_address + 0x812C, _act_segments = 0L);
    if(_status != 0)    
    {
    	DevLogErr(_nid, 'Error getting the number of segments');
 	abort();
    }

/* Read Channel configuration */
    _status = CAENVME_ReadCycle(_handle, _vme_address + 0x8000, _chan_conf = 0L);
    if(_status != 0)    
    {
    	DevLogErr(_nid, 'Error getting channel configuration');
 	abort();
    }

    _is_ghz = ((_chan_conf & (1 << 12)) != 0);


write(*, 'Acquired events: ', _act_segments);

/* Get expected number of samples */
    _num_segments = if_error(data(DevNodeRef(_nid, _N_NUM_SEGMENTS)), _INVALID);
    if(_num_segments == _INVALID)
    {
    	DevLogErr(_nid, "Cannot get number of segments");
 	abort();
    }
    _segment_size =  4194304 / _num_segments;
    if(!_is_ghz)
	_segment_size = _segment_size / 2;


/* Get Active channels */
    _status = CAENVME_ReadCycle(_handle, _vme_address + 0x8120, _chan_mask = 0L);
    if(_status != 0)    
    {
    	DevLogErr(_nid, 'Error getting channel mask');
 	abort();
    }
    _n_act_chans = 0;
    for(_c = 0; _c < 8; _c++)
    {
	if((_chan_mask & (1 << _c)) != 0) 
	{
	    _n_act_chans++;	
	}
    }
    if(_n_act_chans == 0)
    {
    	DevLogErr(_nid, 'No active channels!');
 	return (1);
    }
    _exp_size = _segment_size * _n_act_chans + 16;

    _ch1_data = [];
    _ch2_data = [];
    _ch3_data = [];
    _ch4_data = [];
    _ch5_data = [];
    _ch6_data = [];
    _ch7_data = [];
    _ch8_data = [];

    _act_trigs = [];
    _delta = [];
/* read segments */
    for(_i = 0; _i < _act_segments; _i++)
    {
	_data = CAENVME_FIFOBLTReadCycle(_handle, _vme_address, long(_exp_size), _ret_len = 0L, _status = 0L);
    	if(_status != 0)    
    	{
    	    DevLogErr(_nid, 'Error Reading data for segment '// _i);
 	    abort();
    	}

	_event_size = (0x000000FF & long(_data[0])) | (0x0000FF00 & (long(_data[1]) << 8)) | (0x00FF0000 & (long(_data[2]) << 16)) | (0x0F000000 & (long(_data[3]) << 24));
	_event_size = _event_size * 4;

    	if(_exp_size != _event_size)
    	{
            DevLogErr(_nid, 'Internal Error: expected event size diferent from actual size: '// _exp_size // _event_size);
     	    abort();
     	}

    	_counter = (0x000000FF & long(_data[12])) | (0x0000FF00 & (long(_data[13]) << 8)) | (0x00FF0000 & (long(_data[14]) << 16)) | ((0x000000FF << 24) & (long(_data[15]) << 24));
    	_curr_trig = _counter * _dt * 4.;


    	_act_trigs = [_act_trigs, _curr_trig];
	_delta = [_delta, _dt];

    	_curr_start = 16 + _segment_size - _pts + _start_idx ;
    	_curr_end = 16 + _segment_size - _pts + _end_idx ;

    	if(_chan_mask & 1)
    	{
	    _ch1_data = [_ch1_data, _data[(_curr_start):(_curr_end)]];
    	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
    	}
    	if((_chan_mask & 2) != 0)
    	{
	    _ch2_data = [_ch2_data, _data[(_curr_start):(_curr_end)]];
    	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
  	}
    	if((_chan_mask & 4) != 0)
    	{
	    _ch3_data = [_ch3_data, _data[(_curr_start):(_curr_end)]];
     	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
   	}
    	if((_chan_mask & 8) != 0)
    	{
	    _ch4_data = [_ch4_data, _data[(_curr_start):(_curr_end)]];
    	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
    	}
    	if((_chan_mask & 16) != 0)
    	{
	    _ch5_data = [_ch5_data, _data[(_curr_start):(_curr_end)]];
     	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
   	}
    	if((_chan_mask & 32) != 0)
    	{
	    _ch6_data = [_ch6_data, _data[(_curr_start):(_curr_end)]];
     	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
   	}
    	if((_chan_mask & 64) != 0)
    	{
	    _ch7_data = [_ch7_data, _data[(_curr_start):(_curr_end)]];
     	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
   	}
    	if((_chan_mask & 128) != 0)
    	{
	    _ch8_data = [_ch8_data, _data[(_curr_start):(_curr_end)]];
     	    _curr_start += _segment_size;
    	    _curr_end += _segment_size;
    	}
   }

    _act_trigs = _act_trigs - _act_trigs[0] + _trig;

    _dim = make_dim(make_window(_start_idx, _end_idx * _act_segments, _trig), make_range(_act_trigs + _start_idx * _dt * 1D0, _act_trigs + _end_idx * _dt * 1D0, _delta * 1D0));

/*write(*, 'DIM: ', _dim); */

    if(_chan_mask & 1)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(0 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(0 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch1_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 2) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(1 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(1 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch2_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 4) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(2 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(2 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch3_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 8) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(3 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(3 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch4_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 16) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(4 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(4 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch5_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 32) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(5 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(5 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch6_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 64) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(6 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(6 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch7_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }
    if((_chan_mask & 128) != 0)
    {
	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(7 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(7 *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
	_status = DevPutSignal(_sig_nid, -128 + (_offset/0.5) * 128, 1D0/256., _ch8_data, 0, _act_segments * (_end_idx - _start_idx) - 1, _dim);
    }

    return (1);
}
