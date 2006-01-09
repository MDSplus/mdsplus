public fun LASER_RU_HWreset(in _port, in _bit_states)
{

    private _K_NUM_BITS = 5;
	private _K_NUM_DATA = 8;
    
	private _RS232_XONXOFF = 0;
    private _RS232_CTSDSR  = 1;

    private _TRIG_INTERNAL  = 0;
    private _TRIG_EXTERNAL  = 2;
    private _TRIG_AUTOMATIC = 3;
    private _TRIG_MANUAL    = 4;


    _hComm = RS232Open(_port, "baud=1200 parity=N data=8 stop=1", 0, _RS232_XONXOFF, 13);
	if( _hComm == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return( 0 );
	}


   /* Set CR as command terminator */
	_cmnd =  "dterm 13";
    if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
	{
		RS232Close(_hComm);
		return(0);
	}


/* Set all digital output */
    for(_i = 0, _bit = 0; _i < _K_NUM_BITS; _i++, _bit++)
	{
		if(_i == 3) _bit++;

		if(_bit_states[_i])
			_cmnd =  "on "//_bit;
		else
			_cmnd =  "off "//_bit;

		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}
	}


/* Read analog input */
	_data = [];
    for( _i = 0; _i < _K_NUM_DATA; _i++)
	{
		_cmnd = "adc "//_i;
		_val = LASER_RUSendCommand(_hComm,  _cmnd);
		if( _val < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}
		_data = [_data, _val];
    }

	_cmnd = "hip";
	_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
	if(_retvalue < 0)
	{
		RS232Close(_hComm);
		return(0);
	}

	if(_retvalue == 49) 
	{
/* Dump sequence */
		_cmnd =  "dtime1 2 2 2 2";
		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}

		_cmnd =  "use1";
		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}

		_cmnd =  "dump 0";
		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}

		_cmnd =  "charge 0";
		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
		{
			RS232Close(_hComm);
			return(0);
		}

		wait(1.0);

	}

	RS232Close(_hComm);

	return ( _data );

}















