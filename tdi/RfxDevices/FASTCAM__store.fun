public fun FASTCAM__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES	= 33;
    private _N_HEAD			= 0;
    private _N_COMMENT		= 1;
    private _N_CAMERA_ID	= 2;
    private _N_SW_MODE		= 3;
    private _N_IP_ADDR		= 4;
    private _N_ACQ_MODE		= 5;
    private _N_TRIG_MODE	= 6;
    private _N_TRIG_SOURCE	= 7;
    private _N_CALIBRATE	= 8;
    private _N_USE_TIME		= 9;
    private _N_NUM_FRAMES	= 10;
    private _N_START_TIME	= 11;
    private _N_END_TIME		= 12;
	private _N_RES_SOURCE	= 13;

    private _N_RESOLUTION	= 14;
    private _N_FRAME_RATE	= 15;
    private _N_SHUTTER_IN	= 16;
    private _N_SHUTTER   	= 17;

    private _N_H_RES     	= 18;
    private _N_V_RES	   	= 19;

    private _N_MODEL		= 20;
    private _N_LENS_TYPE	= 21;
    private _N_APERTURE		= 22;
    private _N_F_DISTANCE	= 23;
    private _N_FILTER		= 24;
    private _N_TOR_POSITION	= 25;
    private _N_POL_POSITION	= 26;
    private _N_TARGET_ZONE	= 27;
    private _N_PIXEL_FRAME	= 28;
    private _N_VIDEO		= 29;
    private _N_DATA			= 30;

    private _K_SHADING_OFF	= 0;
    private _K_SHADING_ON1	= 1;
    private _K_SHADING_ON2	= 2;
    private _K_SHADING_ON3	= 3;
    private _K_SHADING_SAVE	= 4;
    private _K_SHADING_LOAD	= 5;


	private  _INVALID = -1;
	
	_status = 1;

	
    write(*, 'FASTCAM store');

