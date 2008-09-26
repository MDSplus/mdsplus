public fun TSEdgeCCD__store(as_is _nid, optional _method)
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



write(*, "TSEdgeCCD__store");

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
			_cmd = 'TSEdgeCCDReadData('//_interface_id//', 50, 1, 300, 578, 1, 1)';
			_data = MdsValue(_cmd);
			if( size(_data) == 1 )
			{
			/*
				_msg = MdsValue('TSEdgeCCDError()');
				DevLogErr(_nid, "Error in readData operation : "//_msg);
			*/
				DevLogErr(_nid, "Error in readData operation : ");
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
		_data = zero(300 / 1 * 578 / 1, 0W);

		_status = TSEdgeCCD->TSEdgeCCD_ReadData(val(_interface_id), val(50), val(1), val(300), val(578), val(1), val(1), ref(_data));
		if(_status == 0)
		{
		/*
		    _msg = TSEdgeCCDError();
			DevLogErr(_nid, "Error in readData operation : "//_msg);
		*/
			DevLogErr(_nid, "Error in readData operation : ");
			abort();
		}
	}
/*
	write(*, _data[0..10]);
*/

    _remove_line = 0;

    _data_sig = _data[ 300 * _remove_line : 300 * 288 ];


	_data_sig = set_range(300, 288 - _remove_line , _data_sig);


	write(*, "Data sig ", size(_data_sig) );

	_sig = compile('build_signal( `_data_sig,, [0 : 299], [0 : 287 - `_remove_line])');

	_sig_nid =  DevHead(_nid) + _N_DATA;
	_status = TreeShr->TreePutRecord(val(_sig_nid), xd(_sig), val(0));
	if(! (_status & 1))
	{
		DevLogErr(_nid, 'Error writing data in pulse file');
		abort();
	}


    _data_back = _data[300 * 289 : 300 * 578 ];

	_data_back = set_range(300, 288, _data_back);

	write(*, "Data back ", size(_data_back) );

	_back = compile('build_signal(`_data_back,, [0..299], [0..287])');

	_back_nid =  DevHead(_nid) + _N_BACKGROUND;
	_status = TreeShr->TreePutRecord(val(_back_nid), xd(_back), val(0));
	if(! (_status & 1) )
	{
		DevLogErr(_nid, 'Error writing data in pulse file');
		abort();
	}

 
    return(1);

}
