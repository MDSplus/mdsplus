public fun CHVPSSendCommand(in _name, in _a, in _f, in _w, inout _error) 
{
 
/*  CAMAC functions definition	*/
    private _K_READ	    = 0;
    private _K_WRITE	    = 16;
    private _K_SELECT	    = 17;

/*  CAMAC arguments definition	*/
    private _K_V0	    = 0;
    private _K_V1	    = 1;
    private _K_I0	    = 2;
    private _K_I1	    = 3;
    private _K_RAMP_UP	    = 4;
    private _K_RAMP_DOWN   = 5;
    private _K_TRIP	    = 6;
    private _K_STATUS_BIT  = 7;
    private _K_GROUP_BIT   = 8;
    private _K_CHAN_TYPE   = 13;
    private _K_GROUP_A	    = 51;
    private _K_GROUP_B	    = 52;

/*  CAMAC control definition	*/
    private _K_HV_ON		= 1;
    private _K_HV_OFF		= 0;
    private _K_BIT_GROUP_A	= 2;
    private _K_BIT_GROUP_B	= 5;
    private _K_START_READOUT	= 0xff00;

    private _wait_val = 0.01;

	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  1, _w, 16));      
	wait(_wait_val);
 	if (_status & 1) 
		_status = DevCamChk(_name, CamQrepw( _name, 0, _K_READ,  1, _error, 16));     
	wait(_wait_val);

	return (_status);
};

