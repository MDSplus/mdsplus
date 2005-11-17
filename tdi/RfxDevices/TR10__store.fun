public fun TR10__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _N_COMMENT = 4;
    private _N_CLOCK_MODE = 5;
    private _N_TRIG_MODE = 6;
    private _N_TRIG_SOURCE = 7;
    private _N_CLOCK_SOURCE = 8;
    private _N_FREQUENCY = 9;
    private _N_USE_TIME = 10;
    private _N_PTS = 11;
    private _K_NODES_PER_CHANNEL = 6;
    private _N_CHANNEL_0= 12;
    private _N_CHAN_START_TIME = 1;
    private _N_CHAN_END_TIME = 2;
    private _N_CHAN_START_IDX = 3;
    private _N_CHAN_END_IDX = 4;
    private _N_CHAN_DATA = 5;
    private _N_INIT_ACTION = 48;
    private _N_STORE_ACTION = 49;
    private _2M = 2097152;
    private _INVALID = 10E20;


    _tr10_read_bug = 0;
    _tr10_bug_message = " for chan ";
    _tr10_status = 0;

	_max_samples = _2M;

write(*, 'TR10 STORE');

     _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 		abort();
    }

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
		    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	}

    _trig = data(DevNodeRef(_nid, _N_TRIG_SOURCE));
    _clock = evaluate(DevNodeRef(_nid, _N_CLOCK_SOURCE));
    _clock = execute('evaluate(`_clock)');
    _pts = data(DevNodeRef(_nid, _N_PTS));


	if(_remote)
	{
	write(*, 'TR10 START STORE');

		_handle = MdsValue('TR10HWStartStore(0, $, $)', _board_id, _pts);
		if(_handle == -1)
		{
			DevLogErr(_nid, 'TR10 device not in STOP state. Board ID: '//_board_id);
			abort();
		}
	}
	else
	{
		_handle = TR10HWStartStore(_nid, _board_id, _pts);
		if(_handle == -1)
		{
			DevLogErr(_nid, 'TR10 device not in STOP state. Board ID: '//_board_id);
			abort();
		}
	}
    
	
	for(_i = 0; _i < 16; _i++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL))))
        { 
			_end_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_END_IDX));	
			_start_idx = data(DevNodeRef(_nid, _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_START_IDX));	
	/*Check and correct memory overflow or wrong setting*/
			if(_end_idx > _pts) _end_idx = _pts;
			if(_end_idx  - _start_idx >  _max_samples) _start_idx = _pts - _max_samples;
			if(_end_idx < _start_idx) _start_idx = _end_idx - 1;

	/* Read data */
			if(_remote)
			{
				_data = MdsValue('TR10HWReadChan($, $, $, $, $)', _handle, (_i + 1), _start_idx, _end_idx, _pts );	

			}
			else
			{
			write(*, 'TR10 READ DATA');
				_data = TR10HWReadChan(_handle, _i + 1, _start_idx, _end_idx, _pts, _tr10_status);	
				
				/* TEST FOR TR10 READOUT BUG */
				_bug_idx = 0;
				while((sum(_data) == 0)&&(_bug_idx < 4))
				{
				    _bug_idx++;
				    _tr10_read_bug = 1;
				    wait(0.5);
				    _data = TR10HWReadChan(_handle, _i + 1, _start_idx, _end_idx, _pts, _tr10_status);	
				   
				}
				if(_bug_idx > 0)
				    _tr10_bug_message = _tr10_bug_message // _i //' ('//_tr10_status// '), ';
				/**********************/
			}						

	/* Build signal */
			_dim = make_dim(make_window(_start_idx, _end_idx - 1, _trig), _clock);

			_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;

			_status = DevPutSignal(_sig_nid, 0, 10/32768., word(_data), 0, _end_idx - _start_idx - 1, _dim);


			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');

			}
		}
        }
	if(_remote)
	{
		MdsValue('TR10HWClose($)', _handle);
		MdsDisconnect();				
	}
	else
		TR10HWClose(_handle);

	if(_tr10_read_bug)
	{
		DevLogErr(_nid, 'TR10 Readout error'//_tr10_bug_message);
		abort();
	
	}

        return(1);
}


