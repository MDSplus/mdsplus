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
    private _N_SLAB		= 9;

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

    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _error = 1);
    if(_error)
    {
		DevLogErr(_nid, "Missing trigger source");
		abort();
    }

	 _sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 2000, _swap=0);
	if( _sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	if((_err_msg = TCPSendCommand( _sock, "ND_DUMP") ) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, "Error during send  ND_DUMP command"); 
		abort();
	}


/* STORE OSC SIGNAL */


	if((_err_msg = TCPSendCommand( _sock, "ND_GET_OSC")) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, "Error during send  ND_GET_OSC command"); 
		abort();
	}


	_data = TCPReadFloat( _sock, 1);

	write(*,  size(_data) - 1);


	if( size( _data ) < 4)
	{
		DevLogErr(_nid, "No data read for OSC signal"); 
	} else {
		_dim = make_dim(make_window(0, size(_data) - 1, _trig), *:*:1.e-4);
		_sig_nid =  DevHead(_nid) +_N_OSC;
		_status = DevPutSignal(_sig_nid, 0, 1., _data[1..*], 0, size(_data) - 1, _dim);
		if(! _status )
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}
	}


/* STORE AMP SIGNAL */

	if((_err_msg = TCPSendCommand( _sock, "ND_GET_AMP")) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, "Error during send  ND_GET_AMP command"); 
		abort();
	}


	_data = TCPReadFloat( _sock, 1);

	if( size( _data ) < 2)
	{
		DevLogErr(_nid, "No data read for AMP signal"); 
	} else {

		_dim = make_dim(make_window(0, size(_data) - 1, _trig), *:*:1.e-4);
		_sig_nid =  DevHead(_nid) +_N_AMP;
		_status = DevPutSignal(_sig_nid, 0, 1., _data[1..*], 0, size(_data) - 1, _dim);
		if(! _status )
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}	}


/* STORE TOTAL SIGNAL */

	if( (_err_msg = TCPSendCommand( _sock, "ND_GET_SLAB") ) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, "Error during send  ND_GET_SLAB command "); 
		abort();
	}

	_data = TCPReadFloat( _sock, 1);

	if( size( _data ) < 4)
	{
		DevLogErr(_nid, "No data read for SLAB signal"); 
	} else {
		_dim = make_dim(make_window(0, size(_data) - 1, _trig), *:*:1.e-4);
		_sig_nid =  DevHead(_nid) +_N_SLAB;
		_status = DevPutSignal(_sig_nid, 0, 1., _data[1..*], 0, size(_data) - 1, _dim);
		if(! _status )
		{
			DevLogErr(_nid, 'Error writing data in pulse file');
		}
	}

	TCPCloseConnection( _sock);

	return (1);

}















