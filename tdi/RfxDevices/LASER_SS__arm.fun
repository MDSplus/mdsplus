public fun LASER_SS__arm(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 10;

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


	if( _remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		_status = execute(_cmd);
		if( _status != 0 )
		{

			_status = MdsValue('LASER_SS_HWarm()');		
			MdsDisconnect();
			if(_status == 0)
			{
				DevLogErr(_nid, "Error arming Solid State Laser");
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

		_status = LASER_SS_HWarm();
		if(_status == 0)
			abort();
	} 



	return (1);

}















