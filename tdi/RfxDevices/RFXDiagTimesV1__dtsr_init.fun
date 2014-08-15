public fun RFXDiagTimesV1__dtsr_init(as_is _nid, optional _method)
{

    private _N_HEAD      	= 0;
    private _N_TRIGGER 		= 2;
    private _N_TRIG_MODE 	= 3;
    private _N_RT_TRIG_MODE = 4;
    private _N_DELAY_PULSE 	= 5;
    private _N_AMP_MAX 		= 6;
    private _N_AMP_MIN 		= 7;
    private _N_PHASE_MAX 	= 8;
    private _N_PHASE_MIN 	= 9;
 
 
    private _K_COMMENT		= 1;
    private _K_IP_ADDRESS	= 2;
    private _K_PORT		= 3;
    private _K_RT_ADDRESS	= 4;
    private _K_TRIG_MODE	= 5;
    private _K_TRIG_SOURCE	= 6;
    private _K_RT_TRIG_MODE	= 7;
    private _K_NUM_PULSES	= 8;
    private _K_DELAY_PULSE	= 9;
    private _K_MAX_AMP		= 10;
    private _K_MIN_AMP		= 11;
    private _K_MAX_PHASE	= 12;
    private _K_MIN_PHASE	= 13;
    private _K_OSC		= 14;
    private _K_AMP		= 15;
    private _K_SLAB		= 16;
 
/*
	_diagTimesTag =  "\\DTSR_RAW::LASER_NDRT";
	_diagTimesTag =  "\\DTSR_RAW::ACQUISITION.NEODINIUM:LASER_NDRT";
*/


	_diagTimesTag =  "\\DTSE_RAW::ACQUISITION:LASER_YLF_RT";


    _diagTimesNid = if_error(getnci( _diagTimesTag, 'nid_number'), (DevLogErr(_nid, 'Cannot resolve decoder');abort();));

    _trigger    = if_error(data(DevNodeRef(_nid, _N_TRIGGER)), (DevLogErr(_nid, 'Cannot resolve trigger');abort();));
	 
    _trigMode   = if_error(data(DevNodeRef(_nid, _N_TRIG_MODE)), (DevLogErr(_nid, 'Cannot resolve trigger mode');abort();));
	DevPut(_diagTimesNid, _K_TRIG_MODE, _trigMode); 


	_rtTrigMode = if_error(data(DevNodeRef(_nid, _N_RT_TRIG_MODE)), (DevLogErr(_nid, 'Cannot resolve real time trigger mode');abort();));
	DevPut(_diagTimesNid, _K_RT_TRIG_MODE, _rtTrigMode); 

    _delayPulse = if_error(data(DevNodeRef(_nid, _N_DELAY_PULSE)), (DevLogErr(_nid, 'Cannot resolve delay pulse');abort();));
	DevPut(_diagTimesNid, _K_DELAY_PULSE, _delayPulse); 

 	_decChanNid = getnci(getnci(getnci( DevNodeRef( _diagTimesNid, _K_TRIG_SOURCE ), 'record'), 'parent'), 'nid_number' );

		
	if( _trigMode == 'EXT_RT')
	{

		TreeTurnOff( _decChanNid );
	
		_maxAmp = if_error(data(DevNodeRef(_nid, _N_AMP_MAX)), (DevLogErr(_nid, 'Cannot resolve max amplitude');abort();));
		DevPut(_diagTimesNid, _K_MAX_AMP, _maxAmp); 
	
		_minAmp = if_error(data(DevNodeRef(_nid, _N_AMP_MIN)), (DevLogErr(_nid, 'Cannot resolve min amplitude');abort();));
		DevPut(_diagTimesNid, _K_MIN_AMP, _minAmp); 
	
		if( index( _rtTrigMode, 'PH') >= 0 )
		{
			_maxPhase = if_error(data(DevNodeRef(_nid, _N_PHASE_MAX)), (DevLogErr(_nid, 'Cannot resolve max phase');abort();));
			DevPut(_diagTimesNid, _K_MAX_PHASE, _maxPhase); 
			
			_minPhase = if_error(data(DevNodeRef(_nid, _N_PHASE_MIN)), (DevLogErr(_nid, 'Cannot resolve min phase');abort();));
			DevPut(_diagTimesNid, _K_MIN_PHASE, _minPhase); 
			
		}
	}
	else
	{
		TreeTurnOn( _decChanNid );
	}

	
    return (1);
}
