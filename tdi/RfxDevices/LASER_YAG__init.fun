public fun LASER_YAG__init(as_is _nid, optional _method)
{
   private _K_CONG_NODES = 14;

	private _N_COMMENT = 1;
	private _N_SW_MODE = 2;
	private _N_IP_ADDR = 3;

    private _ENERGY = 4;
    private _REPETITION_RATE = 5;
    private _N_PULSE_BURST = 6;
    private _SYNC_DELAY_LAMP = 7;
    private _SYNC_DELAY_DIODE = 8;
    private _WAIT_SIMMER_ON = 9;

    private _N_RS232_PORT = 13;

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


   /*  Get energy value */

    _energy = if_error(data(DevNodeRef(_nid, _ENERGY)), _error = 1);
	if( _error  ||  _energy < 20  ||  _energy > 150)
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

	/*  Get  nr pulse in burst */

    _pulse_in_burst = if_error(data(DevNodeRef(_nid, _N_PULSE_BURST)), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid nr of pulse in burst");
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
			write(*, _port, _energy, _repetition_rate, _pulse_in_burst, _sync_delay_lamp, _sync_delay_diode);

			_status = MdsValue('LASER_YAG_HWinit($,$,$,$,$,$)', _port, _energy, _repetition_rate, _pulse_in_burst, _sync_delay_lamp, _sync_delay_diode);		
			MdsDisconnect();
			if( _status >= 0 )
			{
				DevLogErr(_nid, ""//LASER_YAG_Err( _status ));
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
		write(*, _port, _energy, _repetition_rate, _pulse_in_burst, _sync_delay_lamp, _sync_delay_diode);

		_status = LASER_YAG_HWinit(_port, _energy, _repetition_rate, _pulse_in_burst,  _sync_delay_lamp, _sync_delay_diode);
		if( _status >= 0 )
		{
			DevLogErr(_nid, ""//LASER_YAG_Err( _status ));
			abort();
		}
	} 
	
	return (1);

}















