public fun LASER_ND__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

    private _N_HEAD		= 0;
    private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _N_PORT		= 3;
    private _N_TRIG_SOURCE	= 4;
    private _N_NUM_PULSES	= 5;
    private _N_DELAY_PULSE	= 6;
    private _N_OSC		= 7;
    private _N_AMP		= 8;
    private _N_TOTAL		= 9;

    private _ASCII_MODE = 0;

write(*, "LASER Neodimium");

   _error = 0;

    _ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing IP address"); 
	abort();
    }

write(*, _ip);


    _port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing TCP Port");
	abort();
    }

write(*, _port);

    _n_pulses = if_error(data(DevNodeRef(_nid, _N_NUM_PULSES)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing number of pulses");
	abort();
    }

write(*, _n_pulses);

    _delay_pulse = if_error(data(DevNodeRef(_nid, _N_DELAY_PULSE)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing delay between pulse");
	abort();
    }

write(*, _delay_pulse );

    _trig = if_error(data(DevNodeRef(_nid, _TRIG_SOURCE)),(DevLogErr(_nid, "Missing trigger source"); abort();));
    if(_error)
    {
	DevLogErr(_nid, "Missing trigger source");
	abort();
    }

   if( ( allocated (public _laser_nd_connected) ) == 0)
   {

	public _laser_nd_connected = 0;
   }

    if( ( public _laser_nd_connected ) == 0 )
    {

	public _sock = TCPOpenConnection(_ip, _port, _ASCII_MODE);
	if(public _sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}
        public _laser_nd_connected = 1;
    }    


	if(TCPSendCommand(public _sock, "ND_DUMP") == 0)
	{
		DevLogErr(_nid, "Error during send  ND_DUMP command"); 
		abort();
	}


/* STORE OSC SIGNAL */

	if(TCPSendCommand(public _sock, "ND_GET_OSC") == 0)
	{
		DevLogErr(_nid, "Error during send  ND_GET_OSC command"); 
		abort();
	}


	_data = TCPReadFloat(public _sock);

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

	if(TCPSendCommand(public _sock, "ND_GET_AMP") == 0)
	{
		DevLogErr(_nid, "Error during send  ND_GET_AMP command"); 
		abort();
	}


	_data = TCPReadFloat(public _sock);

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

	if(TCPSendCommand(public _sock, "ND_GET_TOTAL") == 0)
	{
		DevLogErr(_nid, "Error during send  ND_GET_TOTAL command"); 
		abort();
	}

	_data = TCPReadFloat(public _sock);

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

/*
	TCPCloseConnection(public _sock);
*/
	return (1);

}















