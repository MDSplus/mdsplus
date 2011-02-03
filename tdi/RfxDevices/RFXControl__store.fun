public fun RFXControl__store(as_is _nid, optional _method)
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

 
	private _MAX_CONTROLS = 22;
/*	private _NUM_PARAMETERS = 321;*/
	private _NUM_PARAMETERS = 352;



    _vme_ip = DevNodeRef(_nid, _N_VME_IP);
    _status = 0;	
    _cmd = '_status = MdsConnect("'//_vme_ip//'")';
    execute(_cmd);
    if(_status == 0)
    {
	  DevLogErr(_nid, 'Cannot connect to VME');
	  abort();
    }

	_trigger = data(DevNodeRef(_nid, _N_TRIG1_TIME));
	_frequency = data(DevNodeRef(_nid, _N_FREQUENCY));
	if(_frequency <= 0)
	{
		write(*, 'Invalid frequency value ', _frequency);
		return(0);
	}
	write(*, 'Frequency: ', _frequency);
	_period = 1. / _frequency;
	_n_samples =  MdsValue('size(Feedback->getDacSignal:dsc(0,0))');
      if(_n_samples == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME : read _n_samples');
	    abort();
    	}
	_n_pretrigger =  MdsValue('Feedback->getPreTriggerSamples()');
      if(_n_pretrigger == *)
      {
	    DevLogErr(_nid, 'Cannot communicate to VME : read _n_pretrigger');
	    abort();
    	}

	write(*, 'Num recorded samples = ', _n_samples);
	_n_samples--;
    _clock = make_range(*,*, _period);

 	/* Build signal dimension */
	_dim = make_dim(make_window(0, _n_samples, _trigger - _n_pretrigger * _period), _clock);
/*	_dim = MdsValue('Feedback->getTimebase:dsc()'); */

	/* Read number of signals */
	_num_adc_in = data(DevNodeRef(_nid, _N_N_ADC_IN));
	write(*, 'Num ADC in: ', _num_adc_in);
	_num_dac_out = data(DevNodeRef(_nid, _N_N_DAC_OUT));
	write(*, 'Num DAC out: ', _num_dac_out);
	_num_modes = data(DevNodeRef(_nid, _N_N_MODES));
	write(*, 'Num MODES: ', _num_modes);

	for(_c = 0; _c < _num_adc_in; _c++)
	{

/*
write(*, _c);
*/
			_sig_nid =  DevHead(_nid) + _N_ADC_IN_1  + _c;
			_data = MdsValue('Feedback->getAdcSignal:dsc($1, $2)', _c / 64, mod(_c,64));
      		if(size(_data) == 0)
      		{
	    			DevLogErr(_nid, 'Cannot communicate to VME');
	    			abort();
    			}

			_status = DevPutSignal(_sig_nid, 0, 10/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				write(*, 'Error writing data in pulse file for channel ', _c);
				DevLogErr(_nid, 'Error writing data in pulse file ');

			}
	}
	
	for(_c = 0; _c < _num_dac_out; _c++)
	{
/*
write(*, _c);
*/
			_sig_nid =  DevHead(_nid) + _N_DAC_OUT_1  + _c;
			_data = MdsValue( 'Feedback->getDacSignal:dsc($1, $2)', _c/32, mod(_c, 32));
      		if(size(_data) == 0)
      		{
	    			DevLogErr(_nid, 'Cannot communicate to VME');
	    			abort();
			}
			
			if(size(_data) <= 2)
			{
				DevLogErr(_nid, 'VME not triggered' // _status);
				abort();	
			}
			
			_status = DevPutSignal(_sig_nid, -2048, 10/2048., word(_data), 0, _n_samples, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing data in pulse file' // _status);

			}
			
	}

	for(_c = 0; _c < _num_modes; _c++)
	{
/*	
write(*, _c);	
*/	
		_data = MdsValue( 'Feedback->getRfxMode:dsc($1, 1)', _c);
      	if(size(_data) == 0)
      	{
	    		DevLogErr(_nid, 'Cannot communicate to VME');
	    		abort();
    		}
		_sig_nid =  DevHead(_nid) + _N_MODES_1  + 2 * _c;
/*
write(*, "Write");
*/
		_status = DevPutSignal(_sig_nid, 0, 1., _data, 0, _n_samples, _dim);

		if(! _status)
		{
			DevLogErr(_nid, 'Error writing modes in pulse file');

		}
/*		
write(*, "Read");
*/
		_data = MdsValue( 'Feedback->getRfxMode:dsc($1, 0)', _c);
      	if(size(_data) == 0)
      	{
	    		DevLogErr(_nid, 'Cannot communicate to VME');
	    		abort();
    		}
		_sig_nid =  DevHead(_nid) + _N_MODES_1  + 2 * _c + 1;
/*		
write(*, "Write");
*/		
		_status = DevPutSignal(_sig_nid, 0, 1., _data, 0, _n_samples, _dim);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing mods in pulse file:'//getmsg(_status));
		}
	}
/*
write(*, "Fine modes");
*/
	 _num_user_signals = MdsValue('Feedback->getNumUserSignals()');
      if(size(_num_user_signals) == 0)
      {
	    	DevLogErr(_nid, 'Cannot communicate to VME');
	    	abort();
    	}
	write(*, 'Num User Signals: ', _num_user_signals);
	if( _num_user_signals > 256 ) _num_user_signals = 256;
	
	for(_c = 0; _c < _num_user_signals; _c++)
	{
/*
write(*, _c);
*/
			_sig_nid =  DevHead(_nid) + _N_USER_1  + _c;
			_data = MdsValue('Feedback->getUserSignal:dsc($1)', _c);
      		if(size(_data) == 0)
      		{
	    			DevLogErr(_nid, 'Cannot communicate to VME');
	    			abort();
    			}
			_status = DevPutSignal(_sig_nid, 0, 1., _data, 0, _n_samples, _dim);
			if(! _status)
			{
				write(*, 'Error writing data in pulse file for channel ', _c);
				DevLogErr(_nid, 'Error writing data in pulse file ');

			}
	}	


/* NET_IN and NET_OUT not yet implemented */ 
	_zero = MdsValue('Feedback->getRfxZero:dsc()');
      if(size(_zero) == 0)
      {
		DevLogErr(_nid, 'Cannot communicate to VME');
	    	abort();
    	}
	_status = DevPut(_nid, _N_ZERO, _zero);
	if(! _status)
	{
		DevLogErr(_nid, 'Error writing offset values in pulse file:'//getmsg(_status));
	}


    MdsDisconnect();
    return (1);
}