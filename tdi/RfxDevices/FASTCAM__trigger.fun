public fun FASTCAM__trigger(as_is _nid, optional _method)
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

    write(*, 'FASTCAM trigger');

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

    DevNodeCvt( _nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_trig = 0);
	if( _ext_trig == 1 )
	{
 	    DevLogErr(_nid, "Perform this operation on in INTERNAL trigger configuration");
 		abort();
	}


	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('FastCamHWinit()');
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error triggering FAST CAMERA");
			abort();
		}
	}
	else
	{
	    _status = FastCamHWtrigger();
		if(_status == 0)
			abort();
	}

	return(1);
}
			



