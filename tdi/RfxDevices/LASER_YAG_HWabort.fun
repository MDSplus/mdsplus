public fun LASER_YAG_HWabort(in _port, in _pulse_in_burst)
{

    /* _port = "COM7:"; */

    _setting = "baud=115200 parity=N data=8 stop=2 ";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  2; /* NONE */
    _eofChar = 0;

	write(*, "LASER_YAG_HWabort ");
  
    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}

	
	/*ABORT COMMANDS 
	*/

	/* Simmer arch switch off */
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 's'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x79	Check Sum  
	********************************************************************/
	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 115BU ]; /* s Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*, "Simmer arch switch off ");


	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "SIMMER OFF"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}

	/* Diode master oscillator switch OFF*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 'b'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x68	Check Sum  
	********************************************************************/

	_cmdBuf = [35BU];				/* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 98BU ]; /* b Command code */
	_cmdBuf = [  _cmdBuf, 0BU ];	/* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*, "Diode master oscillator switch OFF ");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "DIODE OFF"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}

	 wait(5.0);
	 wait(_pulse_in_burst);


	/* STOP the power supply 
	concordato con Fassina di non automatizzarlo ma di metterlo in 
	Check List
	*/
	
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 'D'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x69	Check Sum  
  	********************************************************************/

	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 68BU ]; /* D Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*, "STOP the power supply ");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Power OFF"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}
 
	RS232Close(_handle);
	return ( -1 );

}















