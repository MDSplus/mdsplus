public fun CAMERA__init(as_is _nid, optional _method)
{
	private _K_CONG_NODES = 22;

    private _N_COMMENT = 1;
    private _N_NAME = 2;
    private _N_IP_ADDRESS = 3;
    private _N_PORT = 4;
    private _N_TRIG_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_NUM_FRAMES = 7;
    private _N_MODEL = 8;
    private _N_LENS_TYPE = 9;
    private _N_APERTURE = 10;
    private _N_F_DISTANCE = 11;
    private _N_FILTER = 12;
    private _N_SHUTTER = 13;
    private _N_TOR_POSITION = 14;
    private _N_POL_POSITION = 15;
    private _N_TARGET_ZONE = 16;
    private _N_PIXEL_FRAME = 17;
    private _N_FRAME_RATE = 18;
    private _N_VIDEO = 19;

	private _ASCII_MODE = 0;

write(*, "CAMERA init");

   _error = 0;

	_name = if_error(data(DevNodeRef(_nid, _N_NAME)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing camera name"); 
	abort();
	}


	_ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing IP address"); 
	abort();
	}

write(*, _ip);


	_port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing TCP Port");
	abort();
	}

write(*, _port);

	_n_frames = if_error(data(DevNodeRef(_nid, _N_NUM_FRAMES)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing number of frames");
	abort();
	}

write(*, _n_frames);

    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _trig_mode = 0);


write(*, "Open Connection");

	wait(1.0);


	_sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 4000, _sw=0);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

write(*, "Send init");


	if((_err_msg = TCPSendCommand(_sock, "CAMERA_INIT "//_name//" "//trim(adjustl(_n_frames))) ) != "")
	{
		DevLogErr(_nid, "CAMERA INIT operation error : "//_err_msg);
		TCPCloseConnection(_sock); 
		abort();
	}

	wait(1.0);

	TCPCloseConnection(_sock);



	return (1);

}
