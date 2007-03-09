public fun FASTCAM__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 25;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CAMERA_ID = 2;
    private _N_SW_MODE = 3;
    private _N_IP_ADDR = 4;
    private _N_TRIG_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_USE_TIME = 7;
    private _N_NUM_FRAMES = 8;
    private _N_START_TIME = 9;
    private _N_END_TIME = 10;
    private _N_V_RES = 11;
    private _N_H_RES = 12;
    private _N_FRAME_RATE = 13;
    private _N_SHUTTER = 14;

    private _N_MODEL = 15;
    private _N_LENS_TYPE = 16;
    private _N_APERTURE = 17;
    private _N_F_DISTANCE = 18;
    private _N_FILTER  = 19;
    private _N_SHUTTER = 20;
    private _N_TOR_POSITION = 21;
    private _N_POL_POSITION = 22;
    private _N_TARGET_ZONE = 23;
    private _N_PIXEL_FRAME = 24;
    private _N_VIDEO = 25;

	private  _INVALID = -1;

    write(*, 'FASTCAM');

    _camera_id = if_error(data(DevNodeRef(_nid, _N_CAMERA_ID)), _INVALID);
    if(_camera_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid camera ID specification");
 		abort();
    }

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


    _trig = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);
    if(_trig == _INVALID)
    {
    	DevLogErr(_nid, "Cannot resolve trigger ");
 		abort();
    }

	_num_trig = if_error( esize(_trig), 1);
	if( _num_trig < 0 ) _num_trig = 1;

    _num_frames = if_error( data(DevNodeRef(_nid, _N_NUM_FRAMES)), _INVALID);
    if(_num_frames == _INVALID)
    {
    	DevLogErr(_nid, "Invalid number of frames value ");
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

    _frame_rate = if_error( data(DevNodeRef(_nid, _N_FRAME_RATE)), _INVALID);
	if( _frame_rate == _INVALID || FastCamResToFrate( _v_res, _h_res) != _frame_rate)
    {
    	DevLogErr(_nid, "Invalid frame rate value ");
 		abort();
    }

    _shutte = if_error( data(DevNodeRef(_nid, _N_SHUTTER)), _INVALID);
    if(_shutte == _INVALID)
    {
    	DevLogErr(_nid, "Invalid shutter speed value ");
 		abort();
    }

    DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);
	if( _time_cvt )
	{
		_start_time = if_error( data(DevNodeRef(_nid, _N_START_TIME)), _INVALID);
		if(_start_time == _INVALID)
		{
    		DevLogErr(_nid, "Invalid start time value ");
 			abort();
		}

		_end_time = if_error( data(DevNodeRef(_nid, _N_END_TIME)), _INVALID);
		if(_end_time == _INVALID)
		{
    		DevLogErr(_nid, "Invalid start time value ");
 			abort();
		}
	
		_num_frames = (_end_time - _start_time) * _frame_rate;

		DevPut(_nid, _N_NUM_FRAMES, _num_frames);

	}
	else
	{
		_num_frames = if_error( data(DevNodeRef(_nid, _N_NUM_FRAMES)), _INVALID);
		if(_num_frames == _INVALID)
		{
    		DevLogErr(_nid, "Invalid number of frames value ");
 			abort();
		}
	}


	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('FastCamHWinit($, $, $, $, $, $, $)', _frame_rate, _num_frames, _num_trig, _shutter, _vRes, _hRes, _ext_trig);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error Initializing FAST CAMERA");
			abort();
		}
	}
	else
	{
	    _status = FastCamHWinit( _frame_rate, _num_frames, _num_trig, _shutter, _vRes, _hRes, _ext_trig );
		if(_status == 0)
			abort();
	}

	return(1);
}
			



