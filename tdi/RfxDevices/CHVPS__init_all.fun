public fun CHVPS__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 487;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_NAME = 2;
    private _N_CRATE_NUMBER = 3;

    private _K_NUM_SET = 40;
    private _K_NODES_PER_SET = 11;
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
				             [       0, 6000, 1000, 1, 1, 500 ] ];

	private _wait_val = 0.01;

	private fun readAndResetGroupB(in _name, in _nid, in _crate_num, inout _old_group_B) 
	{

		_target = (_crate_num << 8) | _K_GROUP_B;   
			
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


	/* Read channels defined into group B */

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

 		_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _old_group_B, 16));
		if (~_status & 1) return (0);
		
		wait(_wait_val);


	/* Delete channels define into B group  */

		for (_i = 0, _target -= _K_GROUP_B; _i < 40; _i++, _target++)
		{
			if(_old_group_B[_i / 16] & 1 << (_i % 16))
			{
				_status = camacCommand(_name, 0, _K_SELECT, _target, _error);      
				if (_status & 1 && !(_error < 0)) 
					_status = camacCommand(_name, _K_GROUP_BIT, _K_WRITE, 1, _error);
			}    
			if (~_status & 1 || _error < 0) 
			{
				if(_error < 0) 
					DevLogErr(_nid, "CHVPS command execution error : "//_error);
				return (0);
			}
		}

		return(1);    
	};


	private fun camacCommand(in _name, in _a, in _f, in _w, inout _error) 
    {

		_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  1, _w, 16));      
		wait(_wait_val);
		if (_status & 1) 
			_status DevCamChk(_name, CamQrepw( _name, 0, _K_READ,  1, _error, 16));      
		wait(_wait_val);

		return (_status);
	};
 
    _name = if_error(data(DevNodeRef(_nid, _N_NAME)),(DevLogErr(_nid, "Missing camac name"); abort();));

    _crate_num = if_error(data(DevNodeRef(_nid, _N_CRATE_NUMBER)),(DevLogErr(_nid, "Missing crate name"); abort();));

	_old_group_B = zero(0, 0UW);
	_new_group_B = zero(0, 0UW);
	_chan_bit    = zero(0, 0UB);


	if( ! readAndResetGroupB(_name, _nid, _crate_num, _old_group_B) )
	{
		abort(0);
	}

    _target = _crate_num << 8;


    for(_i = 0; _i < _K_NUM_SET; _i++)
    {
		_error = 0;
		_head_channel = _N_SET_00 + (_i *  _K_NODES_PER_SET);

        if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
        { 
/* Lettura della configurazione dal pulse file */

			_set_type = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_CHAN_TYPE )), (_error = 1; _para = "Chan type") );

			_chan_addr = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_ADDRESS )), ( _error = 1; _para = "Chan address");

			_i0 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_I0 )), ( _error = 1; _para = "Current 0" ) );

			_i1 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_I1 )), ( _error = 1; _para = "Current 1" ) );
			
			_ramp_down = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_RAMP_DOWN )), ( _error = 1; _para = "Ramp down" ) );

			_ramp_up = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_RAMP_UP )), ( _error = 1; _para = "Ramp up" ) );

			DevNodeCvt(_nid, _head_channel + _N_SET_RESET, ['DISABLE', 'ENABLE'], [0,1], _reset_flag = 0);

			_trip = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_TRIP )), ( _error = 1; _para = "Trip" ) );

			_v0 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_V0 )), ( _error = 1; _para = "Voltage 0" ) );

			_v1 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_V1 )), ( _error = 1; _para = "Voltage 1" ) );

