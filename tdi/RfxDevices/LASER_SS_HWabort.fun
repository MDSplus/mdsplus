public fun LASER_SS_HWabort()
{
  
     _port = "COM1:";
    _setting = "baud=115200 parity=N data=8 stop=2 ";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  0; /* XONXOFF */
    _eofChar = 0;

	write(*, "LASER_SS_HWabort ");
  
 
    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}



write(*, "Simmer arch switch off ");

	
	/*ABORT COMMANDS 
	*/

	/* Simmer arch switch off */
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 's'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x99	Check Sum  
	********************************************************************/
	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 115 ]; /* s Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0) 
	{
	   	write(*, "Solid State laser execution command error : Simmer arch switch off");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle);
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : Simmer arch switch off");
		return(0);
	}

write(*, "Diode master oscillator switch OFF ");

	/* Diode master oscillator switch OFF*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 'b'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x88	Check Sum  
	********************************************************************/
	_cmdBuf = [35];				/* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 98 ]; /* b Command code */
	_cmdBuf = [  _cmdBuf, 0 ];	/* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0) 
	{
	   	write(*, "Solid State laser execution command error : oscillator switch OFF");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : oscillator switch OFF");
		return(0);
	}



	/* STOP the power supply 
	concordato con Fassina di non automatizzarlo ma di metterlo in 
	Check List
	*/
	
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3	Length of command
	Byte 3 'D'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x6A	Check Sum  

	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 68 ]; /* D Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0) 
	{
	   	write(*, "Solid State laser execution command error : TOP the power supply");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : TOP the power supply");
		return(0);
	}
  
  	********************************************************************/

  
  
	RS232Close(_handle);
	return (1);

}















