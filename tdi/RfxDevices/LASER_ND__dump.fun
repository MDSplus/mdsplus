public fun LASER_ND__dump(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 12;

	private _N_HEAD			= 0;
	private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _TRIG_SOURCE	= 3;
    private _N_PORT			= 4;
    private _N_NUM_PULSES	= 5;
    private _N_DELAY_PULSE	= 6;
    private _N_OSC			= 7;
    private _N_AMP			= 8;
    private _N_TOTAL		= 9;

	private _ASCII_MODE = 0

    _ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)),(DevLogErr(_nid, "Missing IP address"); abort();));

    _port = if_error(data(DevNodeRef(_nid, _N_PORT)),(DevLogErr(_nid, "Missing TCP Port"); abort();));

	_sock = OpenConnection(_ip, _port, _ASCII_MODE);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	if(SendCommand(_sock, "ND_DUMP") == 0)
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error during send ND_DUMP command "); 
		abort();
	}

	if((_msg = CheckAnswer(_sock) )!= "")
	{
		CloseConnection(_sock);
		DevLogErr(_nid, "Error in ND_DUMP command execution : "//_msg); 
		abort();
	}

	CloseConnection(_sock);

	return (1);


}















