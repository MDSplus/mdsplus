public fun LASER_NDRT__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 21;

    private _N_HEAD		= 0;
    private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _N_PORT		= 3;
    private _N_RT_ADDRESS	= 4;
    private _N_TRIG_MODE	= 5;
    private _N_TRIG_SOURCE	= 6;
    private _N_RT_TRIG_MODE	= 7;
    private _N_NUM_PULSES	= 8;
    private _N_DELAY_PULSE	= 9;
    private _N_MAX_N7_AMP	= 10;
    private _N_MIN_N7_AMP	= 11;
    private _N_MAX_N7_PH	= 12;
    private _N_MIN_N7_PH	= 13;
    private _N_MAX_RATIO	= 14;
    private _N_MIN_RATIO	= 15;
    private _N_OSC		= 16;
    private _N_AMP		= 17;
    private _N_TOTAL		= 18;

    private _K_EXT_DT		= 0;
    private _K_EXT_10_DT	= 1;
    private _K_EXT_RT		= 2;

    private _K_N7_AMP		= 0;
    private _K_N7_AMP_PH	= 1;
    private _K_RATIO_DOM_SEC	= 2;
    private _K_RATIO_DOM_SEC_PH	= 3;


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

    _ip_rt = if_error(data(DevNodeRef(_nid, _N_RT_ADDRESS)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing IP address"); 
	abort();
    }
write(*, _ip_rt);


    DevNodeCvt(_nid, _N_TRIG_MODE, ['EXT_DT', 'EXT_10_DT', "EXT_RT"], [ _K_EXT_DT, _K_EXT_10_DT, _K_EXT_RT], _trg_mode = 0);

    if( _trg_mode == _K_EXT_DT )
    {
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


    } 
    else if( _trg_mode == _K_EXT_10_DT )
    {
 
    	_delay_pulses = if_error(data(DevNodeRef(_nid, _N_DELAY_PULSE)), _error = 1);
    	if( _error || size( _delay_pulses ) < 10 )
    	{
	    DevLogErr(_nid, "Must be define 10 delay between pulse");
	    abort();
    	}
	write(*, _delay_pulse );

    } 
    else if( _trg_mode == _K_EXT_RT ) 
    {
    	DevNodeCvt(_nid, _N_RT_TRIG_MODE, ['N7_AMP', 'N7_AMP_PH', "RATIO_DOM_SEC", "RATIO_DOM_SEC_PH"], [_K_N7_AMP, _K_N7_AMP_PH, _K_RATIO_DOM_SEC, _K_RATIO_DOM_SEC_PH], _trg_mode_rt = 0);
	
    	_n7_max_amp = if_error(data(DevNodeRef(_nid, _N_MAX_N7_AMP)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing n=7 max amplitude trigger level");
	    abort();
    	}

    	_n7_min_amp = if_error(data(DevNodeRef(_nid, _N_MIN_N7_AMP)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing n=7 min amplitude trigger level");
	    abort();
    	}

    	_n7_max_ph = if_error(data(DevNodeRef(_nid, _N_MAX_N7_PH)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing n=7 max phase value for trigger evaluation");
	    abort();
    	}

    	_n7_min_ph = if_error(data(DevNodeRef(_nid, _N_MIN_N7_PH)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing n=7 min phase value for trigger evaluation");
	    abort();
    	}

    	_max_ratio = if_error(data(DevNodeRef(_nid, _N_MAX_RATIO)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing max ratio value for trigger evaluation");
	    abort();
    	}

    	_min_ratio = if_error(data(DevNodeRef(_nid, _N_MAX_RATIO)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing min ratio value for trigger evaluation");
	    abort();
    	}

    } else {

	    DevLogErr(_nid, "Invalid trigger mode");
	    abort();
    }


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

	if( _trg_mode == _K_EXT_DT )
	{

		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE0 "//trim(adjustl(_n_pulses))//" "//trim(adjustl(_delay_pulse))) ) != "")
		{
			TCPCloseConnection( _sock);
			DevLogErr(_nid, _err_msg); 
			abort();
		}
	}     
	else if( _trg_mode == _K_EXT_10_DT )
    	{
		_delta="";
		for( _i = 0; _i < 10; _i++)
		   _delta = _delta//" "//trim(adjustl(_delay_pulses[ _i ]));

		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE1 "//_delta ) ) != "")
		{
			TCPCloseConnection( _sock);
			DevLogErr(_nid, _err_msg); 
			abort();
		}
	}
	else  if( _trg_mode == _K_EXT_RT )
    	{
		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE0" ) ) != "")
		{
			TCPCloseConnection( _sock);
			DevLogErr(_nid, _err_msg); 
			abort();
		}
	}


	TCPCloseConnection( _sock);

	return (1);

}