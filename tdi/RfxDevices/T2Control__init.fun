public fun T2Control__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_VME_IP = 2;
    private _N_FREQUENCY = 3;
    private _N_CALIBRATION = 4;
    private _N_INIT_CONTROL = 5;
    private _N_TRIG1_CONTROL = 6;
    private _N_TRIG2_CONTROL = 7;
    private _N_TRIG1_TIME = 8;
    private _N_DURATION = 9;
    private _N_TRIG2_TIME = 10;
    private _N_PAR1_NAME = 11;
    private _N_PAR1_VALUE = 12; 



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


	_duration = data(DevNodeRef(_nid, _N_DURATION));
	if(_duration <= 0)
	{
		write(*, 'Invalid duration value ', _duration);
		return(0);
	}

	write(*, 'Duration: ', _duration);
	_status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedbackPeriod', float(_duration));


	_control_idx = data(DevNodeRef(_nid, _N_INIT_CONTROL));
	if(_control_idx < 0 || _control_idx > 3)
	{
		write(*, 'Invalid Control Idx ', _control_idx);
		return(0);
	}

	write(*, 'Init Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackInitControl', long(_control_idx));


	_control_idx = data(DevNodeRef(_nid, _N_TRIG1_CONTROL));
	if(_control_idx < 0 || _control_idx > 3)
	{
		write(*, 'Invalid Control Idx ', _control_idx);
		return(0);
	}

	write(*, 'Trig 1 Control: ', _control_idx);
	_status = MdsValue('Feedback->setIntVariable($1, $2)', 'feedbackTrig1Control', long(_control_idx));


	_calibration = data(DevNodeRef(_nid, _N_CALIBRATION));

/*	write(*, 'Calibration: ', _calibration);*/
	_status = MdsValue('Feedback->setCalibration($1, $2)', float(_calibration), 64);
	
    for(_par = 0; _par < 11; _par++)
	{
		_par_name = data(DevNodeRef(_nid, _N_PAR1_NAME + _par * 2));
		_par_value = data(DevNodeRef(_nid, _N_PAR1_VALUE + _par * 2));
	    _status = MdsValue('Feedback->setFloatVariable($1, $2)', 'feedback'//_par_name, float(_par_value));
	}

	_status = MdsValue('Feedback->startFeedback()');

    MdsDisconnect();
    return (1);
}
