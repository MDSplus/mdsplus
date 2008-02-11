public fun FASTCAM__trigger(as_is _nid, optional _method)
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

    write(*, 'FASTCAM trigger');
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

    DevNodeCvt( _nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _ext_trig = 0);
	if( _ext_trig == 1 )
	{
 	    DevLogErr(_nid, "Perform this operation in INTERNAL trigger configuration");
		abort();
	}


	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('FastCamHWtrigger()');
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
			



