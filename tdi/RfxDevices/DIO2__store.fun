public fun DIO2__store(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _N_COMMENT = 4;
    private _N_CLOCK_SOURCE = 5;
    private _N_REC_START_EV = 6;

    private _K_NODES_PER_CHANNEL = 17;
    private _N_CHANNEL_0= 7;
    private _N_CHAN_FUNCTION = 1;
    private _N_CHAN_TRIG_MODE = 2;
    private _N_CHAN_EVENT = 3;
    private _N_CHAN_CYCLIC = 4;
    private _N_CHAN_DELAY = 5;
    private _N_CHAN_DURATION = 6;
    private _N_CHAN_FREQUENCY_1 = 7;
    private _N_CHAN_FREQUENCY_2 = 8;
    private _N_CHAN_INIT_LEVEL_1 = 9;
    private _N_CHAN_INIT_LEVEL_2 = 10;
    private _N_CHAN_DUTY_CYCLE = 11;
    private _N_CHAN_TRIGGER = 12;
    private _N_CHAN_CLOCK = 13;
    private _N_CHAN_TRIGGER_1 = 14;
    private _N_CHAN_TRIGGER_2 = 15;

    private _N_REC_EVENTS = 143;
    private _N_REC_TIMES = 144;
    private _N_SYNCH = 145;
    private _N_SYNCH_EVENT = 146;


	private _LARGE_TIME = 1E6;
    private _INVALID = 10E20;

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
	}



	if(_remote != 0)
	{
	    _cmd = 'MdsConnect("'//_ip_addr//'")';
	    execute(_cmd);
	    _rec_events = MdsValue('DIO2HWGetRecEvents(0, $1)');
	    _rec_times =  MdsValue('_DIO2_rec_times');
	    MdsDisconnect();
	}
	else
	{
	    _rec_events = DIO2HWGetRecEvents(_nid, _board_id);
	    _rec_times = _DIO2_rec_times;
	}

  	_status = DevPut(_nid, _N_REC_EVENTS, _rec_events);
 	DevPut(_nid, _N_REC_TIMES, _rec_times);
    for(_c = 0; _c < 8; _c++)
    {
        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_c *  _K_NODES_PER_CHANNEL))))
        { 
			DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_FUNCTION,
				 ['CLOCK', 'PULSE', 'GCLOCK', 'DCLOCK'], [0,1,2,3], _function = 0);
			if(_function != 0) /* If not clock */
			{
				if(_remote != 0)
				{
					_cmd = 'MdsConnect("'//_ip_addr//'")';
					execute(_cmd);
					_phases_count = MdsValue('DIO2HWGetPhaseCount(0, $1, $2)', _board_id, _c);
					MdsDisconnect();
				}
				else
					_phases_count = DIO2HWGetPhaseCount(_nid, _board_id, _c);
				
 
 				_trig_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER), 'FULLPATH');
				_trig1_expr = _trig_path // ' + ' // _phases_count[0];
				_trig2_expr = _trig_path // ' + ' // _phases_count[1];
  	    			DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_1, compile(_trig1_expr));
 	    			DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_2, compile(_trig2_expr));
			}
		}
	}

	return(1);
}
			
						
				
					
				


