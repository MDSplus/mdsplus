public fun LASER_RU_HWtrigger(in _port, in _trig_mode)
{

    private _RS232_XONXOFF = 0;
    private _RS232_CTSDSR  = 1;

    private _TRIG_INTERNAL  = 0;
    private _TRIG_EXTERNAL  = 2;
    private _TRIG_AUTOMATIC = 3;
    private _TRIG_MANUAL    = 4;



    	_hComm = RS232Open(_port, "baud=1200 parity=N data=8 stop=1", 0, _RS232_XONXOFF, 13);
	if( _hComm == 0 )
	{
    		write(*,  "Cannot open RS232 port : "//RS232GetError() );
		return( 0 );
	}

	if (_trig_mode == _TRIG_INTERNAL) 
	{   
	   /* Set CR as command terminator */
		_cmnd =  "dterm 13";
		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return( 0 );
		}

		_cmnd = "hip";
		_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
		if(_retvalue < 0)
		{
			RS232Close(_hComm);
			return( 0 );
		}

		if(_retvalue == 49) {
		    
			_cmnd = "fire1";
			if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
			{
				RS232Close(_hComm);
				return( 0 );
			}

			_cmnd = "hip";
			_i = 0;
			_retvalue = 0;
			while (_i < 10  && _retvalue != 240) 
			{
				wait(1.0);
				_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
				if(_retvalue < 0)
				{
					RS232Close(_hComm);
					return( 0 );
				}
				_i++;
			}
			if(_i == 10 && _retvalue != 240)
			{
				write(*,  "Ruby laser do not fire"); 
			}

	    } else write(*,  "Ruby laser do not ready"); ;

	} 
	else write(*,  "Trigger mode must be internal to perform trigger operation" );

	RS232Close(_hComm);

	return (1);

}















