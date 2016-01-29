public fun DIO2__trigger(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
    private _K_NODES_PER_CHANNEL = 17;
    private _N_CHANNEL_0= 7;

    private _N_CHAN_FUNCTION = 1;
    private _N_CHAN_TRIG_MODE = 2;

    private _INVALID = 10E20;

    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	write(*, "Invalid Board ID specification");
 		return (0);
    }

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	if(_remote )
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    write(*, "Invalid Crate IP specification");
 		    return (0);
		}
	}

	_channel_mask = 0;

    for(_c = 0; _c < 8; _c++)
    {


write(*, _c);

        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_c *  _K_NODES_PER_CHANNEL))))
        { 
		DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_FUNCTION,
				['PULSE', 'CLOCK', 'GCLOCK', 'DCLOCK'], [0,1,2,3], _clock_mode = 1);
		if(_clock_mode != 1)
		{
			DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_MODE,
				['EVENT', 'RISING EDGE', 'FALLING EDGE', 'SOFTWARE'], [0,1,2,3], _trig_mode = 1);

			if(_trig_mode == 3) 
			{
				write(*, 'Trigger on ', _c);
				_channel_mask = _channel_mask | (1 << _c);
			}
		}
	}
    }




	if(_remote)
	{
               _sock = MdsConnect(_ip_addr);
               if (_sock == -1) {
                   DevLogErr(_nid, "Error connecting to board "//_ip_addr);
                   abort();
               }
	       _status = MdsValue('DIO2HWTrigger(0, $1, $2)', _board_id, _channel_mask);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error software trigger generation in DIO2 device: see CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = DIO2HWTrigger(_nid, _board_id, _channel_mask);
		if(_status == 0)
			abort();
	}
	return(_status);
 }
