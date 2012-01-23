public fun CAENV1731__init(as_is _nid, optional _method)
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

    write(*, 'CAENV1731 INIT');


     
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

/* Software Reset */
    _status = CAENVME_WriteCycle(_handle, long(_vme_address + 0x0EF24), long(0));
    if(_status)
    {
    	DevLogErr(_nid, "Error resetting CAEN device");
 	abort();
    }
 

/* Num Segments */
    DevNodeCvt(_nid, _N_NUM_SEGMENTS, [1,2,4,8,16,32,64,128,256,512, 1024], [0,1,2,3,4,5,6,7,8,9,10], _num_segments=0);
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x800C, long(_num_segments));
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Number of Segments');
 	abort();
    }
    _segment_size = 4194304 / (2 ** _num_segments);


/* Channel Global Configuration */

    DevNodeCvt(_nid, _N_TRIG_MODE, ['OVER THRESHOLD', 'UNDER THRESHOLD'], [0,1], _trig_mode = 0);
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['500 MHz', '1 GHz', 'EXTERNAL'], [0,1, 0], _clock_mode = 0);

    _chan_conf = (_trig_mode << 6) | (_clock_mode << 12) | 0x00000010;

/*    _chan_conf = (_trig_mode << 6) | (_clock_mode << 12) | 0x00000018;
*/    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8000, _chan_conf);
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Channel Configuration');
 	abort();
    }


/* Channel Configuration */
    _chan_enable = 0L;
    _chan_trig_enable = 0L;

    for(_c = 0; _c < 8; _c++)
    {
	_threshold_lev = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +( _c * _K_NODES_PER_CHANNEL) + _N_CHAN_TRIG_TRESH_LEV)),_INVALID);
    	if(_threshold_lev  == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error getting Threshold Level for channel ' // _c);
 	    abort();
    	}
        _status = CAENVME_WriteCycle(_handle, _vme_address + 0x1080 + _c * 0x100, long(_threshold_lev));
    	if(_status != 0)
    	{
    	    DevLogErr(_nid, 'Error setting Channel Configuration for channel '//_c);
 	    abort();
    	}

	_threshold_n = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_TRESH)),_INVALID);
    	if(_threshold_n == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting Threshold samples for channel ' // _c);
 	    abort();
    	}
        _status = CAENVME_WriteCycle(_handle, _vme_address + 0x1084 + _c * 0x100, long(_threshold_n));
    	if(_status != 0)
    	{
    	    DevLogErr(_nid, 'Error setting Channel Configuration for channel ' // _c);
 	    abort();
    	}

	_offset = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_OFFSET)),_INVALID);
    	if(_offset == _INVALID)
    	{
    	    DevLogErr(_nid, 'Error Getting offset for channel ' // _c);
 	    abort();
    	}
	if(_offset > 0.5) _offset = 0.5;
	if(_offset < -0.5) _offset = -0.5;
	_offset = (_offset / 0.5) * 32767;

        _status = CAENVME_WriteCycle(_handle, _vme_address + 0x1098 + _c * 0x100, long(_offset + 0x08000));
    	if(_status != 0)
    	{
    	    DevLogErr(_nid, 'Error setting Channel offset for channel ' // _c);
 	    abort();
    	}
	DevNodeCvt(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_STATE, ['ENABLED', 'DISABLED'], [1,0], _enabled = 0);
	_chan_enable = _chan_enable | (_enabled << _c);
	DevNodeCvt(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_STATE, ['ENABLED', 'DISABLED'], [1,0], _enabled = 0);
	_chan_trig_enable = _chan_trig_enable | (_enabled << _c);
    }
	
    DevNodeCvt(_nid, _N_TRIG_SOFT, ['ENABLED', 'DISABLED'], [1,0], _trig_soft = 0);
    _chan_trig_enable = _chan_trig_enable | (_trig_soft << 31);
    DevNodeCvt(_nid, _N_TRIG_EXT, ['ENABLED', 'DISABLED'], [1,0], _trig_ext = 0);
    _chan_trig_enable = _chan_trig_enable | (_trig_ext << 30);


    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8120, long(_chan_enable));
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Channel state');
 	abort();
    }

    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x810C, long(_chan_trig_enable));
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Channel trigger state');
 	abort();
    }

