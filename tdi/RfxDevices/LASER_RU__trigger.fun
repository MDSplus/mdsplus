public fun LASER_RU__trigger(as_is _nid, optional _method)
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
    
	write(*, "LASER_RU__trigger");

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

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{
			_status = MdsValue('LASER_RU_HWtrigger($,$)',  _port,  _trig_mode);		
			MdsDisconnect();
			if(_status == 0)
			{
				DevLogErr(_nid, "Error trigger ruby Laser");
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
		_status = LASER_RU_HWtrigger(_port, _trig_mode);
		if(_status == 0)
			abort();
	} 


	return (1);

}















