public fun CHVPSResetCommand(in _name, in _nid, in _crate_num, inout _old_group_B) 
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

	_target = (_crate_num << 8) | _K_GROUP_B;   

/* Select Target */
		
	_a = 0;
	_f = _K_SELECT;
	_count = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _target, 16));
	if (~_status & 1) return (0);

	_a = 0;
	_f = _K_READ;
	_count = 1;
	_error = word(0);
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _error, 16));
	if (~_status & 1) return (0);

	
	if( _error < 0 )
	{
		DevLogErr(_nid, "CHVPS command execution error : "//_error);
		return (0);
	}

/* Read channels defined into group  B */

	_a = _K_GROUP_BIT;
	_f = _K_SELECT;
	_w = _K_START_READOUT;
	_size = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _size, _w, 16));
	if (~_status & 1) return (0);


	wait(_wait_val);

	_a = 0;
	_f = _K_READ;
	_size = 3;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _size, _old_group_B, 16));
	if (~_status & 1) return (0);
	
	wait(_wait_val);

	_target -= _K_GROUP_B;

	for (_i = 0;  _i < 40; _i++)
	{
		if(_old_group_B[ _i / 16 ] & 1 << mod(_i,16))
		{
			_status = CHVPSSendCommand(_name, 0, _K_SELECT, _target, _error);      
			if (_status & 1 && !(_error < 0)) 
				_status = CHVPSSendCommand(_name, _K_GROUP_BIT, _K_WRITE, 1, _error);
		}
			
		if (~_status & 1 || _error < 0) 
		{
			if(_error < 0) 
				DevLogErr(_nid, "CHVPS command execution error : "//_error);
			return (0);
		}

		_target++;
	}

	return(1);    
};

