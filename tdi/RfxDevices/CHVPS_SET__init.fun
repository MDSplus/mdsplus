public fun CHVPS_SET__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 14;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_TARGET = 2;
    private _N_SET = 3;
    private _N_RESET_MODE = 4;
    private _N_GATE_MODE = 5;
    private _N_OVERLOAD_DET = 6;
    private _N_TRIP_TIME = 7;
    private _N_VOLTAGE = 8;
    private _N_CURRENT = 9;
    private _N_VMAX = 10;
    private _N_RAISE_RATE = 11;
    private _N_FALL_RATE = 12;

/* CHVPS node ofset position */

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


	_error = 0;

    _target     = DevNodeRef(_nid, _N_TARGET);
    _target_nid = if_error( compile(getnci(getnci(_target, 'record'), 'fullpath')), (_error = 1) );
	if(_error == 1)
	{
		DevLogErr(_nid, 'Cannot resolve CHVPS target device');
		abort();
	}

	_set = if_error(data(DevNodeRef(_nid, _N_SET)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing channel list"); 
		abort();
	}

	_reset_mode = if_error(data(DevNodeRef(_nid, _N_RESET_MODE)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing reset mode"); 
		abort();
	}

	_gate_mode = if_error(data(DevNodeRef(_nid, _N_GATE_MODE)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing gate mode"); 
		abort();
	}

	_overload_mode = if_error(data(DevNodeRef(_nid, _N_OVERLOAD_DET)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing overload detection mode"); 
		abort();
	}

	_trip_time = if_error(data(DevNodeRef(_nid, _N_TRIP_TIME)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing trip time"); 
		abort();
	}

	_current = if_error(data(DevNodeRef(_nid, _N_CURRENT)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing current"); 
		abort();
	}

	_voltage = if_error(data(DevNodeRef(_nid, _N_VOLTAGE)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing vlotage"); 
		abort();
	}

	_volt_max = if_error(data(DevNodeRef(_nid, _N_VMAX)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing max voltage"); 
		abort();
	}

	_raise_rate = if_error(data(DevNodeRef(_nid, _N_RAISE_RATE)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing raise rate"); 
		abort();
	}


	_fall_rate = if_error(data(DevNodeRef(_nid, _N_FALL_RATE)), _error = 1);
	if(_error)
	{
		DevLogErr(_nid, "Missing fall rate"); 
		abort();
	}


	write(*, "Target : ", _target_nid);
	write(*, "Set Number : ", _set);
	write(*, "Reset Mode : ", _reset_mode);
	write(*, "Gate Mode : ", _gate_mode);
	write(*, "Overload Detection Mode : ", _overload_mode);
	write(*, "Trip Time : ", _trip_time);
	write(*, "Current : ", _current);
	write(*, "Voltage : ", _voltage);
	write(*, "Voltage Max : ", _volt_max);
	write(*, "Raise Rate : ", _raise_rate);
	write(*, "Fall Rate : ", _fall_rate);



	if(_overload_mode == "Enable")
	{
		if (_trip_time < 0 || _trip_time > 9998) 
		{
			DevLogErr(_nid, "Trip time value out of range 0 - 9998 set to 0"); 
			_trip_time = 0;
		} 

	}
	else
	{
		_trip_time = 9999;
	}


	if(_gate_mode == "External")
	{
		if(size(_voltage) > 1)
		{
			_v0 = _voltage[0];
			_v1 = _voltage[1];
		} else {
			DevLogErr(_nid, "If gate mode is set as External; Voltge must be an array of two elements [V0, V1] : set V1 = 0"); 
			_v0 = _voltage;
			_v1 = 0;
		}
	}
	else
	{
		if(size(_voltage) > 1 )
		{
			DevLogErr(_nid, "If gate mode is set as None; Voltge must be a scalar : set V1 = V0"); 
			_v0 = _voltage[0];
		} else {
			_v0 = _voltage;
		}
		_v1 = _v0;
	}

	if( abs(_v0) > abs(_volt_max) )
	{
		_v0 = (_v0 < 0 ? -1 : 1)  * abs(_volt_max); 
		DevLogErr(_nid, "V0 value exceed max voltage limit V0 is set equal to "//_v0); 
	}

	if( abs(_v1) > abs(_volt_max) )
	{
		_v1 = (_v1 < 0 ? -1 : 1)  * abs(_volt_max); 
		DevLogErr(_nid, "V1 value exceed max voltage limit V1 is set equal to "//_v1); 
	}


	if(_gate_mode == "External")
	{
		if(size(_current) > 1)
		{
			_i0 = _current[0];
			_i1 = _current[1];
		} else {
			DevLogErr(_nid, "If gate mode is set as External; Current must be an array of two elements [I0, I1] : set I1 = 0"); 
			_i0 = _current;
			_i1 = 0;
		}
	}
	else
	{
		if(size(_current) > 1 )
		{
			DevLogErr(_nid, "If gate mode is set as None; Current must be a scalar : set I1 = I0"); 
			_i0 = _current[0];
		} else {
			_i0 = _current;
		}
		_i1 = _i0;
	}


	_head_channel = _N_SET_00 + (_set *  _K_NODES_PER_SET);


	_set_type  = if_error( data(DevNodeRef(_target_nid, _head_channel + _N_SET_CHAN_TYPE )), ( _error = 1; _param = "Chan type"; ) );
	_chan_addr = if_error(data(DevNodeRef(_target_nid, _head_channel + _N_SET_ADDRESS )), ( _error = 1; _param = "Chan address"; ) );


	write(*, "Set type : ", _set_type);
	write(*, "Chan address : ", int(_chan_addr) );

    _status = DevPut(_target_nid, _head_channel + _N_SET_I0, _i0);
    _status = DevPut(_target_nid, _head_channel + _N_SET_I1, _i1);
    _status = DevPut(_target_nid, _head_channel + _N_SET_RAMP_DOWN, _fall_rate);
    _status = DevPut(_target_nid, _head_channel + _N_SET_RAMP_UP, _raise_rate);
    _status = DevPut(_target_nid, _head_channel + _N_SET_RESET, _reset_mode);
    _status = DevPut(_target_nid, _head_channel + _N_SET_TRIP, _trip_time);
    _status = DevPut(_target_nid, _head_channel + _N_SET_V0, _v0);
    _status = DevPut(_target_nid, _head_channel + _N_SET_V1, _v1);


	return (1);
}
