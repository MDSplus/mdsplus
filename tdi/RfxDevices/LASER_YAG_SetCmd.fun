public fun LASER_YAG_SetCmd(in _handle, in _cmdBuf, in _cmdName )
{

	if( RS232Write ( _handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
		{
	   		write(*, "YAG laser execution command Error : ", _cmdName);
			return( 9 );
		}
		
	write(*, "Message : ", _cmdBuf );

	_status_code = LASER_YAG_ReadErr(_handle) ;
	if( _status_code[0] == 69 ) /* E = 69 acsii code */
		{
			write(*, "YAG laser execution command Error : ", _cmdName);
			return( _status_code[1] - 48  );
		}	

	LASER_YAG_ReadStatus( _handle );

	return( -1 );
 }















