public fun FASTCAM__calibrate(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 27;
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

    private _K_SHADING_OFF		= 0;
    private _K_SHADING_ON1		= 1;
    private _K_SHADING_ON2		= 2;
    private _K_SHADING_ON3		= 3;
    private _K_SHADING_SAVE		= 4;
    private _K_SHADING_LOAD		= 5;

	private  _INVALID = -1;

    write(*, 'FASTCAM calibrate');

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


	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
	
		execute(_cmd);
	    _status = MdsValue('FastCamHWcalibrate()');
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error calibrate FAST CAMERA");
			abort();
		}
	
	}

	else
	{

	    _status = FastCamHWcalibrate(  _K_SHADING_ON1 );
		if(_status < 0)
		{
			_msg = FastCamErrno( _status );	
			DevLogErr(_nid, "Error during FAST CAMERA calibration "//_msg);
			abort();
		}
	

	}


	return(1);
}
			



