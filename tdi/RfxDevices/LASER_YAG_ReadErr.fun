public fun LASER_YAG_ReadErr( in _handle )
{
     _len = 2;

	_startus_code = zero([_len], 0B);	

	/* Check the status or error code*/

	_startus_code = RS232Read(_handle, _len);
	
	if( size( _startus_code ) == 0 )
	{
		_startus_code[ 69, 56 ];
	}
	
	return ( _startus_code[0 : _len] );

 }















