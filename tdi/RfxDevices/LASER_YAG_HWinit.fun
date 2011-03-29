public fun LASER_YAG_HWinit(in _energy, in _repetition_rate, in _pulse_in_burst, in  _sync_delay_lamp, in _sync_delay_diode)
{
    _port = "COM1:";
    _setting = "baud=115200 parity=N data=8 stop=2";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  2; /* NONE */
    _eofChar = 0;

	write(*, "LASER_YAG_HWinit ", _energy, _repetition_rate, _pulse_in_burst, _sync_delay_lamp, _sync_delay_diode);
 
    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}
write(*, "Handler ", _handle);

 
	LASER_YAG_ReadStatus( _handle );

	/* Setting PARAMETERS */

	/********************************************************************
	 setting of ENERY value
  
	Byte 1 '#'		id of the commmand start
	Byte 2  5			Length of command
	Byte 3 'U'		Command
	Byte 4  nn		Nr of the power supply
	Byte 5  -			Upper byte of energy value multiplied by 10
	Byte 6  -			Lower byte of energy value multiplied by 10
	Byte 7 CheckSum	Control sum  
	********************************************************************/
	_energy = int( _energy / 4 * 10 );

	for( _i = 1 ; _i < 5; _i++)
	{
		_cmdBuf = [35BU];  /* # Begin command identifier*/
		_cmdBuf = [  _cmdBuf, 5BU  ]; /* 5 Command length */
		_cmdBuf = [  _cmdBuf, 85BU ]; /* U Command code */
		_cmdBuf = [  _cmdBuf, byte_unsigned( _i ) ]; /*  */
		_cmdBuf = [  _cmdBuf,  byte_unsigned( (_energy /0xFF) ) ]; /* Upper byte energy *10 */
		_cmdBuf = [  _cmdBuf,  byte_unsigned ( mod(  _energy,  0xFF) ) ]; /* Lower byte energy *10 */
		_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) ) ]; /* Checksum  */
    	
write(*,"ENERGY "// _i);

		_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "ENERGY "//_i  );
		if( _status >= 0 )
		{
			RS232Close(_handle);
			return( _status );
		}
	}
	
	/********************************************************************
	Setting of Time of simmer auto switch off
  
	 Byte 1 '#'		id of the commmand start
	 Byte 2  5			Length of command
	 Byte 3 'G'		Command
	 Byte 4  00		Nr of the power supply
	 Byte 5  -			Upper byte of repetition rate value multiplied by 10
	 Byte 6  -			Lower byte of repetition rate value multiplied by 10
	 Byte 7 CheckSum	Control sum  
	********************************************************************/
	
	_auto_switch_off = 60; /* 50 secondi */
	
	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 5BU  ]; /* 5 Command length */
	_cmdBuf = [  _cmdBuf, 71BU ]; /* G Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /*  */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( (_auto_switch_off /0xFF) ) ];	/* Upper byte repetition Rate * 10*/
	_cmdBuf = [  _cmdBuf,  byte_unsigned( mod(  _auto_switch_off,  0xFF) ) ];	/* Lower byte repetition rate * 10 */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*,"TIME of SIMMER AUTO SWITCH OFF");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "SIMMER TIMEOUT 50 s"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}
	
	/********************************************************************
	Quantity of pulses in the burst
  
	 Byte 1 '#'		id of the commmand start
	 Byte 2  5			Length of command
	 Byte 3 'N'		Command
	 Byte 4  00		Nr of the power supply
	 Byte 5  -			Upper byte of repetition rate value multiplied by 10
	 Byte 6  -			Lower byte of repetition rate value multiplied by 10
	 Byte 7 CheckSum	Control sum  
	********************************************************************/
	
	/* _pulse_in_burst = 50; */ 
	
	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 5BU  ]; /* 5 Command length */
	_cmdBuf = [  _cmdBuf, 78BU ]; /* N Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /*  */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( (_pulse_in_burst /0xFF) ) ];
	_cmdBuf = [  _cmdBuf,  byte_unsigned( mod(  _pulse_in_burst,  0xFF) ) ];	
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) )]; /* Checksum  */

write(*,"Nr pulses in burst");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Nr pulses"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}

	/********************************************************************
	Setting of Sync_Pulse_delay
  
	Byte 1 '#'		id of the commmand start
	Byte 2  7			Length of command
	Byte 3 'T'		Command
	Byte 4  00		Nr of the power supply
	Byte 5  -			Upper byte of flash lamp sync
	Byte 6  -			Lower byte of flash lamp sync
	Byte 7  -			Upper byte of diode laser sync
	Byte 8  -			Lower byte of diode laser sync

	Byte 7 CheckSum	Control sum  
	********************************************************************/

	_cmdBuf = [35BU];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 7BU  ]; /* 7 Command length */
	_cmdBuf = [  _cmdBuf, 84BU ]; /* T Command code */
	_cmdBuf = [  _cmdBuf, 0BU ]; /*  */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( (_sync_delay_lamp /0xFF) ) ];			/* Upper byte of flash lamp sync */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( mod(  _sync_delay_lamp,  0xFF) ) ];	/* Lower byte of flash lamp sync */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( (_sync_delay_diode /0xFF) ) ];			/* Upper byte of diode laser sync */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( mod(  _sync_delay_diode,  0xFF) ) ];	/* Lower byte of diode laser sync */
	_cmdBuf = [  _cmdBuf,  byte_unsigned( sum( _cmdBuf  ) ) ];					/* Checksum  */

write(*,"_sync_delay_lamp & _sync_delay_diode");

	_status = LASER_YAG_SetCmd( _handle, _cmdBuf, "Delay Lamp & Diode"  );
	if( _status >= 0 )
	{
		RS232Close(_handle);
		return( _status );
	}
  
	RS232Close(_handle);
	return (-1);

}















