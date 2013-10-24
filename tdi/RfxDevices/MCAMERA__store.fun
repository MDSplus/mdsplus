public fun MCAMERA__store(as_is _nid, optional _method)
{
	private _K_CONG_NODES = 74;

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

    private _TIMEOUT = 3000;

write(*, "Multi CAMERA Store");

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
		_error = 0;

		_head_chan_nid =  _N_CHANNEL_0 + ( _i *  _K_NODES_PER_CHANNEL );
 
		if(DevIsOn(DevNodeRef(_nid, _head_chan_nid )))
		{ 
		    write(*, "Camera "//_i);

			_n_frames = if_error(data(DevNodeRef(_nid , _head_chan_nid + _K_NUM_FRAMES)), _error = 1);
			if(_error)
			{
				_globalErrorMessage = _globalErrorMessage//"Missing number of frames for camera "//trim(adjustl(_i))//"\n";
				_globalError = 1;
				continue;
			}

			DevNodeCvt( _nid , _head_chan_nid +  _K_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _trig_mode = 0);
			if(_trig_mode == 1)
			{
				_trig_time = if_error(data(DevNodeRef(_nid , _head_chan_nid + _K_TRIG_SOURCE)), _error = 1);
				if(_error)
				{
					_globalErrorMessage = _globalErrorMessage//"Missing trigger source value for camera "//trim(adjustl(_i))//"\n";
					_globalError = 1;
					continue;
				}
			}
			else
				_trig_time	= 0;	

			_frame_rate = if_error(data(DevNodeRef(_nid, _head_chan_nid + _K_FRAME_RATE)), _error = 1);

			if(_error)
			{
				_globalErrorMessage = _globalErrorMessage//"Missing frame rate value for camera "//trim(adjustl(_i))//"\n";
				_globalError = 1;
				continue;
			}

			_zero_level = if_error(data(DevNodeRef(_nid , _head_chan_nid + _K_ZERO_LEVEL)), _zero_level = 0);

/************************/
			if( _remote )
				_error =  MdsValue("NI_PCI_14XX->cameraHWCheckFrames( val( $1 ), val( $2 ), val( $3 ) )", _i, _n_frames, _zero_level );
			else
				_error =  NI_PCI_14XX->cameraHWCheckFrames( val( _i ), val( _n_frames ), val( 31 ) );

			if(_error)
			{
				if( _remote )
					_msg = MdsValue("MCAMERAErrorMessage( $1 )",  _i );
				else
					_msg = MCAMERAErrorMessage( _i );
					
				write(*, "  ERROR ",  _msg);

				_globalErrorMessage = _globalErrorMessage//" "//_msg;
 				
				_globalError = 1;
				continue;
			}

			if(_remote)
				_n_frames = MdsValue("NI_PCI_14XX->cameraHWGetNumFrames( val( $1 ) )", _i );
			else
				_n_frames = NI_PCI_14XX->cameraHWGetNumFrames( val( _i ) );

			if( _n_frames == 0 )
			{
				_globalErrorMessage = _globalErrorMessage//"Camera "//trim(adjustl(_i))//" did not acquire data\n";
				_globalError = 1;
				continue;
			}

			if(_remote)
				_x_pixel = MdsValue("NI_PCI_14XX->cameraHWGetWidth( val( $1 ) )", _i );
			else
				_x_pixel = NI_PCI_14XX->cameraHWGetWidth( val( _i ) );

			if( _x_pixel < 0 )
			{
				_globalErrorMessage = _globalErrorMessage//"Camera "//trim(adjustl(_i))//" invalid number of x pixel \n";
				_globalError = 1;
				continue;
			}

			if( _remote )
				_y_pixel = MdsValue("NI_PCI_14XX->cameraHWGetHeight( val( $1 ) )", _i );
			else
				_y_pixel = NI_PCI_14XX->cameraHWGetHeight( val( _i ) );

			if( _y_pixel < 0 )
			{
				_globalErrorMessage = _globalErrorMessage//"Camera "//trim(adjustl(_i))//" invalid number of y pixel \n";
				_globalError = 1;
				continue;
			}

			if( _remote )
				_bitPerPixel = MdsValue("NI_PCI_14XX->cameraHWGetBitPixel( val( $1 ) )", _i );
			else
				_bitPerPixel = NI_PCI_14XX->cameraHWGetBitPixel( val( _i ) );

			if( _bitPerPixel < 0 )
			{
				_globalErrorMessage = _globalErrorMessage//"Camera "//trim(adjustl(_i))//" invalid number of bit per pixel \n";
				_globalError = 1;
				continue;
			}

write(*,_n_frames);
write(*,_x_pixel);
write(*,_y_pixel);
write(*,_bitPerPixel);

			if( _remote )
				_img = MdsValue("MCAMERAReadFrames( $1, $2, $3, $4, $5)", _i, _bitPerPixel,  _n_frames,  _x_pixel,  _y_pixel);
			else
				_img = MCAMERAReadFrames( _i, _bitPerPixel,  _n_frames,  _x_pixel,  _y_pixel);

/***********************************/

_DBG_TEST = 0;
if(_DBG_TEST )
{
write(*, "Dummy data"); 
			_n_frames = 10;
			_x_pixel = 100;
			_y_pixel = 100;
			_img = [];

			for( _k = 0; _k < _n_frames; _k++ )
			{
				_img0 = transpose(set_range(_x_pixel,_y_pixel,replicate( 1.._y_pixel, 0,  _x_pixel ))) * ( _k + 1 );
/*
				_img0 = replicate([byte(1+_k)], 0, _x_pixel * _y_pixel );
*/
				_img = [ _img, byte( _img0 ) ];
			}
}
/*******************************************/

			if( size( _img ) == 0)
			{

				if( _remote )
					_msg = MdsValue("MCAMERAErrorMessage( $1 )",  _i );
				else
					_msg = MCAMERAErrorMessage( _i );


				write(*, "ERROR ", _msg);

				_globalErrorMessage = _globalErrorMessage//" "//_msg;
				_globalError = 1;
			} 
			else 
			{

if( _DBG_TEST != 0 )
{

write(*, "Write Bulk data", _frame_rate); 

				_trig_time = _trig_time + 1./( 2 * _frame_rate );
			
				_dim = make_dim(make_window(0, _n_frames - 1, _trig_time), make_range(*,*,(1./_frame_rate)) );
				_video = compile('build_signal(($VALUE), set_range(`_x_pixel, `_y_pixel, `_n_frames, `_img), (`_dim))');
				
				_video_nid =  DevHead(_nid) + _head_chan_nid + _K_VIDEO;
				_status = TreeShr->TreePutRecord(val(_video_nid),xd(_video),val(0));
}else{

/************************************/


write(*, "Write Segmented data"); 


				_trig_time = _trig_time + 1./( 2 * _frame_rate );

				_video = compile('set_range(`_x_pixel, `_y_pixel, `_n_frames, `_img)');

				_time = _trig_time;

		        _frames1 = _video[*, [ 0 : _y_pixel : 2], *];

		        _framesEven = _frames1[*,int([0 : ( _y_pixel+0.5) : 0.5]), *];

		        _frames1 = _video[*, [ 1 : _y_pixel : 2], *];

		        _framesOdd = _frames1[*,int([0 : (_y_pixel+0.5) : 0.5]), *];

				_video_nid =  DevHead(_nid) + _head_chan_nid + _K_VIDEO;

				_status = 1;

		
				for ( _k = 0; _k < _n_frames && (_status & 1) ; _k++ )
				{

					_array = compile('set_range(`_x_pixel, `_y_pixel, 1, ` _framesEven[*,*,`_k])');
					_dim = compile('[ `_time ]');

/*
					 write( *, "EVEN", _k, _time, _frame_rate, _status, shape(_array ));
					 write( *, _k, _time, _frame_rate, _status, shape(_array ), _array );
*/
      				_status = TreeShr->TreeMakeSegment(val(_video_nid),descr(_time),descr(_time), descr(_dim), descr( _array ),val(-1),val(1) );

					if( _status & 1 )
					{
						_time = _time + 1./_frame_rate/2.;	
						_array = compile('set_range(`_x_pixel, `_y_pixel, 1, `_framesOdd[*,*,`_k])');	
						_dim = compile('[ `_time ]');
/*
					 	write( *, _k, _time, _frame_rate, _status, shape(_array ), _array );
					 	write( *, "Odd", _k, _time, _frame_rate, _status, shape(_array ));
*/

      					_status = TreeShr->TreeMakeSegment(val(_video_nid),descr(_time),descr(_time), descr(_dim), descr( _array ),val(-1),val(1) );
						_time = _time + 1./_frame_rate/2.;
					}
				}
write(*, "End write Segmented data : ", getmsg(_status)); 
/************************************/
}
			
				if(! (_status & 1))
				{
					_globalErrorMessage = _globalErrorMessage//"Camera "//trim(adjustl(_i))//" Error writing data in pulse file\n";
					_globalError = 1;
				}
			}
		}
	}


	
	for(  _i = 0; _i < _NUM_CAMERAS; _i++ )
	{

		_head_chan_nid =  _N_CHANNEL_0 + ( _i *  _K_NODES_PER_CHANNEL );
 
		if(DevIsOn(DevNodeRef(_nid, _head_chan_nid )))
		{ 

			if( _remote )
				MdsValue("NI_PCI_14XX->cameraHWClose( val( $1 ) )", _i);
			else
				NI_PCI_14XX->cameraHWClose( val( _i ) );
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