/* Monitor */
    DevNodeCvt(_nid, _N_MONITOR_MODE, ['MAJORITY', 'SAWTOOTH', 'BUF. OCCUPANCY', 'SELECTED LEVEL'], [0,1,3,4], _monitor_mode = 0);
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8144, long(_monitor_mode));
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Monitor Mode');
 	abort();
    }
    if(_monitor_mode == 4)
    {
    	_monitor_lev = if_error(data(DevNodeRef(_nid, _N_MONITOR_LEV)), _INVALID);
    	if(_monitor_lev == _INVALID)
    	{
    	    DevLogErr(_nid, "Invalid Monitor level");
 	    abort();
    	}
  	_monitor_lev = long(_monitor_lev/0.244);
    	_status = CAENVME_WriteCycle(_handle, _vme_address + 0x8138,_monitor_lev);
    	if(_status != 0)
    	{
    	    DevLogErr(_nid, 'Error setting Monitor Level');
 	    abort();
    	}
    }

/* Front Panel trigger out setting set TRIG/CLK to TTL*/
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x811C,1L);
    if(_status != 0)
    {
        DevLogErr(_nid, 'Error setting trigger out');
     	abort();
    }


/* Trigger source */
    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE))  , _INVALID);
    if(_trig == _INVALID)
    {
	DevLogErr(_nid, "Cannot resolve trigger"); 
	abort();
    }
/* Clock Source */
    DevNodeCvt(_nid, _N_CLOCK_MODE , ['500 MHz', '1 GHz', 'EXTERNAL'], [500E6,1E9,0], _clock_freq = 0);
    if(_clock_freq == 0)
    {
        _clk = DevNodeRef(_nid, _N_CLOCK_SOURCE); 
	_clock_val = if_error(execute('`_clk'), _INVALID);
	if(_clock_val == _INVALID)
    	{
	    DevLogErr(_nid, "Cannot resolve clock"); 
	    abort();
    	}
    }
    else
    {
	_clock_val = make_range(*,*,1./ _clock_freq);
    	 DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
    }

/* PTS */
    _pts = if_error(data(DevNodeRef(_nid, _N_PTS))  , _INVALID);
    if(_pts == _INVALID)
    {
	DevLogErr(_nid, "Invalid PTS"); 
	abort();
    }
    if(_pts > _segment_size)
    {
	DevLogErr(_nid, "PTS larger than segment size: " // _segment_size); 
	abort();
    }
    if(_clock_mode == 0)
      _pts = _pts / 8;
    else
      _pts = _pts / 16;
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8114, long(_pts));
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error setting Number of Segments');
 	abort();
    }
  

/* Time management */
    DevNodeCvt(_nid, _N_USE_TIME, ['YES', 'NO'], [1,0], _use_time=0);
    if(_use_time)
    {
    	_start_time = if_error(data(DevNodeRef(_nid, _N_START_TIME))  , _INVALID);
    	if(_start_time == _INVALID)
    	{
	    DevLogErr(_nid, "Invalid Start time"); 
	    abort();
    	}
    	_end_time = if_error(data(DevNodeRef(_nid, _N_END_TIME))  , _INVALID);
    	if(_end_time == _INVALID)
    	{
	    DevLogErr(_nid, "Invalid End time"); 
	    abort();
    	}

	if(_end_time > 0)
	    _end_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _end_time + _trig);
	else
	    _end_idx = - x_to_i(build_dim(build_window(0,*,_trig + _end_time), _clock_val),  _trig);

        DevPut(_nid, _N_END_IDX, long(_end_idx));
		
	if(_start_time > 0)
	    _start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _start_time + _trig);
	else
	    _start_idx =  - x_to_i(build_dim(build_window(0,*,_trig + _start_time ), _clock_val),_trig);

        DevPut(_nid, _N_START_IDX, long(_start_idx));

   }

/* Run device */
    _status = CAENVME_WriteCycle(_handle, _vme_address + 0x8100, 4L);
    if(_status != 0)
    {
    	DevLogErr(_nid, 'Error Running CAEN device');
 	abort();
    }
    return (1);
}


