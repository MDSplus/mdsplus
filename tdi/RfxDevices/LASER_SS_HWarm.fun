public fun LASER_SS_HWarm()
{
    _port = "COM1:";
    _setting = "baud=115200 parity=N data=8 stop=2 ";
    _binary = 1; /* In binary mode non considera EOF*/
    _handshake =  0; /* XONXOFF */
    _eofChar = 0;

	write(*, "LASER_SS_HWarm ");
 

    _handle = RS232Open(_port,  _setting,  _binary , _handshake ,  _eofChar);
	if( _handle == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}

    
	
	/*COMMANDS to ARM LASER
	*/

	/* Start the power supply 
	----------- concordato con Fassina di non automatizzarlo ma di metterlo in 
	------------Check List

	*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'C'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x69	Check Sum  

	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 67 ]; /* C Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0) 
	{
	   	write(*, "Solid State laser execution command error :  Start the power supply");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error :  Start the power supply");
		return(0);
	}
	********************************************************************/

	/* wait(0.5); */
	
write(*, "Diode master oscillator switch ON ");

	/* Diode master oscillator switch on*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'B'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x68	Check Sum  
	********************************************************************/
	_cmdBuf = [35];				/* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 66 ]; /* B Command code */
	_cmdBuf = [  _cmdBuf, 0 ];	/* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
	{
	   	write(*, "Solid State laser execution command error : switch on");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : switch on");
		return(0);
	}

write(*, "Simmer arch switch  ON ");

	/* Simmer arch switch on*/
	/********************************************************************
	Byte 1 '#'	id of the commmand start
	Byte 2  3		Length of command
	Byte 3 'S'	Command
	Byte 4  00	Nr of the power supply
	Byte 5 0x79	Check Sum  
	********************************************************************/
	_cmdBuf = [35];  /* # Begin command identifier*/
	_cmdBuf = [  _cmdBuf, 3  ]; /* 3 Command length */
	_cmdBuf = [  _cmdBuf, 83 ]; /* S Command code */
	_cmdBuf = [  _cmdBuf, 0 ]; /* Number of the power supply */
	_cmdBuf = [  _cmdBuf,  sum( _cmdBuf  ) ]; /* Checksum  */

	if( RS232Write (_handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
	{
	   	write(*, "Solid State laser execution command error : Simmer arch switch on");
		RS232Close(_handle);
		return(0);
	}

	_status_code=LASER_SS_ReadErr(_handle) ;
	if( _status_code[0] == 'E' )
	{
		LASER_SS_Err(_status_code[1]);
		write(*, "Solid State laser execution command error : Simmer arch switch on");
		return(0);
	}
    
  
	RS232Close(_handle);
	return (1);

}















