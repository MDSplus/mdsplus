public fun LASER_YAG_HWReadStatus()
{
    _port = "COM7:";
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
	----------- concordato con Fassina di non automatizzarlo ma di metterlo in 
	------------Check List
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

	if( RS232Write ( _handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
		{
	   		write(*, "Solid State laser execution command error : Start the power supply ");
			RS232Close(_handle);
			return(0);
		}
	write(*, "Message : ", _cmdBuf );

	_status_code = LASER_YAG_ReadErr(_handle) ;
	if( _status_code[0] == 69 ) /* E = 69 acsii code */
		{
			LASER_YAG_Err(_status_code[1] - 48); /* 0 = 48 ascii code */
			write(*, "Solid State laser execution command error : Start the power supply ");
			RS232Close(_handle);
			return( _status_code );
		}	
  

	wait(2.5); 
			


	/* Diode master oscillator switch on*/
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

	if( RS232Write ( _handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
		{
	   		write(*, "Solid State laser execution command error : Diode master oscillator switch ON ");
			RS232Close(_handle);
			return(0);
		}
	write(*, "Message : ", _cmdBuf );

	_status_code = LASER_YAG_ReadErr(_handle) ;
	if( _status_code[0] == 69 ) /* E = 69 acsii code */
		{
			LASER_YAG_Err(_status_code[1] - 48); /* 0 = 48 ascii code */
			write(*, "Solid State laser execution command error : Diode master oscillator switch ON ");
			RS232Close(_handle);
			return( _status_code );
		}	
	 LASER_YAG_ReadStatus( _handle );
	
	 wait(15.0);


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

	if( RS232Write ( _handle,  _cmdBuf,  size(_cmdBuf)) == 0 ) 
		{
	   		write(*, "Solid State laser execution command error : Simmer arch switch  ON ");
			RS232Close(_handle);
			return(0);
		}
	write(*, "Message : ", _cmdBuf );

	_status_code = LASER_YAG_ReadErr(_handle) ;
	if( _status_code[0] == 69 ) /* E = 69 acsii code */
		{
			LASER_YAG_Err(_status_code[1] - 48); /* 0 = 48 ascii code */
			write(*, "Solid State laser execution command error : Simmer arch switch  ON ");
			RS232Close(_handle);
			return( _status_code );
		}
		
   	LASER_YAG_ReadStatus( _handle ); 
	
	wait(5.0);
  
	LASER_YAG_ReadStatus( _handle ); 
  
	RS232Close(_handle);
	return (1);

}















