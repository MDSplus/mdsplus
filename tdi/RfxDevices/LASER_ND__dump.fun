public fun LASER_ND__dump(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

    private _K_CONG_NODES = 12;

    private _N_HEAD		= 0;
    private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _N_PORT		= 3;
    private _TRIG_SOURCE	= 4;
    private _N_NUM_PULSES	= 5;
    private _N_DELAY_PULSE	= 6;
    private _N_OSC		= 7;
    private _N_AMP		= 8;
    private _N_TOTAL		= 9;

    private _ASCII_MODE = 0;

   _error = 0;

    _ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
    if(_error)
    {
		DevLogErr(_nid, "Missing IP address"); 
		abort();
    }

    _port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
    if(_error)
    {
		DevLogErr(_nid, "Missing TCP Port");
		abort();
    }

    if( ( public _laser_nd_connected ) == 0 )
    {

		public _sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 2000, 0);
		if(public _sock == 0)
		{
			DevLogErr(_nid, "Cannot connect to remote instruments"); 
			abort();
		}
        public _laser_nd_connected = 1;
    }    


	if(TCPSendCommand(public _sock, "ND_DUMP") == 0)
	{
		DevLogErr(_nid, _msg); 
		abort();
	}


	return (1);


}















