public fun OAM01__reset ( as_is _nid, optional _method )
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





	/* Inizializzo GPIB */

	_status = GPIBInit ( ) ;
	if ( 0 == _status )
	{
		DevLogErr ( _nid, 'GPIB initialization failed' ) ;
		abort (  ) ;
	}


	/* Ricavo l'identificatore GPIB */
 
   	_gpib_id = GPIBGetId ( _gpib_addr ) ;
    	if ( 0 == _gpib_id )
    	{
		DevLogErr ( _nid, 'Invalid GPIB identifier' ) ; 
		abort ( ) ;
    	}




	/* predispongo il controllo in remoto */

	_command = 'W5836(80)\n' ;
	_status = GPIBWrite ( _gpib_id, _command ) ;
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}


	/*
		faccio un reset. Il reset spegne i canali, carica una configurazione di
		default e porta il controllo in locale
	*/

	_command = 'J0000\n' ;
	_status = GPIBWrite ( _gpib_id, _command ) ;
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}


	/* Rilascio il device GPIB */

	_status = GPIBClrId ( _gpib_id ) ;
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}

	return ( 1 ) ;
}