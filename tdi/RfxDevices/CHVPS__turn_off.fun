public fun CHVPS__turn_off(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 487;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_NAME = 2;
    private _N_CRATE_NUMBER = 3;

    private _K_NUM_SET = 40;
    private _K_NODES_PER_SET = 12;
    private _N_SET_00 = 4;

    private _N_SET_COMMENT = 1;
    private _N_SET_ADDRESS = 2;
    private _N_SET_CHAN_TYPE = 3;
    private _N_SET_I0 = 4;
    private _N_SET_I1 = 5;
    private _N_SET_RAMP_DOWN = 6;
    private _N_SET_RAMP_UP = 7;
    private _N_SET_RESET = 8;
    private _N_SET_TRIP = 9;
    private _N_SET_V0 = 10;
    private _N_SET_V1 = 11;

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
    private _K_GROUP_ALL	= 50;
    private _K_START_READOUT	= 0xff00;

    private _K_DISABLE  = 0;
    private _K_ENABLE   = 1;

    private _K_CA334_TYPE  = 129; /* 4 negative HV channels 2 KV 3 mA */
    private _K_CA332_TYPE   = 12; /* 4 positive HV channels 6 KV 1 mA */

    private _K_TABLE_SIZE   = 2;
  
    private _K_MIN_VOLTAGE	= 0;
    private _K_MAX_VOLTAGE	= 1;
    private _K_MAX_CURRENT	= 2;
    private _K_CURR_FACTOR	= 3;
    private _K_VOLT_FACTOR	= 4;
    private _K_MAX_RATE		= 5;

    private _data_sheet_t  = [ [ -2000,    0, 3000, 1, 2, 250 ],
			       [     0, 6000, 1000, 1, 1, 500 ] ];

	private _wait_val = 0.01;


	_error = 0;

	_name = if_error(data(DevNodeRef(_nid, _N_NAME)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing camac name"); 
		abort();
	}

	_crate_num = if_error(data(DevNodeRef(_nid, _N_CRATE_NUMBER)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing crate number"); 
		abort();
	}


	_target = _crate_num << 8 | _K_GROUP_ALL;


/* Select Target */
	_a = 0;
	_f = _K_SELECT;
	_count = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _target, 16));
	if (~_status & 1)
	{
		DevLogErr(_nid, "CAMAC command esecution error"); 
		abort();
	}

	_a = 0;
	_f = _K_READ;
	_count = 1;
	_error = word(0);
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _error, 16));
	if (~_status & 1)
	{
		DevLogErr(_nid, "CAMAC command esecution error"); 
		abort();
	}	
	if( _error < 0 )
	{
		DevLogErr(_nid, "CHVPS SELECT command execution error : "//_error);
		return (0);
	}

/* Switch off all selected channels */
    _a = _K_STATUS_BIT;
    _f = _K_WRITE;
    _w = _K_HV_OFF;
    _size = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _size, _w, 16));

	_a = 0;
	_f = _K_READ;
	_count = 1;
	_error = word(0);
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _error, 16));
	if (~_status & 1)
	{
		DevLogErr(_nid, "CAMAC command esecution error"); 
		abort();
	}	
	if( _error < 0 )
	{
		DevLogErr(_nid, "CHVPS HW OFF command execution error : "//_error);
		return (0);
	}

	return (_status);
}
