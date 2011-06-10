public fun RFXControl__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;
    private _N_FREQUENCY = 3;
    private _N_IN_CALIB = 4;
    private _N_OUT_CALIB = 5;
    private _N_INIT_CONTROL = 6;
    private _N_TRIG1_CONTROL = 7;
    private _N_TRIG2_CONTROL = 8;
    private _N_TRIG1_TIME = 9;
    private _N_TRIG2_TIME = 10;
    private _N_CONTR_DURAT = 11;
    private _N_SYS_DURAT = 12;
    private _N_PRE_TIME = 13;
    private _N_POST_TIME = 14;
    private _N_ZERO_START = 15;
    private _N_ZERO_END = 16;
    private _N_ZERO = 17;
    private _N_MAPPING_ID = 18;
    private _N_MAPPING = 19;
    private _N_RAMP_SLOPE = 20;
    private _N_RAMP_TRIGGER = 21;
    private _N_FEEDFORWARD = 22;
    private _N_ROUTINE_NAME = 23;
    private _N_N_ADC_IN = 24;
    private _N_N_DAC_OUT = 25;
    private _N_N_MODES = 27;
    private _N_MODEL_1 = 28;
    private _N_MODEL_2 = 29;
	
    private _N_ADC_IN_1 = 31;
    private _N_DAC_OUT_1 = 223;
/*    private _N_USER_1 = 1476;*/
    private _N_USER_1 = 1540;
    private _N_MODES_1 = 447;



    private _N_PAR1_NAME = 832;
    private _N_PAR1_VALUE = 833; 

 
	private _MAX_CONTROLS = 23;
/*	private _NUM_PARAMETERS = 321;*/
	private _NUM_PARAMETERS = 352;


    private _INVALID = 10E20;

write(*, 'RFXControl init');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);

	MdsDisconnect();
    _status = 0;
    _cmd = '_status = MdsConnect("'//_vme_ip//'")';
    execute(_cmd);

    if(_status == 0)
    {
	DevLogErr(_nid, 'Cannot connect to VME');
	abort();
    }

	_frequency = if_error(data(DevNodeRef(_nid, _N_FREQUENCY)), -1);
	if(_frequency <= 0)
	{
		DevLogErr(_nid, 'Invalid frequency: '//_frequency);
		abort();
	}

	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;
	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackPeriod', float(_period));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}

	_sys_duration = if_error(data(DevNodeRef(_nid, _N_SYS_DURAT)), -1);
	if(_sys_duration <= 0)
	{
		DevLogErr(_nid, 'Invalid system duration value: '// _sys_duration);
		abort();
	}

	write(*, 'System duration: ', _sys_duration);


	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackSystemDuration', float(_sys_duration));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}

	_contr_duration = if_error(data(DevNodeRef(_nid, _N_CONTR_DURAT)), -1);
	if(_contr_duration <= 0)
	{
		DevLogErr(_nid, 'Invalid control duration value: '//_duration);
		abort();
	}
	if(_contr_duration > _sys_duration)
	{
		DevLogErr(_nid, 'Control duration cannot be greater than system duration');
		abort();
	}

	write(*, 'Control duration: ', _contr_duration);
	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackControlDuration', float(_contr_duration));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_trig1_time = if_error(data(DevNodeRef(_nid, _N_TRIG1_TIME)), _INVALID);
	if(_trig1_time == _INVALID)
	{
		DevLogErr(_nid, 'Invalid Trigger 1');
		abort();
	}
	
write(*, _trig1_time);
       _trig2_time = if_error(data(DevNodeRef(_nid, _N_TRIG2_TIME)), _INVALID);
	if(_trig2_time == _INVALID)
	{
		DevLogErr(_nid, 'Invalid Trigger 2');
		abort();
	}
	if(_trig1_time >= _trig2_time)
	{
		DevLogErr(_nid, 'Trigger 1 time cannot be greater than trigger 2 time');
		abort();
	}

	write(*, 'Trigger 1 time: ', _trig1_time);
	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackTrig1Time', float(_trig1_time));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	write(*, 'Trigger 2 time: ', _trig2_time);
	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackTrig2Time', float(_trig2_time));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}


	_control_idx = if_error(data(DevNodeRef(_nid, _N_INIT_CONTROL)), -1);
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid init control:' // _control_idx);
		abort();

	}

	write(*, 'Init Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackInitControl', long(_control_idx));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_start_time = if_error(data(DevNodeRef(_nid, _N_PRE_TIME)), _INVALID);
	if(_start_time == _INVALID)
	{
		DevLogErr(_nid, 'Invalid Start time');
		abort();
	}
 	write(*, 'Start sampling time: ', _start_time);
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackPreTriggerSamples', long( - _start_time * _frequency));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}

	_end_time = if_error(data(DevNodeRef(_nid, _N_POST_TIME)), _INVALID);
	if(_end_time == _INVALID)
	{
		DevLogErr(_nid, 'Invalid End time');
		abort();
	}
	if(_end_time <= _start_time)
	{
		DevLogErr(_nid, 'Post time must be greater than pre time');
		abort();
	}

	write(*, 'End sampling time: ', _end_time);
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackPostTriggerSamples', long(_end_time * _frequency));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}


	_control_idx = if_error(data(DevNodeRef(_nid, _N_TRIG1_CONTROL)), -1);
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid Control 1:' // _control_idx);
		abort();
	}
	write(*, 'Trig1 Control: ', _control_idx);
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackTrig1Control', long(_control_idx));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_control_idx = if_error(data(DevNodeRef(_nid, _N_TRIG2_CONTROL)), -1);
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid Control 2:' // _control_idx);
		abort();
	}




	write(*, 'Trig2 Control: ', _control_idx);
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackTrig2Control', long(_control_idx));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}

	_data_valid = 1;

	_in_calibration = if_error(data(DevNodeRef(_nid, _N_IN_CALIB)), _data_valid = 0);
