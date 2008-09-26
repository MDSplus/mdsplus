public fun FASTCAM__init(as_is _nid, optional _method)
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
	private  _status = 0;

    write(*, 'FASTCAM init');

/****************************************************************************
    _camera_id = if_error(data(DevNodeRef(_nid, _N_CAMERA_ID)), _INVALID);
    if(_camera_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid camera ID specification");
 		abort();
    }
******************************************************************************/

    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);

	write(*, "_remote", _remote);

	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
	}

	write(*, "_remote", _remote);

    DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_trig = 0);

	if( _ext_trig )
	{
		_trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), []);
		_num_trig = esize(_trig);

		if( _num_trig == 0 )
		{
    		DevLogErr(_nid, "Cannot resolve trigger ");
 			abort();
		}
	}
	else
	{
		_num_trig = 1;
		_trig =  0;
	}

	write(*, "_ext_trig", _ext_trig);

	_resolution = if_error( data(DevNodeRef(_nid, _N_RESOLUTION)), _status = _INVALID);
	if( _status == _INVALID )
	{
    	DevLogErr(_nid, "Invalid resolution value ");
 		abort();
	}

	_res = FastCamCheckResolution( _resolution );
	
	_h_res = _res[0]; 
 	_v_res = _res[1]; 

	write(*, "_resolution ", _resolution );

 
	if( _h_res == _INVALID || _v_res == _INVALID )
	{
    	DevLogErr(_nid, "Invalid horozontal - vertical resolution value ");
 		abort();
	}

	DevPut(_nid, _N_H_RES, int( _h_res ) );
	DevPut(_nid, _N_V_RES, int( _v_res ) );

	write(*, "_h_res _v_res", _h_res, _v_res);

	_max_frame_rate = FastCamCheckFrameRate( _resolution );
	write(*, "_max_frame_rate", _max_frame_rate);

    _frame_rate = if_error( data(DevNodeRef(_nid, _N_FRAME_RATE)), _INVALID);
	write(*, "_frame_rate", _frame_rate);

	if( _frame_rate == _INVALID ||  _frame_rate > _max_frame_rate)
    {
    	DevLogErr(_nid, "Invalid frame rate value ; for resolution "//_resolution//" frame rate max "//trim(adjustl(_max_frame_rate))//" fps");
 		abort();
    }

    _shutter_in = if_error( data(DevNodeRef(_nid, _N_SHUTTER_IN)), _status = _INVALID);
	write(*, "_shutter_in ", _shutter_in);
	write(*, getnci(DevNodeRef(_nid, _N_SHUTTER_IN), "FULLPATH"));

    if( _status == _INVALID )
    {
    	DevLogErr(_nid, "Cannot read shutter speed value ");
 		abort();
    }

	if( _shutter_in != _frame_rate )
	{
		_shutter = FastCamCheckShutter( _resolution,  _shutter_in );
		if( _shutter == 0 )
		{
    		DevLogErr(_nid, " Invalid shutter speed value ");
 			abort();
		}
	} else
		_shutter = _shutter_in;

	write(*, "_shutter ", _shutter);

	DevPut(_nid, _N_SHUTTER, _shutter);

    DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
	if( _time_cvt )
	{
		_start_time = if_error( data(DevNodeRef(_nid, _N_START_TIME)), _status = _INVALID);

		if( _status == _INVALID)
		{
    		DevLogErr(_nid, "Invalid start time value ");
 			abort();
		}

		_end_time = if_error( data(DevNodeRef(_nid, _N_END_TIME)), _status = _INVALID);
		if( _status == _INVALID)
		{
    		DevLogErr(_nid, "Invalid end time value ");
 			abort();
		}
	
		_num_frames = int ( (_end_time - _start_time) * _frame_rate ) + 1;

		DevPut(_nid, _N_NUM_FRAMES, _num_frames);

	}
	else
	{
		_num_frames = if_error( data(DevNodeRef(_nid, _N_NUM_FRAMES)), _status = _INVALID);
		if( _status == _INVALID )
		{
    		DevLogErr(_nid, "Invalid frames number value ");
 			abort();
		}

		DevPut(_nid, _N_START_TIME, 0);
		DevPut(_nid, _N_END_TIME,  _num_frames * 1./_frame_rate);

	}

	write(*, "_num_frames", _num_frames);

    DevNodeCvt(_nid, _N_CALIBRATE, ['OFF', 'ON', 'LOAD'], [0,1,5] ,  _calibrate = 0 );

	write(*, "_calibrate", _calibrate);

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';

		execute(_cmd);
	    _status = MdsValue('FastCamHWinit($, $, $, $, $, $, $)', _frame_rate, _num_frames, _num_trig, _shutter, _calibrate, _v_res, _h_res);
		if(_status < 0 )
			_msg = MdsValue('FastCamErrno($)', _status);

		MdsDisconnect();

		write(*, "_ip_addr ", _ip_addr);

		if(_status < 0)
		{
			DevLogErr(_nid, "Error Initializing FAST CAMERA "//_msg);
			abort();
		}

	}
	else
	{
	    _status = FastCamHWinit( _frame_rate, _num_frames, _num_trig, _shutter, _calibrate, _v_res, _h_res);
		if(_status < 0)
		{
			_msg = FastCamErrno( _status );	
			DevLogErr(_nid, "Error Initializing FAST CAMERA "//_msg);
			abort();
		}
	}


	return(1);
}
			



