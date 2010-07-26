public fun LASER_NDRT__init_vme(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 24;

    private _N_HEAD		= 0;
    private _N_COMMENT		= 1;
    private _N_IP_ADDRESS	= 2;
    private _N_PORT		= 3;
    private _N_RT_ADDRESS	= 4;
    private _N_TRIG_MODE	= 5;
    private _N_TRIG_SOURCE	= 6;
    private _N_RT_TRIG_MODE	= 7;
    private _N_NUM_PULSES	= 8;
    private _N_DELAY_PULSE	= 9;
    private _N_MAX_AMP		= 10;
    private _N_MIN_AMP		= 11;
    private _N_MAX_PHASE	= 12;
    private _N_MIN_PHASE	= 13;
    private _N_OSC		= 14;
    private _N_AMP		= 15;
    private _N_SLAB		= 16;

    private _K_EXT_DT		= 0;
    private _K_EXT_10_DT	= 1;
    private _K_EXT_RT		= 2;

    private _K_DISABLE_RT	= 0;
    private _K_M1_N7_AMP	= 1;
    private _K_M1_N7_AMP_PH	= 2;
    private _K_RATIO_DOM_SEC	= 3;
    private _K_RATIO_DOM_SEC_PH	= 4;
    private _K_M0_N1_AMP	= 5;
    private _K_M0_N1_AMP_PH	= 6;
    private _K_M0_N7_AMP	= 7;
    private _K_M0_N7_AMP_PH	= 8;



    private _ASCII_MODE = 0;
    private _MIN_DT = 0.025;
    private _MIN_To = -0.025 - 0.0055;
	

write(*, "INIT VME real Time trigger LASER Neodimium");

wait(1);


   _error = 0;

    _ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing IP address"); 
	abort();
    }

write(*, _ip);

    _port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing TCP Port");
	abort();
    }
write(*, _port);

    _ip_rt = if_error(data(DevNodeRef(_nid, _N_RT_ADDRESS)), _error = 1);
    if(_error)
    {
	DevLogErr(_nid, "Missing REAL TIME node IP address"); 
	abort();
    }
write(*, _ip_rt);


    _to = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE	)), _error = 1);

write(*, _to);

    if( _error || _to < _MIN_To )
    {
	DevLogErr(_nid, "Trigger time (To) is missing or less than -25ms "); 
	abort();
    }

