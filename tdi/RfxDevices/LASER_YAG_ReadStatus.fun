public fun LASER_YAG_ReadStatus(in _handle)
{

wait(2.0);

	/* System Status Request */
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'E'	Command
	Byte 4  00	0 for system
	Byte 5 0x??	Check Sum  
	********************************************************************/
	_cmdBuf = [35BU];				/* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 69BU ]; /* E Command code */
	_cmdBuf = [  _cmdBuf, 0BU ];	/* 0 code for system */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */


write(*, "System Status Request ");

write(*, "Handler ", _handle);

	if( RS232Write ( _handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
		{
	   		write(*, "Solid State laser execution command error : System Status Request ");
			RS232Close(_handle);
			return(0);
		}
	write(*, "Message : ", _cmdBuf);

	_status_code = LASER_YAG_ReadErr(_handle) ;
	
	if( _status_code[0] == 69 ) /* E = 69 acsii code */
		{
			LASER_YAG_Err(_status_code[1] - 48); /* 0 = 48 ascii code */
			write(*, "Solid State laser execution command error : System Status Request ");
			RS232Close(_handle);
			return( _status_code );
		}	

	_len = 7;

	_startus_msg = zero([_len], 0B);	

	/* Check the status or error code*/

	_startus_code = RS232Read(_handle, _len);
	
	_status_byte = _startus_code[4];
	
	_error_byte = _startus_code[5];
	
	write (*,"--------->  STATUS & ERROR BYTE", _status_byte, _error_byte);
	
	/* return( 1 );
	*/
	
	return( _status_byte );
 }
 
 






