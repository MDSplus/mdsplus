public fun CAMERA__store(as_is _nid, optional _method)
{
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

write(*, "CAMERA store");

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
		
		if(_trig_mode)
		{
			_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _error = 1);
			if(_error)
			{
				DevLogErr(_nid, "Missing trigger source");
				abort();
			}
		} else {
			_trig_time = 0;
		}

		_frame_rate = if_error(data(DevNodeRef(_nid, _N_FRAME_RATE)), _error = 1);
		if(_error)
		{
		DevLogErr(_nid, "Missing frame rate");
		abort();
		}
		
		wait(1.0);


		_sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 10000, _sw=0);
		if(_sock == 0)
		{
			DevLogErr(_nid, "Cannot connect to remote instruments");
			abort();
		}

		if((_err_msg = TCPSendCommand( _sock, "CAMERA_STORE "//_name) ) != "")
		{
			DevLogErr(_nid, "Error during send  CAMERA_STORE command "//_err_msg);
			TCPCloseConnection(_sock);  
			abort();
		}

                _img = CAMERAReadFrames(_sock, _n_frames = 0, _x_pixel = 0, _y_pixel = 0);

write(*,_n_frames);
write(*,_x_pixel);
write(*,_y_pixel);


		if( size( _img ) == 0)
		{
			DevLogErr(_nid, "No image read for CAMERA"); 
		} else {
			_trig_time = _trig_time + 1./_frame_rate;
			_dim = make_dim(make_window(0, _n_frames - 1, _trig_time), make_range(*,*,(1./_frame_rate)) );
			_frames_img = set_range(_y_pixel, _x_pixel, _n_frames, _img);
write(*,shape(_frames_img));			
			_video = compile('build_signal(($VALUE), set_range(`_y_pixel, `_x_pixel, `_n_frames, `_img), (`_dim))');
write(*,_video);			
			
			_video_nid =  DevHead(_nid) + _N_VIDEO;
			_status = TreeShr->TreePutRecord(val(_video_nid),xd(_video),val(0));
			if(! (_status & 1))
			{
				DevLogErr(_nid, 'Error writing data in pulse file');
			}
		}

		wait(1.0);

		TCPCloseConnection(_sock);

		return (1);

}















