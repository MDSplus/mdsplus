public fun LASER_RU_HWcheck(in  _port)
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
    	DevLogErr(_nid, "Cannot open RS232 port : "//RS232GetError() );
		abort();
	}

   /* Set CR as command terminator */
	_cmnd =  "dterm 13";
    if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
	{
		RS232Close(_hComm);
		abort();
	}
	_cmnd = "hip";
	_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
	if(_retvalue < 0)
	{
		RS232Close(_hComm);
		abort();
	}

	RS232Close(_hComm);

	return ( _retvalue );

}















