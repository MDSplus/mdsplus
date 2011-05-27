public fun LASER_YAG_ReadErr( in _handle )
{
     _len = 2;

	_status_code = zero([_len], 0B);	

	/* Check the status or error code*/

	_status_code = RS232Read(_handle, _len);
		
	if( size( _status_code ) == 0 )
	{
		_status_code[ 69, 56 ];
	}
	
	return ( _status_code[0 : _len] );

 }















