public fun T2Control__store(as_is _nid, optional _method)
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
	private _N_INPUT_1 = 71;
	private _N_OUTPUT_1 = 135;
	private _N_MODES_1 = 167;
write(*, 'T2Control store');
    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
	_trigger = data(DevNodeRef(_nid, _N_TRIG1_TIME));
	_frequency = data(DevNodeRef(_nid, _N_FREQUENCY));
	if(_frequency <= 0)
	{
		write(*, 'Invalid frequency value ', _frequency);
		return(0);
	}
	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;
	_n_samples =  MdsValue('size(Feedback->getDacSignal:dsc(0))');
	_n_pretrigger =  MdsValue('Feedback->getPreTriggerSamples()');
	write(*, 'Num recorded samples = ', _n_samples);
	_n_samples--;
    _clock = make_range(*,*, _period);

 	/* Build signal dimension */
	_dim = make_dim(make_window(0, _n_samples, _trigger - _n_pretrigger * _period), _clock);
	for(_c = 0; _c < 64; _c++)
	{
			_sig_nid =  DevHead(_nid) + _N_INPUT_1  + _c;
			_data = MdsValue('Feedback->getAdcSignal:dsc($1, $2)', _c / 64, mod(_c,64));
			_status = DevPutSignal(_sig_nid, 0, 10/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				write(*, 'Error writing data in pulse file for channel ', _c);
				DevLogErr(_nid, 'Error writing data in pulse file ');

			}
	}

	for(_c = 0; _c < 32; _c++)
	{
			_sig_nid =  DevHead(_nid) + _N_OUTPUT_1  + _c;
			_data = MdsValue( 'Feedback->getDacSignal:dsc($1)', _c);

			_status = DevPutSignal(_sig_nid, -2048, 5/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');

			}
	}
	_n_samples =  MdsValue('size(Feedback->getMode:dsc(0, 1))');
	_dim = make_dim(make_window(0, _n_samples, _trigger), _clock);
	for(_c = 0; _c < 32; _c++)
	{
		_data = MdsValue( 'Feedback->getMode:dsc($1, 1)', _c);
		_status = DevPut(_nid, _N_MODES_1  + 2 * _c, _data);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing modes in pulse file');

		}
		_data = MdsValue( 'Feedback->getMode:dsc($1, 0)', _c);
		_status = DevPut(_nid, _N_MODES_1  + 2 * _c + 1, _data);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing mods in pulse file:'//getmsg(_status));
		}
	}
    MdsDisconnect();
    return (1);
}
