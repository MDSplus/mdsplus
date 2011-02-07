public fun OAM01__info ( as_is _nid, optional _method )
{
	private __GPIB_ADDR = 1 ;
	private __MODULE_TYPE = 2 ;
	private __COMMENT = 3 ;

	private __CHANNEL_1A = 4 ;
	private __RANGE = 1 ;
	private __COUPLING = 2 ;
	private __SOURCE = 3 ;
	private __INPUT = 4 ;
	private __OUTPUT = 5 ;


	private __SW_MODE = 52 ;
	private __IP_ADDR = 53 ;

	private	_CHANNELS = 8 ;
	private	_NODES_PER_CHANNEL = 6 ;

	private _WAIT = 1. ;









	_status = 1 ;
	_gpib_addr = if_error ( data ( DevNodeRef ( _nid, __GPIB_ADDR ) ), _status = 0 ) ;
	if ( _status == 0 )
	{
		DevLogErr ( _nid, 'Invalid GPIB address' ) ;
		abort (  ) ;
	}





    DevNodeCvt(_nid, __SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 1);

	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, __IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid IP");
 		    abort();
		}
	}


	/* Inizializzo GPIB */

	if (_remote)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
	    	execute(_cmd);
	    	_status = MdsValue('GPIBInit()');
	}
	else
	{
		_status = GPIBInit ( ) ;
	}

	if ( 0 == _status )
	{
		DevLogErr ( _nid, 'GPIB initialization failed' ) ;
		abort (  ) ;
	}


	/* Ricavo l'identificatore GPIB */
 
	if (_remote)
	{
		_gpib_id = MdsValue('GPIBGetId($1)', _gpib_addr);
	}
	else
	{
	   	_gpib_id = GPIBGetId ( _gpib_addr ) ;
	}

    	if ( 0 == _gpib_id )
    	{
		DevLogErr ( _nid, 'Invalid GPIB identifier' ) ; 
		abort ( ) ;
    	}




	/* predispongo il controllo in remoto */

	_command = 'W5836(80)\n' ;
	if (_remote)
	{
		_status = MdsValue('GPIBWrite($1, $2)', _gpib_id, _command);
	}
	else
	{
		_status = GPIBWrite ( _gpib_id, _command ) ;
	}
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}





	for ( _channel = 0 ; _channel < _CHANNELS ; _channel ++ )
	{
		/* seleziono il canale da cui leggere */

		_command = 'W5834(0' // Trim ( AdjustL ( ( 1 + _channel ) ) ) // ')' // '\n' ;
		if (_remote)
		{
			_status = MdsValue('GPIBWrite($1, $2)', _gpib_id, _command);
		}
		else
		{
			_status = GPIBWrite ( _gpib_id, _command ) ;
		}
		wait ( _WAIT ) ;
		if ( 0 == _status )
		{
			_msg = 'Command ' // _command // ' failed' ;
			DevLogErr ( _nid, _msg ) ;
			abort (  ) ;
		}


		/* leggo lo stato del canale */

	}


	/* mi posiziono sul canale virtuale */

 	_command = 'W5834(00)\n' ;
	if (_remote)
	{
		_status = MdsValue('GPIBWrite($1, $2)', _gpib_id, _command);
	}
	else
	{
		_status = GPIBWrite ( _gpib_id, _command ) ;
	}
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}


	/* predispongo il controllo in locale */

	_command = 'W5836(00)\n' ;
	if (_remote)
	{
		_status = MdsValue('GPIBWrite($1, $2)', _gpib_id, _command);
	}
	else
	{
		_status = GPIBWrite ( _gpib_id, _command ) ;
	}
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}

	if ( _remote )
	{
		_status = MdsValue('GPIBClean($1)', _gpib_id);

	    	MdsDisconnect();
	}
	else
	{
	    _status = GPIBClean(_gpib_id);
	}




	return ( 1 ) ;
}
