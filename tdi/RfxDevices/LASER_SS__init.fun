public fun LASER_SS__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

	private _N_COMMENT = 1;
	private _N_SW_MODE = 2;
	private _N_IP_ADDR = 3;

    private _ENERGY = 4;
    private _NUM_POWER_SUPPLY = 5;
    private _REPETITION_RATE = 6;
    private _SYNC_DELAY_LAMP = 7;
    private _SYNC_DELAY_DIODE = 8;

	_error = 0;
	
	
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


 
	/*  Get number of power supply  parameter */

    _num_power_supply = if_error(data(DevNodeRef(_nid, _NUM_POWER_SUPPLY)), _error = 1);
	if( _error ||  _num_power_supply < 1 ||_num_power_supply > 4)
	{
	        DevLogErr(_nid, "Invalid number of power supply");
			abort();
  	}

   /*  Get energy value */

    _energy = if_error(data(DevNodeRef(_nid, _ENERGY)), _error = 1);
	if( _error  ||  _energy < 2  ||  _energy > 50)
	{
	        DevLogErr(_nid, "Invalid Energy value");
			abort();
  	}


    /*  Get repetition rate value */

    _repetition_rate = if_error(data(DevNodeRef(_nid, _REPETITION_RATE)), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid repetition rate parameter");
			abort();
  	}

   	
    /*  Get sync pulse delay lamp parameter */

    _sync_delay_lamp = if_error(data(DevNodeRef(_nid, _SYNC_DELAY_LAMP)), _error = 1);
	if( _error || _sync_delay_lamp < 10  ||  _sync_delay_lamp > 1500 )
	{
	        DevLogErr(_nid, "Invalid sync pulse delay for flash lamp");
			abort();
  	}


    /*  Get sync pulse delay diode parameter */

    _sync_delay_diode = if_error(data(DevNodeRef(_nid, _SYNC_DELAY_DIODE)), _error = 1);
	if( _error || _sync_delay_diode < 10  ||  _sync_delay_diode > 1500)
	{
	        DevLogErr(_nid, "Invalid sync pulse delay for diode laser ");
			abort();
  	}

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{
			write(*, _num_power_supply, _energy, _repetition_rate, _sync_delay_lamp, _sync_delay_diode);

			_status = MdsValue('LASER_SS_HWinit($,$,$,$,$)',  _num_power_supply, _energy, _repetition_rate, _sync_delay_lamp, _sync_delay_diode);		
			MdsDisconnect();
			if(_status == 0)
			{
				DevLogErr(_nid, "Error Initializing Solid State Laser");
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
	write(*, _num_power_supply, _energy, _repetition_rate, _sync_delay_lamp, _sync_delay_diode);

	_status = LASER_SS_HWinit( _num_power_supply, _energy, _repetition_rate, _sync_delay_lamp, _sync_delay_diode);
		if(_status == 0)
			abort();
	} 



	return (1);

}















