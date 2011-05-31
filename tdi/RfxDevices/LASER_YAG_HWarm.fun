public fun LASER_YAG_HWarm(in _port, in _repetition_rate, in _timeWait)
{
   /* _port = "COM/:"; */
    _setting = "baud=115200 parity=N data=8 stop=2 ";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  2; /* NONE */
    _eofChar = 0;

	write(*, "LASER_SS_HWarm ");
 
    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}

write(*, "Handler ", _handle);
	
	/*
	COMMANDS to ARM LASER
	*/

	 if( _timeWait < 0 || _timeWait > 40 )
		_timeWait = 40;
		
	 write(*, "Wait "// _timeWait //" sec ....");
	 wait( _timeWait );
	 
	/* Verifico per 3 volte che il diodo laser sia Ready 
	*/
	while( _i < 3)
	{
	_status_byte = LASER_YAG_ReadStatus( _handle );
	_mask = 1 << 1;
	if ((_status_byte && _mask) == 1) /* Diode laser Ready = bit 1 dello status byte*/
		i=3;
	_i++;
	}
	
	
	/* Simmer arch switch on*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'S'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x79	Check Sum  
	********************************************************************/
	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 83BU ]; /* S Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*, "Simmer arch switch  ON ");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Simmer ON"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}

	 
	
	RS232Close(_handle);
	return (-1);

}















