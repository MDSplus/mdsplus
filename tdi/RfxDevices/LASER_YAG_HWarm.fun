public fun LASER_YAG_HWarm( in _repetition_rate, in _timeWait)
{
    _port = "COM1:";
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

	/* Start the power supply 
	*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'C'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x69	Check Sum  
	********************************************************************/

	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 67BU ]; /* C Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*,"Start the power supply");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Start the power supply"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}
  
write(*, "Wait 5 sec ....");
	 wait(5.0);

/********************************************************************
/********************************************************************
/********************************************************************
/********************************************************************
	Setting of REPETITION RATE - to be set after ttransmission of the command Start
  
	 Byte 1 '#'		id of the commmand start
	 Byte 2  5			Length of command
	 Byte 3 'R'		Command
	 Byte 4  00		Nr of the power supply
	 Byte 5  -			Upper byte of repetition rate value multiplied by 10
	 Byte 6  -			Lower byte of repetition rate value multiplied by 10
	 Byte 7 CheckSum	Control sum  
	
		
	_repetition_rate = int( _repetition_rate * 10 );
	
	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 5BU  ]; /* 5 Command length */
	_cmdBuf = [  _cmdBuf, 82BU ]; /* R Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /*  */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( (_repetition_rate /0xFF) ) ];	/* Upper byte repetition Rate * 10*/
	_cmdBuf = [  _cmdBuf,  byte_unsigned( mod(  _repetition_rate,  0xFF) ) ];	/* Lower byte repetition rate * 10 */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*,"REPETITION RATE");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "REPETITION RATE"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}
********************************************************************/
********************************************************************/
********************************************************************/
********************************************************************/


	/* Diode Master Oscillator switch on
		Comando non necessario (viene eseguito in fase di Start)
	*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'B'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x68	Check Sum  
	********************************************************************/
	_cmdBuf = [35BU];				/* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3BU  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 66BU ]; /* B Command code */
	_cmdBuf = [  _cmdBuf, 0BU ];	/* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*, "Diode master oscillator switch ON ");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Diode ON"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}

	 if( _timeWait < 0 || _timeWait > 35 )
		_timeWait = 35;
		
	 write(*, "Wait "// _timeWait //" sec ....");
	 wait( _timeWait );
	 
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















