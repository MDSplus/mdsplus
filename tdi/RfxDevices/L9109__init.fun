public fun L9109__init(as_is _nid, optional _method)
{

	private _N_GENERAL_GPIB_ADDR = 1;


    	private	_N_CLOCK_SOURCE = 4;
	private	_N_CLOCK_FREQUENCY = 5;
	private	_N_CLOCK_CLOCK_SOURCE = 6;


    	private	_N_TRIGGER_SOURCE = 8;
	private	_N_TRIGGER_LEVEL = 9;
	private	_N_TRIGGER_SLOPE = 10;
	private	_N_TRIGGER_TRIG_SOURCE = 11;


	private	_N_CH01 = 12;
	private	_N_CH01_AMPLITUDE = 13;
	private	_N_CH01_OFFSET = 14;
	private	_N_CH01_ZERO_REF = 15;
	private	_N_CH01_FILTER = 16;


	private	_N_CH01_WAVEFORM_POINTS = 18;
	private	_N_CH01_WAVEFORM_REPETITIONS = 19;
	private	_N_CH01_WAVEFORM_DELAY = 20;


	private	_N_CH02 = 21;
	private	_N_CH02_AMPLITUDE = 22;
	private	_N_CH02_OFFSET = 23;
	private	_N_CH02_ZERO_REF = 24;
	private	_N_CH02_FILTER = 25;

	private	_N_CH02_TRIGGER01 = 26;
	private	_N_CH02_TRIGGER01_DELAY = 27;
	private	_N_CH02_TRIGGER01_DURATION = 28;


	private	_N_CH02_TRIGGER02 = 30;
	private	_N_CH02_TRIGGER02_DELAY = 31;
	private	_N_CH02_TRIGGER02_DURATION = 32;



	private _DEBUG = 0;
	
	private _ON = 1;
	
	private _OFF = 0;



	/* Ricavo il nid della testa del conglomerato */

	_nid_head = getnci(_nid, 'nid_number');

/******************* CLOCK ******************/

	_status = 1 ;
	_clock_source = if_error(data(DevNodeRef(_nid, _N_CLOCK_SOURCE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CLOCK SOURCE');

		abort();
	}

	_status = 1 ;
	_clock_frequency = if_error(data(DevNodeRef(_nid, _N_CLOCK_FREQUENCY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CLOCK FREQUENCY');

		abort();
	}



/******************* TRIGGER ******************/

	DevNodeCvt(_nid, _N_TRIGGER_SOURCE, [ 'MANUAL', 'BUS', 'EXTERNAL', 'EXTERNAL + BUS' ], [ 1, 2, 3, 4 ], _trigger_source = 4);

	_status = 1 ;
	_trigger_slope = if_error(data(DevNodeRef(_nid, _N_TRIGGER_SLOPE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on TRIGGER SLOPE');

		abort();
	}

	_status = 1 ;
	_trigger_level = if_error(data(DevNodeRef(_nid, _N_TRIGGER_LEVEL)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on TRIGGER LEVEL');

		abort();
	}



/******************* CH01 ******************/

	_ch01_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_CH01)))
	{
		_ch01_state = _ON;
	}


	_status = 1 ;
	_ch01_amplitude = if_error(data(DevNodeRef(_nid, _N_CH01_AMPLITUDE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 AMPLITUDE');

		abort();
	}

	_status = 1 ;
	_ch01_offset = if_error(data(DevNodeRef(_nid, _N_CH01_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 OFFSET');

		abort();
	}

	_status = 1 ;
	_ch01_zero_ref = if_error(data(DevNodeRef(_nid, _N_CH01_ZERO_REF)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 ZERO_REF');

		abort();
	}

	DevNodeCvt(_nid, _N_CH01_FILTER, [ 'OFF', '1MHz', '3MHz', '10MHz', '30MHz', '100MHz' ], [ '0MHz', '1MHz', '3MHz', '10MHz', '30MHz', '100MHz' ], _ch01_filter = 1);

	_status = 1 ;
	_ch01_waveform_points = if_error(data(DevNodeRef(_nid, _N_CH01_WAVEFORM_POINTS)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 WAVEFORM POINTS');

		abort();
	}

	_ch01_waveform_points = BYTE_UNSIGNED(_ch01_waveform_points);
	
	_ch01_waveform_n_points = size(_ch01_waveform_points);
	
	_status = 1 ;
	_ch01_waveform_repetitions = if_error(data(DevNodeRef(_nid, _N_CH01_WAVEFORM_REPETITIONS)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 WAVEFORM REPETITIONS');

		abort();
	}

	_status = 1 ;
	_ch01_waveform_delay = if_error(data(DevNodeRef(_nid, _N_CH01_WAVEFORM_DELAY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH01 WAVEFORM DELAY');

		abort();
	}




/******************* CH02 ******************/

	_ch02_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_CH02)))
	{
		_ch02_state = _ON;
	}


	_status = 1 ;
	_ch02_amplitude = if_error(data(DevNodeRef(_nid, _N_CH02_AMPLITUDE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 AMPLITUDE');

		abort();
	}

	_status = 1 ;
	_ch02_offset = if_error(data(DevNodeRef(_nid, _N_CH02_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 OFFSET');

		abort();
	}

	_status = 1 ;
	_ch02_zero_ref = if_error(data(DevNodeRef(_nid, _N_CH02_ZERO_REF)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 ZERO_REF');

		abort();
	}

	DevNodeCvt(_nid, _N_CH01_FILTER, [ 'OFF', '1MHz', '3MHz', '10MHz', '30MHz', '100MHz' ], [ '0MHz', '1MHz', '3MHz', '10MHz', '30MHz', '100MHz' ], _ch02_filter = 1);



	_ch02_trigger01_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_CH02_TRIGGER01)))
	{
		_ch02_trigger01_state = _ON;
	}


	_status = 1 ;
	_ch02_trigger01_delay = if_error(data(DevNodeRef(_nid, _N_CH02_TRIGGER01_DELAY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 TRIGGER01 DELAY');

		abort();
	}

	_status = 1 ;
	_ch02_trigger01_duration = if_error(data(DevNodeRef(_nid, _N_CH02_TRIGGER01_DURATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 TRIGGER01 DURATION');

		abort();
	}


	_ch02_trigger02_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_CH02_TRIGGER02)))
	{
		_ch02_trigger02_state = _ON;
	}


	_status = 1 ;
	_ch02_trigger02_delay = if_error(data(DevNodeRef(_nid, _N_CH02_TRIGGER02_DELAY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 TRIGGER02 DELAY');

		abort();
	}

	_status = 1 ;
	_ch02_trigger02_duration = if_error(data(DevNodeRef(_nid, _N_CH02_TRIGGER02_DURATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on CH02 TRIGGER02 DURATION');

		abort();
	}


	if(1 == _DEBUG)
	{
		write(*, 'SENT PARAMETERS');
		write(*, '_clock_source: ', _clock_source);
		write(*, '_clock_frequency: ', _clock_frequency);
		write(*, '_trigger_source: ', _trigger_source);
		write(*, '_trigger_slope: ', _trigger_slope);
		write(*, '_trigger_level: ', _trigger_level);
		write(*, '_ch01_state: ', _ch01_state);
		write(*, '_ch01_amplitude: ', _ch01_amplitude);
		write(*, '_ch01_offset: ', _ch01_offset);
		write(*, '_ch01_zero_ref: ', _ch01_zero_ref);
		write(*, '_ch01_filter: ', _ch01_filter);
		write(*, '_ch01_waveform_points: ', _ch01_waveform_points);
		write(*, '_ch01_waveform_n_points: ', _ch01_waveform_n_points);
		write(*, '_ch01_waveform_repetitions: ', _ch01_waveform_repetitions);
		write(*, '_ch01_waveform_delay: ', _ch01_waveform_delay);
		write(*, '_ch02_state: ', _ch02_state);
		write(*, '_ch02_amplitude: ', _ch02_amplitude);
		write(*, '_ch02_offset: ', _ch02_offset);
		write(*, '_ch02_zero_ref: ', _ch02_zero_ref);
		write(*, '_ch02_filter: ', _ch02_filter);
		write(*, '_ch02_trigger01_state: ', _ch02_trigger01_state);
		write(*, '_ch02_trigger01_delay: ', _ch02_trigger01_delay);
		write(*, '_ch02_trigger01_duration: ', _ch02_trigger01_duration);
		write(*, '_ch02_trigger02_state: ', _ch02_trigger02_state);
		write(*, '_ch02_trigger02_delay: ', _ch02_trigger02_delay);
		write(*, '_ch02_trigger02_duration: ', _ch02_trigger02_duration);
	}
	
	
	L9109->SetClock(_clock_source, float(_clock_frequency));
	L9109->SetTrigger(val(_trigger_source), _trigger_slope, float(_trigger_level));
	L9109->SetCH01(val(_ch01_state),float(_ch01_amplitude),float(_ch01_offset),float(_ch01_zero_ref),_ch01_filter);
	L9109->SetCH01Waveform(ref(_ch01_waveform_points),val(_ch01_waveform_n_points),val(_ch01_waveform_repetitions),float(_ch01_waveform_delay));

	L9109->SetCH02(val(_ch02_state),float(_ch02_amplitude),float(_ch02_offset),float(_ch02_zero_ref),_ch02_filter);
	L9109->SetCH02Trigger01(val(_ch02_trigger01_state),float(_ch02_trigger01_delay),float(_ch02_trigger01_duration));
	L9109->SetCH02Trigger02(val(_ch02_trigger02_state),float(_ch02_trigger02_delay),float(_ch02_trigger02_duration));

	L9109->INIT();



	return ( 1 );
}

