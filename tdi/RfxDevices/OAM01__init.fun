public fun OAM01__init ( as_is _nid, optional _method )
{
	private __GPIB_CTRL = 1 ;
	private __GPIB_ADDR = 2 ;
	private __MODULE_TYPE = 3 ;
	private __COMMENT = 4 ;

	private __CHANNEL_1A = 5 ;
	private __RANGE = 1 ;
	private __COUPLING = 2 ;
	private __SOURCE = 3 ;
	private __INPUT = 4 ;
	private __OUTPUT = 5 ;


	private	_CHANNELS = 8 ;
	private	_NODES_PER_CHANNEL = 6 ;

	private _WAIT = 1. ;







	/* Ricavo il nid della testa del conglomerato */

	_nid_head = getnci ( _nid, 'nid_number' ) ;



	_status = 1 ;
	_gpib_ctrl = '' ;
	_gpib_ctrl = if_error ( data ( DevNodeRef ( _nid, __GPIB_CTRL ) ), _status = 0 ) ;
	if (  ( _status == 0 ) || ( _gpib_ctrl == '' ) )
	{
		DevLogErr ( _nid, 'Invalid GPIB controller name' ) ;
		abort (  ) ;
	}

	_status = 1 ;
	_gpib_addr = if_error ( data ( DevNodeRef ( _nid, __GPIB_ADDR ) ), _status = 0 ) ;
	if ( _status == 0 )
	{
		DevLogErr ( _nid, 'Invalid GPIB address' ) ;
		abort (  ) ;
	}

	DevNodeCvt ( _nid, __MODULE_TYPE, [ '5V OUTPUT', '0.5V OUTPUT' ], [ 5, 0.5 ], _module_type = 0.5 ) ;




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




	for ( _channel = 0 ; _channel < _CHANNELS ; _channel ++ )
	{
		/* Ricavo il nome del canale corrente */

		_channel_name = element ( 2, '_', getnci ( _nid_head + __CHANNEL_1A + ( _channel * _NODES_PER_CHANNEL ), 'fullpath' ) ) ;


		/* Ricavo il nid del campo output corrente */

		_nid_output = getnci ( _nid_head + __CHANNEL_1A + __OUTPUT + ( _channel * _NODES_PER_CHANNEL ), 'fullpath' ) ;


		/* seleziono il canale da configurare */

		_command = 'W5834(0' // Trim ( AdjustL ( ( 1 + _channel ) ) ) // ')' // '\n' ;
		_status = GPIBWrite ( _gpib_id, _command ) ;
		wait ( _WAIT ) ;
		if ( 0 == _status )
		{
			_msg = 'Command ' // _command // ' failed' ;
			DevLogErr ( _nid, _msg ) ;
			abort (  ) ;
		}




		if ( DevIsOn ( DevNodeRef ( _nid, __CHANNEL_1A + ( _channel * _NODES_PER_CHANNEL ) ) ) )
		{
			DevNodeCvt ( _nid, __CHANNEL_1A + __RANGE + ( _channel * _NODES_PER_CHANNEL ), [ 250, 100, 50, 25, 10, 5, 2.5, 1, 0.5 ], [ '00', '01', '02', '03', '04', '05', '06', '07', '08' ], _range = '00' ) ;

			DevNodeCvt ( _nid, __CHANNEL_1A + __RANGE + ( _channel * _NODES_PER_CHANNEL ), [ 250, 100, 50, 25, 10, 5, 2.5, 1, 0.5 ], [ 250, 100, 50, 25, 10, 5, 2.5, 1, 0.5 ], _rangen = 250 ) ;

			DevNodeCvt ( _nid, __CHANNEL_1A + __COUPLING + ( _channel * _NODES_PER_CHANNEL ), [ 'AC', 'DC' ], [ '01', '00' ], _coupling = '01' ) ;

			DevNodeCvt ( _nid, __CHANNEL_1A + __SOURCE + ( _channel * _NODES_PER_CHANNEL ), [ 'INPUT', 'ZERO', 'REF', 'BAT' ], [ '00', '01', '02', '03' ], _source = '00' ) ;

			_status = 1 ;
			_output = if_error ( data ( DevNodeRef ( _nid, __CHANNEL_1A + __OUTPUT + ( _channel * _NODES_PER_CHANNEL ) ) ), _status = 0 ) ;
			if (  ( _status == 0 ) || ( _output == '' ) )
			{
				_msg = 'Invalid output in channel ' // _channel_name ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}

			DevPut ( _nid, __CHANNEL_1A + __INPUT + ( _channel * _NODES_PER_CHANNEL ), compile ( Trim ( AdjustL ( _rangen / _module_type ) ) // '*' // _nid_output ) ) ;



			/* metto ON il canale */

			_command = 'W5836(C401)\n' ;
			_status = GPIBWrite ( _gpib_id, _command ) ;
			wait ( _WAIT ) ;
			if ( 0 == _status )
			{
				_msg = 'Command ' // _command // ' failed' ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}




			/* setto il range */

			_command = 'W5836(C0' // _range // ')' // '\n' ;
			_status = GPIBWrite ( _gpib_id, _command ) ;
			wait ( _WAIT ) ;
			if ( 0 == _status )
			{
				_msg = 'Command ' // _command // ' failed' ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}


			/* setto il coupling */

			_command = 'W5836(C1' // _coupling // ')' // '\n' ;
			_status = GPIBWrite ( _gpib_id, _command ) ;
			wait ( _WAIT ) ;
			if ( 0 == _status )
			{
				_msg = 'Command ' // _command // ' failed' ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}


			/* setto il source */

			_command = 'W5836(C2' // _source // ')' // '\n' ;
			_status = GPIBWrite ( _gpib_id, _command ) ;
			wait ( _WAIT ) ;
			if ( 0 == _status )
			{
				_msg = 'Command ' // _command // ' failed' ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}
		}
		else
		{
			/* metto OFF il canale */

			_command = 'W5836(C400)\n' ;
			_status = GPIBWrite ( _gpib_id, _command ) ;
			wait ( _WAIT ) ;
			if ( 0 == _status )
			{
				_msg = 'Command ' // _command // ' failed' ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}
		}
	}


	/* mi posiziono sul canale virtuale */

 	_command = 'W5834(00)\n' ;
	_status = GPIBWrite ( _gpib_id, _command ) ;
	wait ( _WAIT ) ;
	if ( 0 == _status )
	{
		_msg = 'Command ' // _command // ' failed' ;
		DevLogErr ( _nid, _msg ) ;
		abort (  ) ;
	}


	/* predispongo il controllo in locale */

	_command = 'W5836(00)\n' ;
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

