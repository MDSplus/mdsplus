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
    private _N_ROUTINE_NAME = 15;
    private _N_N_ADC_IN = 16;
    private _N_N_DAC_OUT = 17;
    private _N_N_NET_IN = 18;
    private _N_N_NET_OUT = 19;

    private _N_PAR1_NAME = 438;
    private _N_PAR1_VALUE = 439; 

	private _N_ADC_IN_1 = 20;
	private _N_DAC_OUT_1 = 212;
	private _N_NET_IN_1 = 308;
	private _N_NET_OUT_1 = 372;


	private _MAX_CONTROLS = 6;
	private _NUM_PARAMETERS = 112;

write(*, 'RFXControl init');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);


	_frequency = data(DevNodeRef(_nid, _N_FREQUENCY));
	if(_frequency <= 0)
	{
		DevLogErr(_nid, 'Invalid frequency: '//_frequency);
		abort();
	}

	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackPeriod', float(_period));


	_sys_duration = data(DevNodeRef(_nid, _N_SYS_DURAT));
	if(_sys_duration <= 0)
	{
		DevLogErr(_nid, 'Invalid system duration value: '// _sys_duration);
		abort();
	}

	write(*, 'System duration: ', _sys_duration);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackSystemDuration', float(_sys_duration));

	_contr_duration = data(DevNodeRef(_nid, _N_CONTR_DURAT));
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
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackControlDuration', float(_contr_duration));


	_trig1_time = data(DevNodeRef(_nid, _N_TRIG1_TIME));
	_trig2_time = data(DevNodeRef(_nid, _N_TRIG2_TIME));
	if(_trig1_time >= _trig2_time)
	{
		DevLogErr(_nid, 'Trigger 1 time cannot be greater than trigger 2 time');
		abort();
	}

	write(*, 'Trigger 1 time: ', _trig1_time);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackTrig1Time', float(_trig1_time));
	write(*, 'Trigger 2 time: ', _trig2_time);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackTrig2Time', float(_trig2_time));



	_control_idx = data(DevNodeRef(_nid, _N_INIT_CONTROL));
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid init control:' // _control_idx);
		abort();

	}

	write(*, 'Init Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackInitControl', long(_control_idx));

	_start_time = data(DevNodeRef(_nid, _N_PRE_TIME));
 	write(*, 'Start sampling time: ', _start_time);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackPreTriggerSamples', long( - _start_time * _frequency));


	_end_time = data(DevNodeRef(_nid, _N_POST_TIME));
	if(_end_time <= _start_time)
	{
		DevLogErr(_nid, 'Post time must be greater than pre time');
		abort();
	}

	write(*, 'End sampling time: ', _end_time);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackPostTriggerSamples', long(_end_time * _frequency));



	_control_idx = data(DevNodeRef(_nid, _N_TRIG1_CONTROL));
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid Control 1:' // _control_idx);
		abort();
	}
	write(*, 'Trig1 Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackTrig1Control', long(_control_idx));

	_control_idx = data(DevNodeRef(_nid, _N_TRIG2_CONTROL));
	if(_control_idx < 0 || _control_idx > _MAX_CONTROLS)
	{
		DevLogErr(_nid, 'Invalid Control 2:' // _control_idx);
		abort();
	}

	write(*, 'Trig2 Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackTrig2Control', long(_control_idx));



	_in_calibration = data(DevNodeRef(_nid, _N_IN_CALIB));
/*	write(*, 'Calibration: ', _calibration);*/
	_status = MdsValue('Feedback->setInputCalibration($1, $2)', float(_in_calibration), 192);
	
	_out_calibration = data(DevNodeRef(_nid, _N_OUT_CALIB));
/*	write(*, 'Calibration: ', _calibration);*/
	_status = MdsValue('Feedback->setOutputCalibration($1, $2)', float(_out_calibration), 96);
	
    for(_par = 0; _par < _NUM_PARAMETERS; _par++)
	{

		_par_name = data(DevNodeRef(_nid, _N_PAR1_NAME + _par * 2));
	    if(DevIsOn(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2)))
		{
			_par_value = data(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2));
			if(size(_par_value) > 1)
				_status = MdsValue('Feedback->setFloatArray($1, $2, $3)', 'feedback'//_par_name, float(_par_value), size(_par_value));
			else
				_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedback'//_par_name, float(_par_value));
		}
		else
			_status = MdsValue('Feedback->setFloatVariable($1, 0.)', 'feedback'//_par_name//'Length');

	}

	_routine_name= data(DevNodeRef(_nid, _N_ROUTINE_NAME));
	_status = MdsValue('feedback->stop'// _routine_name // '()');
	_status = MdsValue('feedback->start'// _routine_name // '()');

    MdsDisconnect();
    return (1);
}
