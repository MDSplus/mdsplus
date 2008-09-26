public fun TSEdgeCCD__trigger(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     10;
    private _N_HEAD =		  0;
    private _N_COMMENT =	  1;
    private _N_INTERFACE =      2;
    private _N_SW_MODE =        3;
    private _N_IP_ADDR =        4;
    private _N_TRIG_ARM =       5;
    private _N_TRIG_LASER =     6;
    private _N_DATA       =     7;
    private _N_BACKGROUND =     8;



write(*, "TSEdgeCCD__trigger");

	_error = 0;

    _interface_id = if_error(data(DevNodeRef(_nid, _N_INTERFACE)), -1);
    if(_interface_id < 0 )
    {
    	DevLogErr(_nid, "Invalid interface ID specification");
 	abort();
    }

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

    _ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
    if(_ip_addr == "")
    {
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 	    abort();
    }

    _error = 0;
    _trig_arm = if_error(data(DevNodeRef(_nid, _N_TRIG_ARM)), _error = 1);
    if(_error )
    {
    	  DevLogErr(_nid, "Cannot resolve arm trigger ");
 		  abort();
    }

    _error = 0;
    _trig_arm = if_error(data(DevNodeRef(_nid, _N_TRIG_LASER)), _error = 1);
    if(_error )
    {
    	  DevLogErr(_nid, "Cannot resolve laser trigger ");
 	  abort();
    }

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{
			_cmd = 'TSEdgeCCD->TSEdgeCCD_trigger(val('//_interface_id//'))';
			_status = MdsValue(_cmd);
			if(_status == 0)
			{
			/*
				_msg = MdsValue('TSEdgeCCDError()');
				DevLogErr(_nid, "Error in trigger operation :  "//_msg);
			*/
				DevLogErr(_nid, "Error in trigger operation :  ");
				MdsDisconnect();
				abort();
			}
			MdsDisconnect();
		}
		else
		{
			DevLogErr(_nid, "Cannot connect to mdsip server "//_ip_addr);
			abort();
		}

	}
	else
	{
		_status = TSEdgeCCD->TSEdgeCCD_trigger(val(_interface_id));
		if(_status == 0)
		{
		/*
			_msg = TSEdgeCCDError();
			DevLogErr(_nid, "Error in trigger operation :  "//_msg);
		*/
			DevLogErr(_nid, "Error in trigger operation :  ");
			abort();
		}
	}


    return(1);

}
