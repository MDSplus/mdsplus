public fun DIO2Encoder__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
	private _N_COMMENT = 4;
    private _N_CLOCK_SOURCE = 5;

    private _K_NODES_PER_CHANNEL = 5;
    private _N_CHANNEL_0= 6;
    private _N_CHAN_EVENT_NAME = 1;
    private _N_CHAN_EVENT = 2;
    private _N_CHAN_EVENT_TIME = 3;
    private _N_CHAN_TERMINATION = 4;

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

    DevNodeCvt(_nid, _N_CLOCK_SOURCE, ['INTERNAL', 'HIGHWAY'], [0,1], _ext_clock = 0);
 



	_events = [];
	_terminations = [];
    for(_c = 0; _c < 17; _c++)
    {

        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_c *  _K_NODES_PER_CHANNEL))))
        { 

			_ev_name =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_EVENT_NAME)),'');
			if(_ev_name != '')
				_event = TimingDecodeEvent(_ev_name);
			else
				_event = 0;

			if(_event != 0)
				DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_EVENT, _event);
			else
				_event =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_EVENT)),0);
			if(_event == 0) 
    				DevLogErr(_nid, "Invalid Event specification for channel" // (_c+1));
			else
			{
				_ev_time =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_EVENT)),_INVALID);
				if(_ev_time == _INVALID)
				{
    				DevLogErr(_nid, "Invalid Event time specification for channel" // (_c+1));
					_event = 0;
				}
				else
				{
					_status = TimingRegisterEventTime(_ev_name, getnci(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_EVENT_TIME), 'fullpath'));
				}
				if(_status == -1)
				{
					DevLogErr(_nid, "Internal error in TimingRegisterEventTimes: different array sizes");
					abort();
				}		
		    }
		if(_c < 16)	
			DevNodeCvt(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TERMINATION, ['OFF', 'ON'], [0,1], _termination = 0);
		else
		    _termination = 0;
		}
		else
		{
			_event = 0;
			_termination = 0;
		}
	 	if(_c < 16)
		{ 
		    _events = [_events, _event];
			_terminations = [_terminations, _termination];
		}
	}



/* Setup HW configuration */
	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('DIO2EncoderHWInit(0, $1, $2, $3, $4)', _board_id, _ext_clock, _events, _terminations);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error initializing DIO2Encoder device: see CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = DIO2EncoderHWInit(_nid, _board_id, _ext_clock, _events, _terminations);
		if(_status == 0)
			abort();
	}

	return (1);
}
