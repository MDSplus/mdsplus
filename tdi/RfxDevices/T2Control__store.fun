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

	private _N_INPUT_1 = 21;
	private _N_OUTPUT_1 = 85;

write(*, 'T2Control store');

    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _cmd = 'MdsConnect("'//_vme_ip//'")';
    execute(_cmd);

	_trigger = data(DevNodeRef(_nid, _N_TRIG1_TIME));
	if(_trigger <= 0)
	{
		write(*, 'Invalid Trigger time ', _trigger);
		return(0);
	}

	_frequency = data(DevNodeRef(_nid, _N_FREQUENCY));
	if(_frequency <= 0)
	{
		write(*, 'Invalid frequency value ', _frequency);
		return(0);
	}

	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;

	_n_samples =  MdsValue('size(Feedback->getDacSignal(0))');

	write(*, 'Num recorded samples = ', _n_samples);
	_n_samples--;

    _clock = make_range(*,*, _period);
 

	/* Build signal dimension */
	_dim = make_dim(make_window(0, _n_samples, _trigger), _clock);
	for(_c = 0; _c < 64; _c++)
	{
			_sig_nid =  DevHead(_nid) + _N_INPUT_1  + _c;
			_data_command = 'Feedback->getAdcSignal:dsc('//_c//')';
			write(*, _data_command);
			_data = MdsValue(_data_command);

			_status = DevPutSignal(_sig_nid, 0, 10/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');

			}
	}

	for(_c = 0; _c < 32; _c++)
	{
			_sig_nid =  DevHead(_nid) + _N_OUTPUT_1  + _c;
			_data_command = 'Feedback->getDacSignal:dsc('//_c//')';
			write(*, _data_command);
			_data = MdsValue(_data_command);

			_status = DevPutSignal(_sig_nid, 0, 10/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file');

			}
	}


    MdsDisconnect();
    return (1);
}
