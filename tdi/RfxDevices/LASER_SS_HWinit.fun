public fun LASER_SS_HWinit(in _num_power_supply, in _energy, in _repetition_rate, in  _sync_delay_lamp, in _sync_delay_diode)
{
    _port = "COM1:";
    _setting = "baud=115200 parity=N data=8 stop=2";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  0; /* XONXOFF */
    _eofChar = 0;

	write(*, "LASER_SS_HWinit ", _num_power_supply,  _energy, _repetition_rate, _sync_delay_lamp, _sync_delay_diode);
 
    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}

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

	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 5  ]; /* 5 Command length */
	_cmdBuf = [  _cmdBuf, 85 ]; /* U Command code */
	_cmdBuf = [  _cmdBuf, _num_power_supply ]; /*  */
	_cmdBuf = [  _cmdBuf,  (_energy /0xFF)*10 ]; /* Upper byte energy *10 */
	_cmdBuf = [  _cmdBuf,   mod(  _energy,  0xFF) *10 ]; /* Lower byte energy *10 */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */
    	
write(*,"ENERGY");

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
	{
	   	write(*, "Solid State laser execution command error : ENERGY");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error ENERGY ");
		return(0);
	}


	/********************************************************************
	Setting of REPETITION RATE
  
	 Byte 1 '#'		id of the commmand start
	 Byte 2  5			Length of command
	  Byte 3 'R'		Command
	 Byte 4  00		Nr of the power supply
	 Byte 5  -			Upper byte of repetition rate value multiplied by 10
	 Byte 6  -			Lower byte of repetition rate value multiplied by 10
	 Byte 7 CheckSum	Control sum  
	********************************************************************/
	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 5  ]; /* 5 Command length */
	_cmdBuf = [  _cmdBuf, 82 ]; /* R Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /*  */
	_cmdBuf = [  _cmdBuf,  (_repetition_rate /0xFF)*10 ];			/* Upper byte repetition Rate * 10*/
	_cmdBuf = [  _cmdBuf,   mod(  _repetition_rate,  0xFF) *10 ];	/* Lower byte repetition rate * 10 */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

write(*,"REPETITION RATE");

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
	{
	   	write(*, "Solid State laser execution command error : REPETITION RATE");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : REPETITION RATE");
		return(0);
	}

	/********************************************************************
	setting of Sync_Pulse_delay
  
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

	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 7  ]; /* 7 Command length */
	_cmdBuf = [  _cmdBuf, 84 ]; /* T Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /*  */
	_cmdBuf = [  _cmdBuf,  (_sync_delay_lamp /0xFF)];			/* Upper byte of flash lamp sync */
	_cmdBuf = [  _cmdBuf,   mod(  _sync_delay_lamp,  0xFF) ];	/* Lower byte of flash lamp sync */
	_cmdBuf = [  _cmdBuf,  (_sync_delay_diode /0xFF)];			/* Upper byte of diode laser sync */
	_cmdBuf = [  _cmdBuf,   mod(  _sync_delay_diode,  0xFF) ];	/* Lower byte of diode laser sync */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ];					/* Checksum  */

write(*,"Sync_Pulse_delay");

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
	{
	   	write(*, "Solid State laser execution command error : Sync_Pulse_delay");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : Sync_Pulse_delay");
		return(0);
	}
  
  
	RS232Close(_handle);
	return (1);

}