/*	write(*, 'Calibration: ', _calibration); */
	_status = MdsValue('support->setInputCalibration($1, $2)', float(_in_calibration), size(_in_calibration)/2);
/* 	_status = MdsValue('support->setInputCalibration($1, $2)', float(_in_calibration), 192); */
     if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	
	_out_calibration = if_error(data(DevNodeRef(_nid, _N_OUT_CALIB)), _data_valid = 0);
/*	write(*, 'Calibration: ', _calibration);*/

	if(!_data_valid)
	{
		DevLogErr(_nid, 'Invalid Calibration');
		abort();
	}
	

	_status = MdsValue('support->setOutputCalibration($1, $2)', float(_out_calibration), 96);
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}


	_zero_start = if_error(data(DevNodeRef(_nid, _N_ZERO_START)), _INVALID);
	if(_zero_start == _INVALID)
	{
		DevLogErr(_nid, 'Invalid zero start');
		abort();
	}
	


	write(*, 'Zero Start: ', _zero_start);
	_status = MdsValue('variables->setFloatVariable("feedbackAutozeroStart", $1)', float(_zero_start));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_zero_end = if_error(data(DevNodeRef(_nid, _N_ZERO_END)), _INVALID);
	if(_zero_end == _INVALID)
	{
		DevLogErr(_nid, 'Invalid zero end');
		abort();
	}
	_status = MdsValue('variables->setFloatVariable("feedbackAutozeroEnd", $1)', float(_zero_end));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	write(*, 'Zero End: ', _zero_end);


	_ramp_trigger = if_error(data(DevNodeRef(_nid, _N_RAMP_TRIGGER)), _INVALID);
	if(_ramp_trigger == _INVALID)
	{
		DevLogErr(_nid, 'Invalid ramp trigger');
		abort();
	}
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackRampDownTrigger', long(_ramp_trigger));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_ramp_slope = if_error(data(DevNodeRef(_nid, _N_RAMP_SLOPE)), _INVALID);
	if(_ramp_slope == _INVALID)
	{
		DevLogErr(_nid, 'Invalid ramp slope');
		abort();
	}
	_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedbackRampDownSlope', float(_ramp_slope));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
    DevNodeCvt(_nid, _N_FEEDFORWARD, ['DISABLED', 'ENABLED'], [0,1], _feedforward = 0);
	_status = MdsValue('variables->setIntVariable($1, $2)', 'feedbackFeedForward', long(_feedforward));
      if(_status == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	
    for(_par = 0; _par < _NUM_PARAMETERS; _par++)
	{

		_par_name = if_error(data(DevNodeRef(_nid, _N_PAR1_NAME + _par * 2)), "");
		/*write(*, _par_name, _par);*/
		if(_par_name == "")
			DevLogErr(_nid, 'Invalid name for parameter '// _par);
		else
		{
	    	    if(DevIsOn(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2)))
		    {
		    	_data_valid = 1;
			_par_value = if_error(data(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2)), _data_valid = 0);
			if(!_data_valid)
			{
			    DevLogErr(_nid, 'Invalid value for parameter '// _par);
			    abort();
			}
			
			
			/*write(*, _par_name, _par_value, _par);*/
			
			if(size(_par_value) > 1)
				_status = MdsValue('variables->setFloatArray($1, $2, $3)', 'feedback'//_par_name, float(_par_value), size(_par_value));
			else
				_status = MdsValue('variables->setFloatVariable($1, $2)', 'feedback'//_par_name, float(_par_value));
		    }
		    else
			_status = MdsValue('variables->setFloatVariable($1, 0.)', 'feedback'//_par_name//'Length');
      	    if(_status == *)
      	    {
	    		DevLogErr(_nid, 'Cannot communicate to VME');
	    		abort();
    			}
		    }
	}


	_routine_name= data(DevNodeRef(_nid, _N_ROUTINE_NAME));
	write(*, _routine_name);
	_status = MdsValue('eda3->stop'// _routine_name // '()');
	_status = MdsValue('eda3->start'// _routine_name // '()');
    MdsDisconnect();
    return (1);
}
