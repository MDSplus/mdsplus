public fun TR10__trigger(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;


    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);
    if(_board_id == _INVALID)
    {
    	write(*, "Invalid Board ID specification");
 		return (0);
    }

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	if(_remote)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), _INVALID);
		if(_ip_addr == _INVALID)
		{
    	    write(*, "Invalid Crate IP specification");
 		    return (0);
		}
	}

	if(_remote)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('TR10HWTrigger($1)', _board_id);
		MdsDisconnect();
	}
	else
	{
		_status = TR10HWTrigger(_board_id);
	}
	return(_status);
 }