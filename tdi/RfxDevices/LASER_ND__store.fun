public fun LASER_ND__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

	private _N_HEAD			= 0;
	private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _TRIG_SOURCE	= 3;
    private _N_PORT			= 4;
    private _N_NUM_PULSES	= 5;
    private _N_DELAY_PULSE	= 6;
    private _N_OSC			= 7;
    private _N_AMP			= 8;
    private _N_TOTAL		= 9;

	private _ASCII_MODE = 0

    _ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)),(DevLogErr(_nid, "Missing IP address"); abort();));

    _port = if_error(data(DevNodeRef(_nid, _N_PORT)),(DevLogErr(_nid, "Missing TCP Port"); abort();));

    _n_pulses = if_error(data(DevNodeRef(_nid, _N_NUM_PULSES)),(DevLogErr(_nid, "Missing number of pulses"); abort();));

    _delay_pulse = if_error(data(DevNodeRef(_nid, _N_NUM_PULSES)),(DevLogErr(_nid, "Missing delay between pulse"); abort();));

    _trig = if_error(data(DevNodeRef(_nid, _TRIG_SOURCE)),(DevLogErr(_nid, "Missing trigger source"); abort();));

	_sock = TCPOpenConnection(_ip, _port, _ASCII_MODE);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	if(TCPSendCommand(_sock, "ND_DUMP") == 0)
	{
		TCPCloseConnection(_sock);
		DevLogErr(_nid, "Error during send  ND_DUMP command"); 
		abort();
	}

	if((_msg = CheckAnswer(_sock) )!= "")
	{
		TCPCloseConnection(_sock);
		DevLogErr(_nid, "Error in ND_DUMP command execution : "//_msg); 
		abort();
	}

/* STORE OSC SIGNAL */

	if(TCPSendCommand(_sock, "ND_GET_OSC") == 0)
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error during send  ND_GET_OSC command"); 
		abort();
	}

	if((_msg = TCPCheckAnswer(_sock) )!= "")
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error in ND_GET_OSC command execution : "//_msg); 
		abort();
	}

	_data = TCPReadFloat(_sock);

	if( size( _data ) < 4)
	{
		DevLogErr(_nid, "No data read for OSC signal"); 
	} else {
		_dim = make_dim(make_window(0, size(_data) - 1, _trig), [* : *: _data[1]]);
		_osc_sig = build_signal(_data[1,*],, _dim );
		if(! DevPut(_nid, _N_OSC, _osc_sig))
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}
	}

/* STORE AMP SIGNAL */

	if(TCPSendCommand(_sock, "ND_GET_AMP") == 0)
	{
		TCPCloseConnection(_sock);
		DevLogErr(_nid, "Error during send  ND_GET_AMP command"); 
		abort();
	}

	if((_msg = TCPCheckAnswer(_sock) )!= "")
	{
		TCPCloseConnection(_sock);
		DevLogErr(_nid, "Error in ND_GET_AMP command execution : "//_msg); 
		abort();
	}

	_data = TCPReadFloat(_sock);

	if( size( _data ) < 2)
	{
		DevLogErr(_nid, "No data read for AMP signal"); 
	} else {

		_dim = make_dim(make_window(0, size(_data) - 1, _trig), [* : *: _data[1]]);
		_amp_sig = build_signal(_data[1,*],, _dim );
		if(! DevPut(_nid, _N_AMP, _amp_sig))
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}
	}


/* STORE TOTAL SIGNAL */

	if(TCPSendCommand(_sock, "ND_GET_TOTAL") == 0)
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error during send  ND_GET_TOTAL command"); 
		abort();
	}

	if((_msg = TCPCheckAnswer(_sock) )!= "")
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error in ND_GET_TOTAL command execution : "//_msg); 
		abort();
	}

	_data = TCPReadFloat(_sock);

	if( size( _data ) < 4)
	{
		DevLogErr(_nid, "No data read for TOTAL signal"); 
	} else {
		_dim = make_dim(make_window(0, size(_data) - 1, _trig), [* : *: _data[1]]);
		_total_sig = build_signal(_data[1,*],, _dim );
		if(! DevPut(_nid, _N_TOTAL, _total_sig))
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}
	}

	TCPCloseConnection(_sock);

	return (1);

}















