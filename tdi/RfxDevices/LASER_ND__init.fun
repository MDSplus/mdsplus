public fun LASER_ND__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

    private _N_HEAD		= 0;
    private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _N_PORT		= 3;
    private _TRIG_SOURCE	= 4;
    private _N_NUM_PULSES	= 5;
    private _N_DELAY_PULSE	= 6;
    private _N_OSC		= 7;
    private _N_AMP		= 8;
    private _N_TOTAL		= 9;

    private _ASCII_MODE = 0;

write(*, "LASER Neodimium");

wait(1);


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

	 _sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 2000, _swap=0);
	if( _sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	write(*, "OK" );

	if((_err_msg = TCPSendCommand( _sock, "ND_DUMP")) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, _err_msg); 
		abort();
	}

	wait(5);

	if((_err_msg = TCPSendCommand( _sock, "ND_INIT")) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, _err_msg); 
		abort();
	}

	wait(5);

	if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE "//trim(adjustl(_n_pulses))//" "//trim(adjustl(_delay_pulse))) ) != "")
	{
		TCPCloseConnection( _sock);
		DevLogErr(_nid, _err_msg); 
		abort();
	}


	TCPCloseConnection( _sock);

	return (1);

}















