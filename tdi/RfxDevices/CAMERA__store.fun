public fun CAMERA__store(as_is _nid, optional _method)
{
		private _K_CONG_NODES = 22;

		private _N_COMMENT = 1;
		private _N_NAME = 2;
		private _N_MODEL = 3;
		private _N_IP_ADDRESS = 4;
		private _N_PORT = 5;
		private _N_TRIG_SOURCE = 6;
		private _N_NUM_FRAMES = 7;
		private _N_DELAY_FRAMES = 8;
		private _N_LENS_TYPE= 9;
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

		_delay_frames = if_error(data(DevNodeRef(_nid, _N_DELAY_FRAMES)), _error = 1);
		if(_error)
		{
		DevLogErr(_nid, "Missing delay between frame");
		abort();
		}

	write(*, _delay_frames );
	/*****************************************************************************
		if( ( allocated (public _laser_nd_connected) ) == 0)
		{
			public _laser_nd_connected = 0;
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
	*******************************************************************************/

		_sock = TCPOpenConnection(_ip, _port, _ASCII_MODE, 10000, _sw=0);
		if(_sock == 0)
		{
			DevLogErr(_nid, "Cannot connect to remote instruments"); 
			abort();
		}

		if((_err_msg = TCPSendCommand( _sock, "CAMERA_STORE") ) != "")
		{
			DevLogErr(_nid, "Error during send  CAMERA_STORE command "//_err_msg); 
			abort();
		}

        _img = CAMERAReadFrames(_sock, _n_frames = 0, _x_pixel = 0, _y_pixel = 0);


	    _trig = 0;


		if( size( _img ) == 0)
		{
			DevLogErr(_nid, "No image read for CAMERA"); 
		} else {
			_dim = make_dim(make_window(0, _n_frames - 1, _trig), [1.._n_frames]);
			_img = set_range(_y_pixel, _x_pixel, _n_frames, _img);
			_video = compile('build_signal(($VALUE), (`_img), (`_dim))');
			_video_nid =  DevHead(_nid) + _N_VIDEO;
			_status = TreeShr->TreePutRecord(val(_video_nid),xd(_video),val(0));
			if(! (_status & 1))
			{
				DevLogErr(_nid, 'Error writing data in pulse file');
			}
		}

		TCPCloseConnection(_sock);

		return (1);

}















