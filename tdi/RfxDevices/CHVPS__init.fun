public fun CHVPS__init(as_is _nid, optional _method)
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

	_old_group_B = zero(3, 0UW);
	_new_group_B = zero(3, 0UW);
	_chan_bit    = zero(5, 0UB);


	if( ! CHVPSResetCommand(_name, _nid, _crate_num, _old_group_B) )
	{
		abort();
	}

	_target = _crate_num << 8;
	_chan_set_error = 0;

	for(_i = 0; _i < _K_NUM_SET; _i++)
	{

		_error = 0;
		_head_channel = _N_SET_00 + (_i *  _K_NODES_PER_SET);
		
		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_set_type = if_error( data(DevNodeRef(_nid, _head_channel + _N_SET_CHAN_TYPE )), ( _error = 1; _param = "Chan type"; ) );

			_chan_addr = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_ADDRESS )), ( _error = 1; _param = "Chan address"; ) );

			_i0 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_I0 )), ( _error = 1; _param = "Current 0"; ) );

			_i1 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_I1 )), ( _error = 1; _param = "Current 1"; ) );
			
			_ramp_down = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_RAMP_DOWN )), ( _error = 1; _param = "Ramp down"; ) );

			_ramp_up = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_RAMP_UP )), ( _error = 1; _param = "Ramp up"; ) );

			if_error( DevNodeCvt(_nid, _head_channel + _N_SET_RESET, ['DISABLE', 'ENABLE'], [0,1], _reset_flag = 0), ( _error = 1; _param = "Reset"; ) );

			_trip = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_TRIP )), ( _error = 1; _param = "Trip"; ) );

			_v0 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_V0 )), ( _error = 1; _param = "Voltage 0"; ) );

			_v1 = if_error(data(DevNodeRef(_nid, _head_channel + _N_SET_V1 )), ( _error = 1; _param = "Voltage 1"; ) );


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
					if(_set_type == "CA332")
					{
						_chan_type_code = _K_CA332_TYPE;
						_chan_type_idx = 1;
					} else {
						DevLogErr(_nid, "CHVPS set"//_i//" channel type "//_chan_type//" not sypported");
						_error = 1;
					}
				}
			}


			if( ! _error)
			{

				_v0_code = abs( word( _v0 * _data_sheet_t[_K_VOLT_FACTOR, _chan_type_idx]));
				if (_v0 < _data_sheet_t[_K_MIN_VOLTAGE, _chan_type_idx] || 
					_v0 > _data_sheet_t[_K_MAX_VOLTAGE, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" Voltage 0 out of range");
					_error = 1;
				}

				_v1_code = abs( word( _v1 * _data_sheet_t[_K_VOLT_FACTOR, _chan_type_idx]));

				if ( _v1 < _data_sheet_t[_K_MIN_VOLTAGE, _chan_type_idx] || 
					 _v1 > _data_sheet_t[_K_MAX_VOLTAGE, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" Voltage 1 out of range");
					_error = 1;
				}


				_i0_code = ( word( _i0 * _data_sheet_t[_K_CURR_FACTOR, _chan_type_idx]) );

				if ( _i0 < 0|| 
					 _i0 > _data_sheet_t[_K_MAX_CURRENT, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" Current 0 out of range");
					_error = 1;
				}


				_i1_code = ( word( _i1 * _data_sheet_t[_K_CURR_FACTOR, _chan_type_idx]) );

				if ( _i1 < 0|| 
					 _i1 > _data_sheet_t[_K_MAX_CURRENT, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" Current 1 out of range");
					_error = 1;
				}


				_rup_code = ( word( _ramp_up * _data_sheet_t[_K_VOLT_FACTOR, _chan_type_idx]));

				if ( _ramp_up <= 0|| 
					 _ramp_up > _data_sheet_t[_K_MAX_RATE, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" ramp up out of range");
					_error = 1;
				}


				_rdown_code = ( word( _ramp_down * _data_sheet_t[_K_VOLT_FACTOR, _chan_type_idx]));

				if ( _ramp_down <= 0|| 
					 _ramp_down > _data_sheet_t[_K_MAX_RATE, _chan_type_idx])
				{ 
					DevLogErr(_nid, "CHVPS set"//_i//" ramp down out of range");
					_error = 1;
				}


				if (_trip < 0 || _trip > 9999) 
				{
					DevLogErr(_nid, "CHVPS set"//_i//" trip time out of range");
					_error = 1;
				}

				if(! _error )
				{
					_status = 1;

					for (_j = 0; _j < _chan_num && _status & 1; _j++)
					{

						if ((_chan_bit[_chan_addr[_j] / 8]) & (1 << mod(_chan_addr[_j],8)))
						{
							DevLogErr(_nid, "CHVPS set"//_i//" over write channel configuration "//_chan_addr[_j]);
						}
					
						_target += _chan_addr[_j];

						_chan_bit_new = [];

						for( _k = 0; _k < 5; _k++)
						{
							if( ( _chan_addr[_j] / 8) == _k)
							{
								_chan_bit_new = [ _chan_bit_new,  _chan_bit[_k] | ( 1 << mod(_chan_addr[_j], 8) ) ];
							}
							else
							{
								_chan_bit_new = [_chan_bit_new, _chan_bit[_k] ];
							}
						}

						_chan_bit = _chan_bit_new;

						_chan_type = 0;
						_status = CHVPSSendCommand(_name, 0, _K_SELECT, _target, _error); 

	
						wait(_wait_val);

						
						if (_status & 1 && !(_error < 0))
						{
							
							_status = CHVPSSendCommand(_name, _K_CHAN_TYPE, _K_SELECT, _K_START_READOUT, _chan_type);
						}     

						
						 
						if (_status & 1 && !(_error < 0))
						{
							if ( _chan_type_code != _chan_type)
							{
								DevLogErr(_nid, "CHVPS set"//_i//" invalid channel type hardware configuration");
								_status = 0;
							}
						}

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_V0 , _K_WRITE, _v0_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_V1 , _K_WRITE, _v1_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_I0 , _K_WRITE, _i0_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_I1 , _K_WRITE, _i1_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_RAMP_UP , _K_WRITE, _rup_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_RAMP_DOWN , _K_WRITE, _rdown_code, _error);

						if (_status & 1 && !(_error < 0))
							_status = CHVPSSendCommand(_name, _K_TRIP , _K_WRITE, _trip, _error);

						if (_status & 1 && !(_error < 0))
						{
							if(_reset_flag)
							{
								_status = CHVPSSendCommand(_name, _K_GROUP_BIT, _K_WRITE, _K_BIT_GROUP_A, _error);
							}
							else 
							{

								_new_group_B_new = [];

								for( _k = 0; _k < 3; _k++)
								{
									if( ( _chan_addr[_j] / 16) == _k)
									{
										_new_group_B_new = [ _new_group_B_new,  _new_group_B[_k] | ( 1 << mod(_chan_addr[_j], 16) ) ];
									}
									else
									{
										_new_group_B_new = [_new_group_B_new, _new_group_B[_k] ];
									}
								}
								_new_group_B = _new_group_B_new;

								_status = CHVPSSendCommand(_name, _K_GROUP_BIT, _K_WRITE, _K_BIT_GROUP_B, _error);
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
				_chan_set_error = 1;
			}		
		}
	}
	
	if(_chan_set_error == 1)
	{
		DevLogErr(_nid, "CHVPS channel set configuration error");
		abort();
	}


	for ( _chan = 0; _chan < 40; _chan++) 
	{
		_in_old_B =  (_old_group_B[_chan / 16]) & (1 << mod(_chan, 16) );
		_in_new_B =  (_new_group_B[_chan / 16]) & (1 << mod(_chan, 16) );

		if ( _in_old_B && !_in_new_B  ) 
		{
			_target += _chan;

			_status = CHVPSSendCommand(_name, 0, _K_SELECT, _target, _error);      
			if (_status & 1 && !(_error < 0))
				 _status = CHVPSSendCommand(_name, _K_STATUS_BIT, _K_WRITE, _K_HV_OFF, _error);

			if (~_status & 1 || _error < 0) 
				DevLogErr( _nid, "CHVPS cannot disable channel "//_chan);

			_target -= _chan;
		}	
	}

	return (_status);
}
