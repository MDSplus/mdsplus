public fun ST133__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     30;
    private _N_HEAD =		    0;
    private _N_COMMENT =        1;
    private _N_IP_ADDRESS =     2;
    private _N_CLOCK_MODE =     3;
    private _N_CLOCK_SOURCE =   4;
    private _N_SPEC_GROOVES =   5;
    private _N_SPEC_WAV_MID =   6;
    private _N_SPEC_MIR_POS =   7;
    private _N_GLUE_FLAG    =   8;
    private _N_X_DIM_DET =      9;
    private _N_Y_DIM_DET =     10;
    private _N_X_DIM =         11;
    private _N_Y_DIM =         12;
    private _N_DET_TEMP =      13;
    private _N_EXP_TIME =      14;
    private _N_READ_OUT =      15;
    private _N_CLEANS =        16;
    private _N_N_SKIP_CLEAN =  17;
    private _N_N_FRAMES =      18;
    private _N_NROI =          19;
    private _N_ROI =           20;
    private _N_NUM_ACC =       21;
    private _N_XCAL_VALID =    22;
    private _N_XCAL_STING =    23;
    private _N_XCAL_POL_ORD =  24;
    private _N_XCAL_COEFF =    25;
    private _N_HEADER =        26;
    private _N_DATA   =        27;


    write(*, "ST133__init");

    _shot_num = current_shot('rfx');

	_ip_address = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), "");
	if(_ip_address == "")
	{
    	DevLogErr(_nid, "Invalid Crate IP specification");
 		abort();
	}

	MdsConnect(_ip_address);


    _u=mdsvalue('fopen("C:\CURRENT_SHOT.TXT","w")');
	mdsvalue('write($,$)', _u, _shot_num);
	mdsvalue('fclose($)', _u); 

	MdsDisconnect();


    return(1);
}
