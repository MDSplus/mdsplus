public fun OAM01__calib ( as_is _nid, optional _method )
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







	_status = 1 ;
	_gpib_ctrl = "" ;
	_gpib_ctrl = if_error ( data ( DevNodeRef ( _nid, __GPIB_CTRL ) ), _status = 0 ) ;
	if (  ( _status == 0 ) || ( _gpib_ctrl == "" ) )
	{
		DevLogErr ( _nid, "Invalid GPIB controller name" ) ;
		abort (  ) ;
	}

	_status = 1 ;
	_gpib_addr = if_error ( data ( DevNodeRef ( _nid, __GPIB_ADDR ) ), _status = 0 ) ;
	if ( _status == 0 )
	{
		DevLogErr ( _nid, "Invalid GPIB address" ) ;
		abort (  ) ;
	}




	/* predispongo in remoto */

	_command = "W5836(80)" ;





	for ( _channel = 0 ; _channel < _CHANNELS ; _channel ++ )
	{
		/* seleziono il canale da configurare */
		_command = "W5834(0" // Trim ( AdjustL ( ( 1 + _channel ) ) ) // ")" ;
		write(*, "ch: ", _command);

		/* calibro il canale */
		_command = "W5836(C501)" ;
		write(*, "ch: ", _command);
	}

	/* mi metto sul canale virtuale */
	_command = "W5834(00)" ;
	write(*, "ch: ", _command);
	/* rimetto il controllo in locale */
	_command = "W5836(00)" ;
	write(*, _command);

	return ( 1 ) ;
}