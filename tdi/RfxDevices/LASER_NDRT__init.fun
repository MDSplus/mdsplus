public fun LASER_NDRT__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 23;

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
    private _N_MAX_AMP		= 10;
    private _N_MIN_AMP		= 11;
    private _N_MAX_PHASE	= 12;
    private _N_MIN_PHASE	= 13;
    private _N_OSC		= 14;
    private _N_AMP		= 15;
    private _N_SLAB		= 16;

    private _K_EXT_DT		= 0;
    private _K_EXT_10_DT	= 1;
    private _K_EXT_RT		= 2;

    private _K_DISABLE_RT	= 0;
    private _K_M1_N7_AMP	= 1;
    private _K_M1_N7_AMP_PH	= 2;
    private _K_RATIO_DOM_SEC	= 3;
    private _K_RATIO_DOM_SEC_PH	= 4;
    private _K_M0_N1_AMP	= 5;
    private _K_M0_N1_AMP_PH	= 6;
    private _K_M0_N7_AMP	= 7;
    private _K_M0_N7_AMP_PH	= 8;



    private _ASCII_MODE = 0;
    private _MIN_DT = 0.025;
    private _MIN_To = -0.025 - 0.0055;
	

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
	DevLogErr(_nid, "Missing REAL TIME node IP address"); 
	abort();
    }
write(*, _ip_rt);


    _to = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE	)), _error = 1);

write(*, _to);

    if( _error || _to < _MIN_To )
    {
	DevLogErr(_nid, "Trigger time (To) is missing or less than -25ms "); 
	abort();
    }
write(*, _to);


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

    	_delay_pulses = if_error(data(DevNodeRef(_nid, _N_DELAY_PULSE)), _error = 1);
    	if(_error)
    	{
			DevLogErr(_nid, "Missing delay between pulse");
			abort();
    	}
		
		if( _delay_pulses[0] < _MIN_DT )
		{
			_delay_pulses = [ _MIN_DT, zero(9) ];
			DevPut( _nid, _N_DELAY_PULSE, _delay_pulses );
		}

		write(*, _delay_pulses );


    } 
    else if( _trg_mode == _K_EXT_10_DT )
    {
 
    	_delay_pulses = if_error(data(DevNodeRef(_nid, _N_DELAY_PULSE)), _error = 1);
    	if( _error || size( _delay_pulses ) < 10 )
    	{
			DevLogErr(_nid, "Must be defined 10 delay values");
			abort();
    	}
		
		_checked_delay = [];
		_to_save = 0;
		for( _i = 0; _i < size( _delay_pulses ) ; _i++)
		{
			if( _delay_pulses[ _i ] < _MIN_DT )
			{
				_checked_delay = [ _checked_delay , _MIN_DT ];
				_to_save = 1;
			}
			else
			{
				_checked_delay = [ _checked_delay , _delay_pulses[ _i ] ];
			}
		}
		
		if( _to_save )
		{
			_delay_pulses = _checked_delay;
			DevPut( _nid, _N_DELAY_PULSE, _delay_pulses );			
		}
		
		write(*, _delay_pulses );

    } 
    else if( _trg_mode == _K_EXT_RT ) 
    {

	Write(*, "CONFIGURED REAL TIME TRIGGER");

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

		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE "//trim(adjustl(_n_pulses))//" "//trim(adjustl(_delay_pulses[0]))) ) != "")
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
		{
		   _delta = _delta//" "//trim(adjustl(_delay_pulses[ _i ]));
		}
		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE_1 "//_delta ) ) != "")
		{
			TCPCloseConnection( _sock);
			DevLogErr(_nid, _err_msg); 
			abort();
		}
	}
	else  if( _trg_mode == _K_EXT_RT )
	{

		if((_err_msg = TCPSendCommand( _sock, "ND_CHARGE_2" ) ) != "")
		{
			TCPCloseConnection( _sock);
			DevLogErr(_nid, _err_msg); 
			abort();
		}
		
		
	}

	TCPCloseConnection( _sock);

	return (1);

}