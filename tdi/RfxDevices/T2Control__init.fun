public fun T2Control__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;
    private _N_FREQUENCY = 3;
    private _N_IN_CALIB = 4;
    private _N_OUT_CALIB = 5;
    private _N_INIT_CONTROL = 6;
    private _N_TRIG_CONTROL = 7;
    private _N_TRIG_TIME = 8;
    private _N_CONTR_DURAT = 9;
    private _N_SYS_DURAT = 10;
    private _N_PRE_TIME = 11;
    private _N_POST_TIME = 12;
    private _N_PAR1_NAME = 13;
    private _N_PAR1_VALUE = 14; 

    private _N_ZERO_START = 127;
    private _N_ZERO_END = 128;
    private _N_ZERO = 129;
    private _N_MAPPING_ID = 130;
    private _N_MAPPING = 131;



write(*, 'T2Control init');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);


	_frequency = data(DevNodeRef(_nid, _N_FREQUENCY));
	if(_frequency <= 0)
	{
		write(*, 'Invalid frequency value ', _frequency);
		return(0);
	}

	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackPeriod', float(_period));


	_sys_duration = data(DevNodeRef(_nid, _N_SYS_DURAT));
	if(_sys_duration <= 0)
	{
		write(*, 'Invalid system duration value ', _sys_duration);
		return(0);
	}

	write(*, 'System duration: ', _sys_duration);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackSystemDuration', float(_sys_duration));

	_contr_duration = data(DevNodeRef(_nid, _N_CONTR_DURAT));
	if(_contr_duration <= 0)
	{
		write(*, 'Invalid control duration value ', _duration);
		return(0);
	}
	if(_contr_duration > _sys_duration)
	{
		write(*, 'Control duration cannot be greater than system duration ', _contr_duration);
		return(0);
	}

	write(*, 'Control duration: ', _contr_duration);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackControlDuration', float(_contr_duration));


	_control_idx = data(DevNodeRef(_nid, _N_INIT_CONTROL));
	if(_control_idx < 0 || _control_idx > 9)
	{
		write(*, 'Invalid Control Idx ', _control_idx);
		return(0);
	}

	write(*, 'Init Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackInitControl', long(_control_idx));

	_start_time = data(DevNodeRef(_nid, _N_PRE_TIME));
 	write(*, 'Start sampling time: ', _start_time);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackPreTriggerSamples', long( - _start_time * _frequency));


	_end_time = data(DevNodeRef(_nid, _N_POST_TIME));
	if(_end_time <= _start_time)
	{
		write(*, 'Invalid end sampling time ', _end_time);
		return(0);
	}

	write(*, 'End sampling time: ', _end_time);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackPostTriggerSamples', long(_end_time * _frequency));


	_control_idx = data(DevNodeRef(_nid, _N_TRIG_CONTROL));
	if(_control_idx < 0 || _control_idx > 9)
	{
		write(*, 'Invalid Control Idx ', _control_idx);
		return(0);
	}

	write(*, 'Trig Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackTrig1Control', long(_control_idx));


	_in_calibration = data(DevNodeRef(_nid, _N_IN_CALIB));
/*	write(*, 'Calibration: ', _calibration);*/
	_status = MdsValue('Feedback->setInputCalibration($1, $2)', float(_in_calibration), 96);
	
	_out_calibration = data(DevNodeRef(_nid, _N_OUT_CALIB));
/*	write(*, 'Calibration: ', _calibration);*/
	_status = MdsValue('Feedback->setOutputCalibration($1, $2)', float(_out_calibration), 32);

	_mapping_id = data(DevNodeRef(_nid, _N_MAPPING_ID));
	write(*, 'Mapping_id: ', _mapping_id);
	_status = MdsValue('Feedback->setIntVariable("feedbackMappingId", $1)', long(_mapping_id));

	_zero_start = data(DevNodeRef(_nid, _N_ZERO_START));
	write(*, 'Zero Start: ', _zero_start);
	_status = MdsValue('Feedback->setFloatVariable("feedbackAutozeroStart", $1)', float(_zero_start));
	_zero_end = data(DevNodeRef(_nid, _N_ZERO_END));
	_status = MdsValue('Feedback->setFloatVariable("feedbackAutozeroEnd", $1)', float(_zero_end));
	write(*, 'Zero End: ', _zero_end);




	
    for(_par = 0; _par < 57; _par++)
	{
		_par_name = data(DevNodeRef(_nid, _N_PAR1_NAME + _par * 2));


		_par_value = data(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2));
		if(size(_par_value) > 1)
		    _status = MdsValue('Feedback->setFloatArray($1, $2, $3)', 'feedback'//_par_name, float(_par_value), size(_par_value));
		else
		    _status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedback'//_par_name, float(_par_value));
	}

	_status = MdsValue('Feedback->stopLocal()');
	_status = MdsValue('Feedback->startLocal()');

    MdsDisconnect();
    return (1);
}