write(*, _to);


    DevNodeCvt(_nid, _N_TRIG_MODE, ['EXT_DT', 'EXT_10_DT', "EXT_RT"], [ _K_EXT_DT, _K_EXT_10_DT, _K_EXT_RT], _trg_mode = 0);

    if( _trg_mode == _K_EXT_RT ) 
    {


    	DevNodeCvt(_nid, _N_RT_TRIG_MODE, ['M1_N7_AMP', 'M1_N7_AMP_PH', 'RATIO_DOM_SEC', 'RATIO_DOM_SEC_PH','M0_N1_AMP', 'M0_N1_AMP_PH','M0_N7_AMP', 'M0_N7_AMP_PH'], [_K_M1_N7_AMP, _K_M1_N7_AMP_PH, _K_RATIO_DOM_SEC, _K_RATIO_DOM_SEC_PH, _K_M0_N1_AMP, _K_M0_N1_AMP_PH, _K_M0_N7_AMP, _K_M0_N7_AMP_PH], _trg_mode_rt = _K_DISABLE_RT);

	
    	_max_amp = if_error(data(DevNodeRef(_nid, _N_MAX_AMP)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing max amplitude level");
	    abort();
    	}

	write(*, "OOOOOOOOOOOOO");

    	_min_amp = if_error(data(DevNodeRef(_nid, _N_MIN_AMP)), _error = 1);
    	if(_error  )
    	{
	    DevLogErr(_nid, "Missing min amplitude level");
	    abort();
    	}


	if( _trg_mode_rt == _K_RATIO_DOM_SEC || _trg_mode_rt == _K_RATIO_DOM_SEC_PH )
	{
		_max_amp_rt = _max_amp * _max_amp;
		_min_amp_rt = _min_amp * _min_amp;
	}
	else
	{
		_max_amp_rt = _max_amp * _max_amp * 192.0;
		_min_amp_rt = _min_amp * _min_amp * 192.0;
	}


	_max_ph = 0;
 	_min_ph = 0;
	_max_ph_rt = 0;
 	_min_ph_rt = 0;

/*
Always evaluate phase also if trigger is set on aplitude limit only
	if( _trg_mode_rt == _K_M1_N7_AMP_PH || _trg_mode_rt == _K_RATIO_DOM_SEC_PH )
*/

	{


    		_max_ph = if_error(data(DevNodeRef(_nid, _N_MAX_PHASE)), _error = 1);
    		if(_error  )
    		{
	    		DevLogErr(_nid, "Missing max phase value");
	    		abort();
    		}

		_ph_deg =  mod( _max_ph + 180 + 37.5, 360);
/*
_min_ph_rt e' corretto in quanto la funzione di conversione inverte i limiti
*/
		_min_ph_rt = ( _ph_deg  <= 180 ) ? - _ph_deg  * $pi/180. : 2*$pi - _ph_deg  * $pi/180.; 


    		_min_ph = if_error(data(DevNodeRef(_nid, _N_MIN_PHASE)), _error = 1);
    		if(_error  )
    		{
	    		DevLogErr(_nid, "Missing min phase value");
	    		abort();
    		}

		_ph_deg =  mod( _min_ph + 180 + 37.5, 360);
/*
_max_ph_rt e' corretto in quanto la funzione di conversione inverte i limiti
*/
		_max_ph_rt = ( _ph_deg  <= 180 ) ? - _ph_deg  * $pi/180. : 2*$pi - _ph_deg  * $pi/180.; 

	
	}

    } 
    else 
    {
	write(*, "Disable real time trigger generation");

	_trg_mode_rt = _K_DISABLE_RT;
		
	_cmd = 'MdsConnect("'//_ip_rt//'")';
	execute(_cmd);

	_status = MdsValue('variables->setIntVariable("feedbackDfluTriggerMode", $)', long( _trg_mode_rt )) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set real time trigger mode");
		MdsDisconnect();
		abort();
	}
	MdsDisconnect();
    }


    if( _trg_mode == _K_EXT_RT )
    {


	_cmd = 'MdsConnect("'//_ip_rt//'")';

write(*, "max amp", _max_amp, _max_amp_rt);
write(*, "min amp", _min_amp, _min_amp_rt);
write(*, "max ph", _max_ph, _max_ph_rt);
write(*, "min ph", _min_ph, _min_ph_rt);
write(*, "trig mode RT", _trg_mode_rt );


	execute(_cmd);
		
	_status = MdsValue('variables->setFloatVariable("feedbackDfluMaxAmp", $)', float(_max_amp_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set N7 max amplitude value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setFloatVariable("feedbackDfluMinAmp", $)', float(_min_amp_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set N7 min amplitude value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setFloatVariable("feedbackDfluMaxPhase", $)', float(_max_ph_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set N7 max phase value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setFloatVariable("feedbackDfluMinPhase", $)', float(_min_ph_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set N7 max phase value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setFloatVariable("feedbackDfluMaxRatio", $)', float(_max_amp_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set max ratio value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setFloatVariable("feedbackDfluMinRatio", $)', float(_min_amp_rt)) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set min ratio value");
		MdsDisconnect();
		abort();
	}

	_status = MdsValue('variables->setIntVariable("feedbackDfluTriggerMode", $)', long( _trg_mode_rt )) ;
	if(_status == 0)
	{
		DevLogErr(_nid, "Error set real time trigger mode");
		MdsDisconnect();
		abort();
	}

	MdsDisconnect();
		
		
	}

	return (1);

}