/* Controllo della configurazione */

			if( ! _error)
			{
				_chan_num = size(_chan_addr);

				if(_chan_num == 0)
				{
				   DevLogErr(_nid, "CHVPS set"//_i//" do not define channel into set");
				   _error = 1;
				}

				if(_set_type == "CA334")
				{
					_chan_type_code = _K_CA334_TYPE;
					_chan_type_idx = 0;

				} else {
				if(_set_type == "CA334")
				{
					_chan_type_code = _K_CA334_TYPE;
					_chan_type_idx = 1;
				} else {
					DevLogErr(_nid, "CHVPS set"//_i//" channel type "//chan_type//" not sypported");
					_error = 1;
				}

/* Controllo dei parametri di configurazione */				
			
				if( ! _error)
				{
					_v0_code = abs( word( _v0 * _data_sheet_t[_K_VOLT_FACTOR, chan_type_idx]));

					if (_v0_code < _data_sheet_t[_K_MIN_VOLTAGE, chan_type_idx] || 
						_v0_code > _data_sheet_t[_K_MAX_VOLTAGE, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" Voltage 0 out of range");
						_error = 1;
					}


					_v1_code = abs( word( _v1 * _data_sheet_t[_K_VOLT_FACTOR, chan_type_idx]));

					if (_v1_code < _data_sheet_t[_K_MIN_VOLTAGE, chan_type_idx] || 
						_v1_code > _data_sheet_t[_K_MAX_VOLTAGE, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" Voltage 1 out of range");
						_error = 1;
					}

					_i0_code = ( word( _i0 * _data_sheet_t[_K_CURR_FACTOR, chan_type_idx]) );

					if (_i0_code < 0|| 
						_i0_code > _data_sheet_t[_K_MAX_CURRENT, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" Current 0 out of range");
						_error = 1;
					}

					_i1_code = ( word( _i1 * _data_sheet_t[_K_CURR_FACTOR, chan_type_idx]) );

					if (_i1_code < 0|| 
						_i1_code > _data_sheet_t[_K_MAX_CURRENT, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" Current 1 out of range");
						_error = 1;
					}

					_rup_code = ( word( _ramp_up * _data_sheet_t[_K_VOLT_FACTOR, chan_type_idx]));

					if (_rup_code <= 0|| 
						_rup_code > _data_sheet_t[_K_MAX_RATE, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" ramp up out of range");
						_error = 1;
					}

					_rdown_code = ( word( _ramp_down * _data_sheet_t[_K_VOLT_FACTOR, chan_type_idx]));

					if (_rdown_code <= 0|| 
						_rdown_code > _data_sheet_t[_K_MAX_RATE, chan_type_idx])
					{ 
						DevLogErr(_nid, "CHVPS set"//_i//" ramp down out of range");
						_error = 1;
					}

					if (_trip < 0 || _trip > 9999) 
					{
						DevLogErr(_nid, "CHVPS set"//_i//" trip time out of range");
						_error = 1;
					}

/* Impostazione del dispositivo */

					if(! _error )
					{
						for (_j = 0; _j < _chan_num && status & 1; _j++)
						{
							if ((_chan_bit[_chan_addr[_j] / 8]) & (1 << (_chan_addr[_j] % 8)))
							{
								DevLogErr(_nid, "CHVPS set"//_i//" over write channel configuration "//__chan_addr[_j]);
							}
							
							_target += _chan_addr[_j];
							_chan_bit[_chan_addr[_j] / 8] |= 1 << (_chan_addr[_j] % 8);

							_status = camacCommand(_name, 0, _K_SELECT, _target, _error); 

							wait(_wait_val);
							
							if (_status & 1 && !(_error < 0))
							{
								_status = camacCommand(_name, _K_CHAN_TYPE, _K_SELECT, _K_START_READOUT, _chan_type);
							}     
							 
							if (_status & 1 && !(_error < 0))
							{
								if ((_chan_type_code != _chan_type)
								{
									DevLogErr(_nid, "CHVPS set"//_i//" invalid channel type hardware configuration");
									_status = 0;
								}
							}

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_V0 , _K_WRITE, _v0_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_V1 , _K_WRITE, _v1_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_I0 , _K_WRITE, _i0_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_I1 , _K_WRITE, _i1_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_I1 , _K_RUMP_UP, _rup_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_I1 , _K_RUMP_DOWN, _rdown_code, _error);

							if (_status & 1 && !(_error < 0))
								_status = camacCommand(_name, _K_I1 , _K_TRIP, _trip, _error);

							if (_status & 1 && !(_error < 0))
							{
								if(_reset_flag)
									_status = camacCommand(_name, _K_GROUP_BIT, _K_WRITE, _K_BIT_GROUP_A, _error);
								else {
									_new_group_B[_chan_addr[_j] / 16] |= 1 << (_chan_addr[_j] % 16);
									_status = camacCommand(_name, _K_GROUP_BIT, _K_WRITE, _K_BIT_GROUP_B, _error);
								}
							}
							
							if( _error < 0 )
									DevLogErr(_nid, "CHVPS command execution error : "//_error);

							_target -= _chan_addr[_j];						 
						}
					}
				} 
				else
				{
					DevLogErr(_nid, "CHVPS set"//_i//" configuration parameter "//_param//" error");
				}
			}
		}
	}

/* Turn off all channels defined in old_group_B and not in new_group_B */

	for ( _chan = 0; _chan < 40; _chan++) 
	{
		if ( ((_old_group_B[_chan / 16]) & (1 << (_chan % 16))) && !((_new_group_B[_chan / 16]) & (1 << (_chan % 16))) )
		{
			_target += _chan;

			_status = camacCommand(_name, 0, _K_SELECT, _target, _error);      
			if (_status & 1 && !(_error < 0))
				 _status = camacCommand(_name, _K_STATUS_BIT, _K_WRITE, _K_HV_OFF, _error);
			if (~_status & 1 || _error < 0) 
				DevLogErr(_nid, "CHVPS cannot disable channel "//_chan);

			target -= chan;
		}
	}

    return (_status);
}
