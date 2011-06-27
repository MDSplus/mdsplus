public fun LASER_YAG__abort(as_is _nid, optional _method)
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

	/*  Get  nr pulse in burst */
    _pulse_in_burst = if_error(data(DevNodeRef(_nid, _N_PULSE_BURST)), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid nr of pulse in burst");
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


	if( _remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{
			_status = MdsValue('LASER_YAG_HWabort($,$)', _port, _pulse_in_burst);		
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

		_status = LASER_YAG_HWabort(_port, _pulse_in_burst);
		if( _status >= 0 )
		{
			DevLogErr(_nid, ""//LASER_YAG_Err( _status ));
			abort();
		}
	} 



	return (1);

}















