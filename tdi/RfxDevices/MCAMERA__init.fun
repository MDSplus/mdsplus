public fun MCAMERA__init(as_is _nid, optional _method)
{
	private _K_CONG_NODES = 70;

    private _N_COMMENT = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDRESS = 3;

    private _N_CHANNEL_0 = 4;

    private _K_NODES_PER_CHANNEL = 17;

    private _K_TRIG_MODE = 1;
    private _K_TRIG_SOURCE = 2;
    private _K_NUM_FRAMES = 3;
    private _K_MODEL = 4;
    private _K_LENS_TYPE = 5;
    private _K_APERTURE = 6;
    private _K_F_DISTANCE = 7;
    private _K_FILTER = 8;
    private _K_SHUTTER = 9;
    private _K_TOR_POSITION = 10;
    private _K_POL_POSITION = 11;
    private _K_TARGET_ZONE = 12;
    private _K_PIXEL_FRAME = 13;
    private _K_FRAME_RATE = 14;
    private _K_VIDEO = 15;
    private _K_ZERO_LEVEL = 16;

    private _NUM_CAMERAS = 4;

/*
time out di 6 minuti
*/
    private _TIMEOUT = (60 * 1000 * 6) / 3;

write(*, "Multi CAMERA init");

   _status = 0;
   _error = 0;
   _globalError = 0;
   _globalErrorMessage = "";

	DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0, 1], _remote = 0);
	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), "");
		if(_ip_addr == "")
		{
    	    	DevLogErr(_nid, "Invalid Crate IP specification");
 		    	abort();
		}
	}

	if( _remote )
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		if_error( execute(_cmd), _error = 1);
		if( _error )
		{
			DevLogErr(_nid, "Cannot connect to mdsip server "//_ip_addr );
			abort();
		}
	}


	for(  _i = 0; _i < _NUM_CAMERAS; _i++ )
	{
		if(_remote)
			MdsValue("NI_PCI_14XX->cameraHWClose( val($1) )", _i );
		else
			NI_PCI_14XX->cameraHWClose( val( _i ) );

	}

	for(  _i = 0; _i < _NUM_CAMERAS; _i++ )
	{
		write(*, "Camera "//_i);
		_error = 0;

		_head_chan_nid =  _N_CHANNEL_0 + ( _i *  _K_NODES_PER_CHANNEL );


		if(DevIsOn(DevNodeRef(_nid, _head_chan_nid )))
		{ 

			_n_frames = if_error(data(DevNodeRef(_nid , _head_chan_nid + _K_NUM_FRAMES)), _error = 1);
			if(_error)
			{
				_globalErrorMessage = _globalErrorMessage//"Missing number of frames for camera "//trim(adjustl(_i))//"\n";
				_globalError = 1;
				continue;
			}
			
			DevNodeCvt( _nid , _head_chan_nid +  _K_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _trig_mode = 0);
			if( _trig_mode == 1)
			{
				_trigTime = if_error(data(DevNodeRef(_nid , _head_chan_nid + _K_TRIG_SOURCE)), _error = 1);
				if(_error)
				{
					_globalErrorMessage = _globalErrorMessage//"Missing trigger source value for camera "//trim(adjustl(_i))//"\n";
					_globalError = 1;
					continue;
				}
			}

			
			if( _remote )
				_status = MdsValue("NI_PCI_14XX->cameraHWOpen( val($1) )", _i );
			else
				_status = NI_PCI_14XX->cameraHWOpen( val( _i ) );


			if( _status )
			{
				if( _remote )
					_msg = MdsValue("MCAMERAErrorMessage( $1 )",  _i );
				else
					_msg = MCAMERAErrorMessage( _i );

				_globalErrorMessage = _globalErrorMessage//"\n"//_msg;
				_globalError = 1;
				
				
				continue;
			}

			if( _remote )
				_status = MdsValue('NI_PCI_14XX->cameraHWArm( val( $1 ), val( $2 ), val( $3 ), val( $4 ) )', _i, _TIMEOUT, _n_frames ,  _trig_mode );
			else
				_status = NI_PCI_14XX->cameraHWArm( val( _i ), val(_TIMEOUT), val( _n_frames ), val( _trig_mode) );


			if( _status )
			{
				if( _remote )
					_msg =  MdsValue("MCAMERAErrorMessage( $1 )",  _i );
				else
					_msg = MCAMERAErrorMessage( _i );


				_globalErrorMessage = _globalErrorMessage//"\n"//_msg;
				

				_globalError = 1;
			}
		}
	}

	if( _remote )
		MdsDisconnect();

	if( _globalError )
	{
		DevLogErr(_nid, _globalErrorMessage);
		abort();
	}

	return (1);

}
