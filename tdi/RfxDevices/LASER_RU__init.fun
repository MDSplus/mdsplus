public fun LASER_RU__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 62;

	private _N_COMMENT = 1;
	
	private _N_SW_MODE = 2;
	private _N_IP_ADDR = 3;
	
    private _N_RS232_PORT = 4;
    private _N_TRIG_MODE = 5;
    private _N_NUM_SHOTS = 6;
    private _N_ENERGY = 7;
    private _N_VOLTAGE_OSC = 8;
    private _N_VOLTAGE_A_1 = 9;
    private _N_VOLTAGE_A_2 = 10;
    private _N_VOLTAGE_A_3 = 11;

    private _N_BALANCE_P_1 = 12;
    private _N_BALANCE_P_2 = 13;
    private _N_BALANCE_P_3 = 14;

    private _N_DELAY_AMP = 15;
    private _N_DELAY_PULS_1 = 16;
    private _N_DELAY_PULS_2 = 17;
    private _N_DELAY_PULS_3 = 18;
    private _N_DELAY_PULS_4 = 19;
    private _N_DELAY_FIRE = 20;

    private _N_CALIBRATION = 21;
    private _N_CAL_KOSC = 22;
    private _N_CAL_KAMP_1 = 23;
    private _N_CAL_KAMP_2 = 24;
    private _N_CAL_KAMP_3 = 25;
    private _N_CAL_KBAL_1 = 26;
    private _N_CAL_KBAL_2 = 27;
    private _N_CAL_KBAL_3 = 28;

    private _N_OSC_PAR = 29;
	private _N_AMP_PAR_1 = 30;
    private _N_AMP_PAR_2 = 31;
    private _N_AMP_PAR_3 = 32;
    private _N_PBAL_PAR_1 = 33;
    private _N_PBAL_PAR_2 = 34;
    private _N_PBAL_PAR_3 = 35;

    private _N_DTIME1_PAR_1 = 36;
    private _N_DTIME1_PAR_2 = 37;
    private _N_DTIME1_PAR_3 = 38;
    private _N_DTIME1_PAR_4 = 39;
    private _N_DTIME1_PAR_5 = 40;
    private _N_DF1M_PAR_1 = 41;
    private _N_DF1M_PAR_2 = 42;

    private _K_NUM_BITS = 5;
	private _K_NUM_DATA = 8;
    private _K_NODES_PER_BIT = 2;
    private _N_BIT_0= 44;
    private _N_BIT_PRE_SHOT = 1;
    private _N_BIT_POST_SHOT = 1;

	private _N_DATA = 59;
    

	_error = 0;
  
    _port = if_error(data(DevNodeRef(_nid, _N_RS232_PORT)), _error = 1);
    if( _error )
    {
		DevLogErr(_nid, "Missing RS232 Port"); 
		abort();
	}
	
	DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
	}



 	/*  Decode trigger mode parameter */

	DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL',  'EXTERNAL',  'AUTOMATIC', 'MANUAL'], [0, 1, 2, 3], _trig_mode = 0);



	/*  Get OSC parameter */

   _osc_par = if_error(data(DevNodeRef(_nid, _N_OSC_PAR)), _error = 1);
	if( _error || _osc_par > 4095)
	{
	        DevLogErr(_nid, "Invalid voltage oscillator value");
			abort();
  	}


    /*  Get amplifier parameter 1 2 3 */

    _amp_par = [];

    _amp_par = [_amp_par, if_error(data(DevNodeRef(_nid, _N_AMP_PAR_1)), _error = 1)];
	if( _error || _amp_par[0] > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 1 value");
			abort();
  	}

    _amp_par = [_amp_par, if_error(data(DevNodeRef(_nid, _N_AMP_PAR_2)), _error = 1)];
	if( _error || _amp_par[1] > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 2 value");
			abort();
  	}

    _amp_par = [_amp_par, if_error(data(DevNodeRef(_nid, _N_AMP_PAR_3)), _error = 1)];
	if( _error || _amp_par[2] > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 3 value");
			abort();
  	}


	/*  Get pbal  parameter 1 2 3 */

	_pbal_par = [];

    _pbal_par = [_pbal_par, if_error(data(DevNodeRef(_nid, _N_PBAL_PAR_1)), _error = 1)];
	if( _error || _pbal_par[0] > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 1 value");
			abort();
  	}


    _pbal_par = [_pbal_par, if_error(data(DevNodeRef(_nid, _N_PBAL_PAR_2)), _error = 1)];
	if( _error || _pbal_par[1] > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 2 value");
			abort();
  	}

    _pbal_par = [_pbal_par, if_error(data(DevNodeRef(_nid, _N_PBAL_PAR_3)), _error = 1)];
	if( _error || _pbal_par[2] > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 3 value");
			abort();
  	}

	/*  Get delay time parameter 1 2 3 4 5 */

	_dtime1_par = [];


    _dtime1_par = [ _dtime1_par, if_error(DevNodeRef(_nid, _N_DTIME1_PAR_1), _error = 1)];
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 1 value");
			abort();
  	}


    _dtime1_par = [ _dtime1_par, if_error(DevNodeRef(_nid, _N_DTIME1_PAR_2), _error = 1)];
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 2 value");
			abort();
  	}


    _dtime1_par = [ _dtime1_par, if_error(DevNodeRef(_nid, _N_DTIME1_PAR_3), _error = 1)];
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 3 value");
			abort();
  	}


    _dtime1_par = [ _dtime1_par, if_error(DevNodeRef(_nid, _N_DTIME1_PAR_4), _error = 1)];
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 4 value");
			abort();
  	}


    _dtime1_par = [ _dtime1_par, if_error(DevNodeRef(_nid, _N_DTIME1_PAR_5), _error = 1)];
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 5 value");
			abort();
  	}


	/* Get delay fire */

    _delay_fire = if_error(data(DevNodeRef(_nid, _N_DELAY_FIRE)), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay fire value");
			abort();
  	}


	_df1m_par = [];

	/*  Get delay DF1M parameter 1 2 */
	if(_delay_fire)
	{
		_df1m_par = [ _df1m_par, if_error(DevNodeRef(_nid, _N_DF1M_PAR_1), _error = 1)];
		if( _error )
		{
				DevLogErr(_nid, "Invalid df1m parameter 1 value");
				abort();
  		}

		_df1m_par = [ _df1m_par, if_error(DevNodeRef(_nid, _N_DF1M_PAR_2), _error = 1) ];
		if( _error )
		{
				DevLogErr(_nid, "Invalid df1m parameter 2 value");
				abort();
  		}
	}
	else
	{
		_df1m_par = [0, 0];
	}

	/* Get status digital output lines*/

    _bit_states = [];
    for(_i = 0; _i < _K_NUM_BITS; _i++)
    {
 		_bit_state = if_error(data(DevNodeRef(_nid, _N_BIT_0  + ( _i *  _K_NODES_PER_BIT) +  _N_BIT_PRE_SHOT)), _bit_state = 0);
		_bit_states = [_bit_states, _bit_state];
    }



	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{
			write(*,  _port,  _trig_mode,  _osc_par,  _amp_par, _pbal_par, _dtime1_par, _delay_fire, _df1m_par,  _bit_states);
		
			_status = MdsValue('LASER_RU_HWinit($,$,$,$,$,$,$,$,$)',  _port,  _trig_mode,  _osc_par,  _amp_par, _pbal_par, _dtime1_par, _delay_fire, _df1m_par,  _bit_states);		
			MdsDisconnect();
			if(_status == 0)
			{
				DevLogErr(_nid, "Error Initializing ruby Laser");
				abort();
			}
		}
		else
		{
			DevLogErr(_nid, "Cannot connect to mdsip server "//_ip_addr);
			abort();
		}
	}
	else
	{
		write(*,  _port,  _trig_mode,  _osc_par,  _amp_par, _pbal_par, _dtime1_par, _delay_fire, _df1m_par,  _bit_states);

		_status = LASER_RU_HWinit(_port, _trig_mode, _osc_par, _amp_par,  _pbal_par, _dtime1_par, _delay_fire, _df1m_par, _bit_states);
		if(_status == 0)
			abort();
	} 



	return (1);

}















