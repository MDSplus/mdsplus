public fun LASER_RU_HWdump(in _port)
{
    private _K_NUM_BITS = 5;
    
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
		return(0);
	}

    _cmnd =  "dterm 13";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
	{
		RS232Close(_hComm);
		return(0);
	}


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

	RS232Close(_hComm);

	return (1);

}















