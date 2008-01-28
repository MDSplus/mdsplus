public fun FASTCAM__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 28;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CAMERA_ID = 2;
    private _N_SW_MODE = 3;
    private _N_IP_ADDR = 4;
    private _N_TRIG_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_CALIBRATE = 7;
    private _N_USE_TIME = 8;
    private _N_NUM_FRAMES = 9;
    private _N_START_TIME = 10;
    private _N_END_TIME = 11;
    private _N_V_RES = 12;
    private _N_H_RES = 13;
    private _N_FRAME_RATE = 14;
    private _N_SHUTTER = 15;

    private _N_MODEL = 16;
    private _N_LENS_TYPE = 17;
    private _N_APERTURE = 18;
    private _N_F_DISTANCE = 19;
    private _N_FILTER  = 20;
    private _N_TOR_POSITION = 21;
    private _N_POL_POSITION = 22;
    private _N_TARGET_ZONE = 23;
    private _N_PIXEL_FRAME = 24;
    private _N_VIDEO = 25;


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
 
    _num_frames = if_error( data(DevNodeRef(_nid, _N_NUM_FRAMES)), _INVALID);
    if(_num_frames == _INVALID)
    {
    	DevLogErr(_nid, "Invalid frames number  value ");
 		abort();
    }

    _v_res = if_error( data(DevNodeRef(_nid, _N_V_RES)), _INVALID);
	if( _v_res == _INVALID || FastCamCheckVres( _v_res ) ==  _INVALID )
	{
    	DevLogErr(_nid, "Invalid vertical resolution vale ");
 		abort();
	}

    _h_res = if_error( data(DevNodeRef(_nid, _N_H_RES)), _INVALID);
	if( _h_res == _INVALID || FastCamCheckHres( _h_res ) ==  _INVALID )
	{
    	DevLogErr(_nid, "Invalid horizontal resolution vale ");
 		abort();
	}

	_mac_frame_rate = FastCamResToFrate( _v_res, _h_res);
    _frame_rate = if_error( data(DevNodeRef(_nid, _N_FRAME_RATE)), _INVALID);
	if( _frame_rate == _INVALID || _frame_rate > _mac_frame_rate)
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

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	}

	_imgs = [];
	_ranges = [];
	_maxFramesSingleRead = int ( (1024 * 1024) / (_v_res * _h_res) );

	_startFrame = 0;

	_numTotFrame = 0;

	for( _nTr = 0; _nTr < _num_trig; _nTr++)
	{
		_frameToRead = _num_frames;
		while( _frameToRead > 0 )
		{

			_nFramesSingleRead = (_maxFramesSingleRead < _frameToRead ) ? _maxFramesSingleRead : _frameToRead;

/*
			write(*, "Tot "//_frameToRead//" Frame to read "// _nFramesSingleRead//" start "//_startFrame);
*/

			if(_remote != 0)
				_data = MdsValue('FastCamHWReadFrame($, $, $, $)', _startFrame,  _nFramesSingleRead, _v_res, _h_res);
			else
				_data = FastCamHWReadFrame( _startFrame, _nFramesSingleRead, _v_res, _h_res);


			write(*, "Data size ", esize( _data ) );
			
			if( esize( _data ) < 0)
				break;

			for( _nFr = _startFrame; _nFr < _startFrame + _nFramesSingleRead; _nFr++)
				_ranges = [ _ranges, _trigs[_nTr] + ( 1./_frame_rate ) * _nFr ];

			_imgs = [ _imgs, _data ];
			
			_frameToRead -= _nFramesSingleRead;	
			_startFrame  += _nFramesSingleRead;
		}
	}


	if(_remote != 0)
		MdsValue('FastCamHWClose()');
	else
		_msg = FastCamHWClose();

	_n_frames = _startFrame;

	write(*, "test ", _n_frames, _num_frames * _num_trig);

	_y_pixel = _h_res;
	_x_pixel = _v_res;

	_imgs = WORD(( _imgs >> 6 ) & 0x3FF);

	_dim = make_dim(make_window(0, _n_frames - 1, _trigs[0]), _ranges );

	_video = compile('build_signal(($VALUE), set_range(`_y_pixel, `_x_pixel, `_n_frames, `_imgs), (`_dim))');

	_video_nid =  DevHead(_nid) + _N_VIDEO;
	_status = TreeShr->TreePutRecord(val(_video_nid),xd(_video),val(0));
	if( !(_status & 1) )
	{
		if( _remote != 0 )
			MdsDisconnect();
		DevLogErr(_nid, 'Error writing data in pulse file');
		abort();
	}


	if(	_n_frames != _num_frames * _num_trig )
	{
		if(_remote != 0)
		{
			_msg = MdsValue('FastCamErrno($)', _data);
			MdsDisconnect();
		}
		else
			_msg = FastCamErrno( _data );
		
		DevLogErr(_nid, "Error reading FAST CAMERA "//_msg);
		Abort();
	}

	if( _remote != 0 )
		MdsDisconnect();


	return( _status );
}
			