/*

    _camera_id = if_error(data(DevNodeRef(_nid, _N_CAMERA_ID)), _INVALID);
    if(_camera_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid camera ID specification");
 		abort();
    }
*/

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

    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_trig = 0);

	if( _ext_trig )
	{
		_trigs = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), []);
		_num_trig = esize(_trigs);
		if( _num_trig == 0 )
		{
    		DevLogErr(_nid, "Cannot resolve trigger ");
 			abort();
		}
		if(_num_trig == -1)
		{
			_num_trig = 1;
			_trigs =  [0];
		}
	}
	else
	{
		_num_trig = 1;
		_trigs =  [0];
	}


    DevNodeCvt(_nid, _N_ACQ_MODE, ['VIDEO & DATA', 'VIDEO', 'DATA' ], [2, 0, 1], _acq_mode = 0);

 
    _num_frames = if_error( data(DevNodeRef(_nid, _N_NUM_FRAMES)), _INVALID);
    if(_num_frames == _INVALID)
    {
    	DevLogErr(_nid, "Invalid frames number  value ");
 		abort();
    }

	_num_frames--;

	write(*, "Num frames ", _num_frames);

	_resolution = if_error( data(DevNodeRef(_nid, _N_RESOLUTION)), _status = _INVALID);
	if( _status == _INVALID )
	{
    	DevLogErr(_nid, "Invalid resolution value ");
 		abort();
	}

	write(*, "_N_RESOLUTION ", _resolution);

	_res = FastCamCheckResolution( _resolution );
	
	_h_res = _res[0]; 
 	_v_res = _res[1]; 
 
	if( _h_res == _INVALID || _v_res == _INVALID )
	{
    	DevLogErr(_nid, "Invalid horozontal/vertical resolution value ");
 		abort();
	}

	write(*, "_h_res _v_res ", _h_res, _v_res);

	_max_frame_rate = FastCamResToFrate( _v_res, _h_res);
    _frame_rate = if_error( data(DevNodeRef(_nid, _N_FRAME_RATE)), _INVALID);
	if( _frame_rate == _INVALID || _frame_rate > _max_frame_rate)
    {
    	DevLogErr(_nid, "Invalid frame rate value ; for resolution "//trim(adjustl(_v_res))//"x"//trim(adjustl(_h_res))//" frame rate max "//trim(adjustl(_max_frame_rate))//" fps");
 		abort();
    }

    _shutter = if_error( data(DevNodeRef(_nid, _N_SHUTTER)), _INVALID);
    if(_shutter == _INVALID)
    {
    	DevLogErr(_nid, "Invalid shutter speed value ");
 		abort();
    }

	_end_time = if_error( data(DevNodeRef(_nid, _N_END_TIME)), _status = _INVALID);
	if( _status == _INVALID)
	{
    	DevLogErr(_nid, "Invalid end time value ");
 		abort();
	}

	write(*, "End time : ", _end_time);

    _resample_source = if_error( data(DevNodeRef(_nid, _N_RES_SOURCE)), _status = _INVALID);
    if( _status == _INVALID )
	{
		_resample_source = _trigs : _trigs + _end_time : 1./_frame_rate;
	}
/*
	write(*, "_resample_source : ", _resample_source);
*/
	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	}

	if(_remote != 0)
		_status = MdsValue('FastCamHWTriggered()');
	else
		_status = FastCamHWTriggered();

	if( _status < 0 )
	{
		if( _remote != 0) 
		{
			_msg = MdsValue('FastCamErrno($)', _status);
			MdsValue('FastCamHWClose()');
			MdsDisconnect();
		}
		else
			_msg = FastCamErrno( _status );
		
		DevLogErr(_nid, "Error reading FAST CAMERA "//_msg);
		Abort();
	}


	if( _acq_mode == 0 || _acq_mode == 2)
	{

/*
 * Save into pulse file the video signal
 */

		_ranges = [];
		_maxNumFramesInSingleRead = int ( (1024 * 1024) / ( _v_res * _h_res) );

		_startFrame = 0;
		_numFrameToStore = 0;

		_expositionDelay = 2.35e-6;

		_deltaFrameTime =  _expositionDelay + (2./_shutter);

		_frameRange = ( _trigs : _trigs + (_num_frames * 1./_frame_rate) : 1./_frame_rate ) + _deltaFrameTime; 

 /*
		 write(*, "frame range ", _frameRange);
 */

		_totalFrame = size( _frameRange );
		_frameIndexes = [];
		for(_n = 0, _k=0;  _n < _totalFrame - 1 && _k < size( _resample_source ); _n++ )
		{			
			for(;	_k < (size( _resample_source ) - 1) &&  _resample_source[ _k ] < _frameRange[ _n ]  ; _k++ );

		    if( _frameRange[ _n ] <= _resample_source[ _k ]  && _frameRange[ _n + 1] > _resample_source[ _k ] ) _frameIndexes = [ _frameIndexes, _n];
		}

		_frameTimes = _frameRange[ _frameIndexes ];
		_n_frames   = size( _frameIndexes );

		_max_frames = 25*(1024/_v_res)*(1024/_h_res);
		_imgs	    = [];
		for(_idx_start = 0;  _idx_start < _n_frames; _idx_start = _idx_start + _max_frames)
		{
			_idx_end = _idx_start + _max_frames - 1;
			if( _idx_end >= _n_frames)
			{
				_idx_end    = _n_frames - 1;
				_num_frames = _idx_end - _idx_start + 1;
			}
			else _num_frames = _max_frames;
			
			_startFrames = _frameIndexes[ _idx_start : _idx_end ];
			
			write(*, "Leggo dal frame ", _frameIndexes[ _idx_start ]);
			write(*, "       al Frame ", _frameIndexes[ _idx_end ]);
			if(_remote != 0)
				_data = MdsValue('FastCamHWReadFrames($, $, $, $)', _startFrames, _num_frames, _v_res, _h_res);
			else
				_data = FastCamHWReadFrames( _startFrames, _num_frames, _v_res, _h_res);
			
			
			if( esize( _data ) < 0)
			{
				_n_frames = 0;
				write(*, "Interroto");
				break;
			}
			_imgs = [ _imgs, _data ];
		}

		write(*, "Letti num frames ", _n_frames);
		if( _n_frames > 0 )
		{
			_y_pixel = _v_res;
			_x_pixel = _h_res;
	
			_imgs = WORD(( _imgs >> 6 ) & 0x3FF);
	
	/*
			_dim = make_dim(make_window(0, _n_frames - 1, _frameRange[0]), _frameRange );
*/
			_video = compile('build_signal(($VALUE), set_range(`_x_pixel, `_y_pixel, `_n_frames, `_imgs), (`_frameTimes))');
	

			_video_nid =  DevHead(_nid) + _N_VIDEO;
			
 _SEGMENTED_ACQ = 1;	
if( _SEGMENTED_ACQ == 0 )
{			
			write(*, "Write Bulk data"); 

			_status = TreeShr->TreePutRecord(val(_video_nid),xd(_video),val(0));
			if( !(_status & 1) )
			{

				if( _remote != 0 )
				{
					MdsValue('FastCamHWClose()');
					MdsDisconnect();
				}
				else
				{
					_msg = FastCamHWClose();
				}


				DevLogErr(_nid, 'Error writing data in pulse file');
				abort();
			}
			write(*, "End Write Bulk data"); 
}
else
{

			write(*, "Write Segmented data"); 

			_frames = set_range(_x_pixel, _y_pixel, _n_frames, _imgs);
			_status = 1;

			for ( _k = 0; _k < _n_frames && (_status & 1) ; _k++ )
			{
				if( ( _k mod 50 ) == 0 || (_k+1) == _n_frames  )
				write(*, "Frames wrote ", _k );

				_array = compile('set_range(`_x_pixel, `_y_pixel, 1, ` _frames[*,*,`_k])');
				_time =_frameTimes[ _k ];
				_dim = compile('[ `_time ]');

    			_status = TreeShr->TreeMakeSegment(val(_video_nid),descr(_time),descr(_time), descr(_dim), descr( _array ),val(-1),val(1) );
			}

			if( !(_status & 1) )
			{
				if( _remote != 0 )
				{
					MdsValue('FastCamHWClose()');
					MdsDisconnect();
				}
				else
				{
					_msg = FastCamHWClose();
				}
				DevLogErr(_nid, 'Error writing segmented data in pulse file');
				abort();
			}
			write(*, "End write Segmented data : ", getmsg(_status)); 
}
/**/
/*
		}
	}

	if( _acq_mode == 1 || _acq_mode == 2)
	{
*/
			write(*, "Reading signal ");

	/**
	* Save into pulse file the signal of the mean value of the frame 
	**/
			_expositionDelay = 2.35e-6;
			_deltaFrameTime =  _expositionDelay + (2./_shutter);

			_num_samples = long(_end_time*_frame_rate)+1;
			_period = 1./_frame_rate;

			_lens_type = if_error( data(DevNodeRef(_nid, _N_LENS_TYPE)), _status = _INVALID);
			if( _status == _INVALID )
			{
				DevLogErr(_nid, "Invalid lens type ");
				abort();
			}
			
			_aperture = if_error( data(DevNodeRef(_nid, _N_APERTURE)), _status = _INVALID);
			if( _status == _INVALID )
			{
				DevLogErr(_nid, "Invalid aperture ");
				abort();
			}

			_f_distance = if_error( data(DevNodeRef(_nid, _N_F_DISTANCE)), _status = _INVALID);
			if( _status == _INVALID )
			{
				DevLogErr(_nid, "Invalid Aperture ");
				abort();
			}
			
			_filter = if_error( data(DevNodeRef(_nid, _N_FILTER)), _status = _INVALID);
			if( _status == _INVALID )
			{
				DevLogErr(_nid, "Invalid F distance ");
				abort();
			}
			
			_comp_mode = 0;
			_help = "Average Intensity";
			if(_remote != 0)
					_data = MdsValue('FastCamHWReadSignal($, $, $, $, $, $)',_comp_mode , _num_samples, text(_lens_type), float(_aperture), float(_f_distance), text(_filter));
				else
					_data = FastCamHWReadSignal(_comp_mode, _num_samples, text(_lens_type), float(_aperture), float(_f_distance), text(_filter));
			
			_signal_nid =  DevHead(_nid) + _N_DATA;
			_dim = make_dim(make_window(0, _num_samples - 1, _trigs[0]+_deltaFrameTime), make_range(*,*,_period));
			_signal = compile('build_param(build_signal(($VALUE), (`_data), (`_dim)),(`_help),)');

				
			_status = TreeShr->TreePutRecord(val(_signal_nid),xd(_signal),val(0));
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');
				abort();
			}
		}
	}	

	if(_remote != 0)
		MdsValue('FastCamHWClose()');
	else
		_msg = FastCamHWClose();
	
	if( _remote != 0 )
		MdsDisconnect();


	return( _status );
}
			
