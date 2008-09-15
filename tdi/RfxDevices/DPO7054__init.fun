public fun DPO7054__init(as_is _nid, optional _method)
{

	private _N_COMMON_HORIZ_ACQ_SAMPLE_RATE = 4;

	private _N_COMMON_TRIGGER_SOURCE = 6;
	private _N_COMMON_TRIGGER_COUPLING = 7;
	private _N_COMMON_TRIGGER_SLOPE = 8;
	private _N_COMMON_TRIGGER_LEVEL = 9;
	private _N_COMMON_TRIGGER_HOLDOFF = 10;
	private _N_COMMON_TRIGGER_TRIG_SOURCE = 11;

	private _N_COMMON_VERTICAL_CH01 = 13;
	private _N_COMMON_VERTICAL_CH01_COUPLING = 14;
	private _N_COMMON_VERTICAL_CH01_TERMINATION = 15;
	private _N_COMMON_VERTICAL_CH01_POSITION = 16;
	private _N_COMMON_VERTICAL_CH01_OFFSET = 17;
	private _N_COMMON_VERTICAL_CH01_BW_LIMIT = 18;

	private _N_COMMON_VERTICAL_CH02 = 19;
	private _N_COMMON_VERTICAL_CH02_COUPLING = 20;
	private _N_COMMON_VERTICAL_CH02_TERMINATION = 21;
	private _N_COMMON_VERTICAL_CH02_POSITION = 22;
	private _N_COMMON_VERTICAL_CH02_OFFSET = 23;
	private _N_COMMON_VERTICAL_CH02_BW_LIMIT = 24;

	private _N_COMMON_VERTICAL_CH03 = 25;
	private _N_COMMON_VERTICAL_CH03_COUPLING = 26;
	private _N_COMMON_VERTICAL_CH03_TERMINATION = 27;
	private _N_COMMON_VERTICAL_CH03_POSITION = 28;
	private _N_COMMON_VERTICAL_CH03_OFFSET = 29;
	private _N_COMMON_VERTICAL_CH03_BW_LIMIT = 30;

	private _N_COMMON_VERTICAL_CH04 = 31;
	private _N_COMMON_VERTICAL_CH04_COUPLING = 32;
	private _N_COMMON_VERTICAL_CH04_TERMINATION = 33;
	private _N_COMMON_VERTICAL_CH04_POSITION = 34;
	private _N_COMMON_VERTICAL_CH04_OFFSET = 35;
	private _N_COMMON_VERTICAL_CH04_BW_LIMIT = 36;

	private _N_WINDOW01_HORIZ_ACQ_SCALE = 39;
	private _N_WINDOW01_HORIZ_ACQ_DELAY = 40;
	private _N_WINDOW01_HORIZ_ACQ_POSITION = 41;

	private _N_WINDOW01_VERTICAL_CH01_SCALE = 44;
	private _N_WINDOW01_VERTICAL_CH01_DATA = 45;

	private _N_WINDOW01_VERTICAL_CH02_SCALE = 47;
	private _N_WINDOW01_VERTICAL_CH02_DATA = 48;

	private _N_WINDOW01_VERTICAL_CH03_SCALE = 50;
	private _N_WINDOW01_VERTICAL_CH03_DATA = 51;

	private _N_WINDOW01_VERTICAL_CH04_SCALE = 53;
	private _N_WINDOW01_VERTICAL_CH04_DATA = 54;

	private _N_WINDOW02 = 55;
	private _N_WINDOW02_HORIZ_ACQ_SCALE = 57;
	private _N_WINDOW02_HORIZ_ACQ_DELAY = 58;
	private _N_WINDOW02_HORIZ_ACQ_POSITION = 59;

	private _N_WINDOW02_VERTICAL_CH01_SCALE = 62;
	private _N_WINDOW02_VERTICAL_CH01_DATA = 63;

	private _N_WINDOW02_VERTICAL_CH02_SCALE = 65;
	private _N_WINDOW02_VERTICAL_CH02_DATA = 66;

	private _N_WINDOW02_VERTICAL_CH03_SCALE = 68;
	private _N_WINDOW02_VERTICAL_CH03_DATA = 69;

	private _N_WINDOW02_VERTICAL_CH04_SCALE = 71;
	private _N_WINDOW02_VERTICAL_CH04_DATA = 72;



	private _DEBUG = 1;
	
	private _ON = 1;
	
	private _OFF = 0;

	private _MAX_RECORD_LENGTH = 200E6;




	/* Ricavo il nid della testa del conglomerato */

	_nid_head = getnci(_nid, 'nid_number');

/******************* COMMON HORIZONTAL - ACQ ******************/

	_status = 1 ;
	_common_horiz_acq_sample_rate = if_error(data(DevNodeRef(_nid, _N_COMMON_HORIZ_ACQ_SAMPLE_RATE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON HORIZ_ACQ SAMPLE_RATE');

		abort();
	}

/******************* COMMON TRIGGER ******************/

	_status = 1 ;
	_common_trigger_source = if_error(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_SOURCE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON TRIGGER SOURCE');

		abort();
	}

	_status = 1 ;
	_common_trigger_coupling = if_error(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_COUPLING)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON TRIGGER COUPLING');

		abort();
	}

	_status = 1 ;
	_common_trigger_slope = if_error(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_SLOPE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON TRIGGER SLOPE');

		abort();
	}

	_status = 1 ;
	_common_trigger_level = if_error(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_LEVEL)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON TRIGGER LEVEL');

		abort();
	}

	_status = 1 ;
	_common_trigger_holdoff = if_error(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_HOLDOFF)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON TRIGGER HOLDOFF');

		abort();
	}

/******************* COMMON VERTICAL CH01 ******************/

	_status = 1 ;
	_common_vertical_ch01_coupling = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01_COUPLING)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH01 COUPLING');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch01_termination = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01_TERMINATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH01 TERMINATION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch01_position = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH01 POSITION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch01_offset = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH01 OFFSET');

		abort();
	}
	
	DevNodeCvt(_nid, _N_COMMON_VERTICAL_CH01_BW_LIMIT, [ 'FULL', '20MHz', '250MHz', '500MHz' ], [ 'FULL', 'TWENTY', 'TWOFIFTY', 'FIVE' ], _common_vertical_ch01_bw_limit = 'FULL');


/******************* COMMON VERTICAL CH02 ******************/

	_status = 1 ;
	_common_vertical_ch02_coupling = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02_COUPLING)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH02 COUPLING');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch02_termination = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02_TERMINATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH02 TERMINATION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch02_position = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH02 POSITION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch02_offset = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH02 OFFSET');

		abort();
	}

	DevNodeCvt(_nid, _N_COMMON_VERTICAL_CH02_BW_LIMIT, [ 'FULL', '20MHz', '250MHz', '500MHz' ], [ 'FULL', 'TWENTY', 'TWOFIFTY', 'FIVE' ], _common_vertical_ch02_bw_limit = 'FULL');

/******************* COMMON VERTICAL CH03 ******************/

	_status = 1 ;
	_common_vertical_ch03_coupling = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03_COUPLING)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH03 COUPLING');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch03_termination = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03_TERMINATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH03 TERMINATION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch03_position = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH03 POSITION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch03_offset = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH03 OFFSET');

		abort();
	}

	DevNodeCvt(_nid, _N_COMMON_VERTICAL_CH03_BW_LIMIT, [ 'FULL', '20MHz', '250MHz', '500MHz' ], [ 'FULL', 'TWENTY', 'TWOFIFTY', 'FIVE' ], _common_vertical_ch03_bw_limit = 'FULL');

/******************* COMMON VERTICAL CH04 ******************/

	_status = 1 ;
	_common_vertical_ch04_coupling = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04_COUPLING)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH04 COUPLING');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch04_termination = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04_TERMINATION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH04 TERMINATION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch04_position = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH04 POSITION');

		abort();
	}

	_status = 1 ;
	_common_vertical_ch04_offset = if_error(data(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04_OFFSET)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON VERTICAL CH04 OFFSET');

		abort();
	}

	DevNodeCvt(_nid, _N_COMMON_VERTICAL_CH04_BW_LIMIT, [ 'FULL', '20MHz', '250MHz', '500MHz' ], [ 'FULL', 'TWENTY', 'TWOFIFTY', 'FIVE' ], _common_vertical_ch04_bw_limit = 'FULL');
	
/******************* WINDOW01 HORIZONTAL - ACQ ******************/

	_status = 1 ;
	_window01_horiz_acq_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW01_HORIZ_ACQ_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 HORIZ_ACQ SCALE');

		abort();
	}	
	
	_status = 1 ;
	_window01_horiz_acq_delay = if_error(data(DevNodeRef(_nid, _N_WINDOW01_HORIZ_ACQ_DELAY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 HORIZ_ACQ DELAY');

		abort();
	}
	
	_status = 1 ;
	_window01_horiz_acq_position = if_error(data(DevNodeRef(_nid, _N_WINDOW01_HORIZ_ACQ_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 HORIZ_ACQ POSITION');

		abort();
	}		

/******************* WINDOW01 VERTICAL CH01 ******************/

	_status = 1 ;
	_window01_vertical_ch01_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW01_VERTICAL_CH01_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 VERTICAL CH01 SCALE');

		abort();
	}
	
/******************* WINDOW01 VERTICAL CH02 ******************/

	_status = 1 ;
	_window01_vertical_ch02_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW01_VERTICAL_CH02_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 VERTICAL CH02 SCALE');

		abort();
	}

/******************* WINDOW01 VERTICAL CH03 ******************/

	_status = 1 ;
	_window01_vertical_ch03_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW01_VERTICAL_CH03_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 VERTICAL CH03 SCALE');

		abort();
	}
	
/******************* WINDOW01 VERTICAL CH04 ******************/

	_status = 1 ;
	_window01_vertical_ch04_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW01_VERTICAL_CH04_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW01 VERTICAL CH04 SCALE');

		abort();
	}
	
/******************* WINDOW02 HORIZONTAL - ACQ ******************/

	_status = 1 ;
	_window02_horiz_acq_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW02_HORIZ_ACQ_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 HORIZ_ACQ SCALE');

		abort();
	}	
	
	_status = 1 ;
	_window02_horiz_acq_delay = if_error(data(DevNodeRef(_nid, _N_WINDOW02_HORIZ_ACQ_DELAY)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 HORIZ_ACQ DELAY');

		abort();
	}
	
	_status = 1 ;
	_window02_horiz_acq_position = if_error(data(DevNodeRef(_nid, _N_WINDOW02_HORIZ_ACQ_POSITION)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 HORIZ_ACQ POSITION');

		abort();
	}		

/******************* WINDOW02 VERTICAL CH01 ******************/

	_status = 1 ;
	_window02_vertical_ch01_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW02_VERTICAL_CH01_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 VERTICAL CH01 SCALE');

		abort();
	}
	
/******************* WINDOW02 VERTICAL CH02 ******************/

	_status = 1 ;
	_window02_vertical_ch02_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW02_VERTICAL_CH02_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 VERTICAL CH02 SCALE');

		abort();
	}

/******************* WINDOW02 VERTICAL CH03 ******************/

	_status = 1 ;
	_window02_vertical_ch03_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW02_VERTICAL_CH03_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 VERTICAL CH03 SCALE');

		abort();
	}
	
/******************* WINDOW02 VERTICAL CH04 ******************/

	_status = 1 ;
	_window02_vertical_ch04_scale = if_error(data(DevNodeRef(_nid, _N_WINDOW02_VERTICAL_CH04_SCALE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WINDOW02 VERTICAL CH04 SCALE');

		abort();
	}


	_n_channels = 0;
	_ch01_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01)))
	{
		++ _n_channels;
		_ch01_state = _ON;
	}
	_ch02_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02)))
	{
		++ _n_channels;
		_ch02_state = _ON;
	}
	_ch03_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03)))
	{
		++ _n_channels;
		_ch03_state = _ON;
	}
	_ch04_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04)))
	{
		++ _n_channels;
		_ch04_state = _ON;
	}

	_window02_state = _OFF;
	if(DevIsOn(DevNodeRef(_nid, _N_WINDOW02)))
	{
		_window02_state = _ON;
	}
	


	

	_window01_record_length = long((ft_float(10.) * ft_float(_window01_horiz_acq_scale) * ft_float(_common_horiz_acq_sample_rate)) + 0.5);


	_window02_record_length = long((ft_float(10.) * ft_float(_window02_horiz_acq_scale) * ft_float(_common_horiz_acq_sample_rate)) + 0.5);


	write(*,'_window01_record_length',_window01_record_length);

	write(*,'_window02_record_length',_window02_record_length);

	
	if(1 == _DEBUG)
	{
		write(*, 'SENT PARAMETERS');
		write(*, '_n_channels: ', _n_channels);
		write(*, '_common_horiz_acq_sample_rate: ', _common_horiz_acq_sample_rate);
		
		write(*, '_common_trigger_source: ', _common_trigger_source);
		write(*, '_common_trigger_coupling: ', _common_trigger_coupling);
		write(*, '_common_trigger_slope: ', _common_trigger_slope);
		write(*, '_common_trigger_level: ', _common_trigger_level);
		write(*, '_common_trigger_holdoff: ', _common_trigger_holdoff);
		
		write(*, '_ch01_state: ', _ch01_state);
		write(*, '_common_vertical_ch01_coupling: ', _common_vertical_ch01_coupling);
		write(*, '_common_vertical_ch01_termination: ', _common_vertical_ch01_termination);
		write(*, '_common_vertical_ch01_position: ', _common_vertical_ch01_position);
		write(*, '_common_vertical_ch01_offset: ', _common_vertical_ch01_offset);
		write(*, '_common_vertical_ch01_bw_limit: ', _common_vertical_ch01_bw_limit);

		write(*, '_ch02_state: ', _ch02_state);
		write(*, '_common_vertical_ch02_coupling: ', _common_vertical_ch02_coupling);
		write(*, '_common_vertical_ch02_termination: ', _common_vertical_ch02_termination);
		write(*, '_common_vertical_ch02_position: ', _common_vertical_ch02_position);
		write(*, '_common_vertical_ch02_offset: ', _common_vertical_ch02_offset);
		write(*, '_common_vertical_ch02_bw_limit: ', _common_vertical_ch02_bw_limit);
			
		write(*, '_ch03_state: ', _ch03_state);
		write(*, '_common_vertical_ch03_coupling: ', _common_vertical_ch03_coupling);
		write(*, '_common_vertical_ch03_termination: ', _common_vertical_ch03_termination);
		write(*, '_common_vertical_ch03_position: ', _common_vertical_ch03_position);
		write(*, '_common_vertical_ch03_offset: ', _common_vertical_ch03_offset);
		write(*, '_common_vertical_ch03_bw_limit: ', _common_vertical_ch03_bw_limit);

		write(*, '_ch04_state: ', _ch04_state);
		write(*, '_common_vertical_ch04_coupling: ', _common_vertical_ch04_coupling);
		write(*, '_common_vertical_ch04_termination: ', _common_vertical_ch04_termination);
		write(*, '_common_vertical_ch04_position: ', _common_vertical_ch04_position);
		write(*, '_common_vertical_ch04_offset: ', _common_vertical_ch04_offset);
		write(*, '_common_vertical_ch04_bw_limit: ', _common_vertical_ch04_bw_limit);	
			
		write(*, '_window01_horiz_acq_scale: ', _window01_horiz_acq_scale);		
		write(*, '_window01_horiz_acq_delay: ', _window01_horiz_acq_delay);		
		write(*, '_window01_horiz_acq_position: ', _window01_horiz_acq_position);
		write(*, '_window01_acq_duration: ', _window01_acq_duration);
		write(*, '_window01_record_length: ', _window01_record_length);	
			
		write(*, '_window01_vertical_ch01_scale: ', _window01_vertical_ch01_scale);	
		write(*, '_window01_vertical_ch02_scale: ', _window01_vertical_ch02_scale);	
		write(*, '_window01_vertical_ch03_scale: ', _window01_vertical_ch03_scale);	
		write(*, '_window01_vertical_ch04_scale: ', _window01_vertical_ch04_scale);		
		
		write(*, '_window02_state: ', _window02_state);
		write(*, '_window02_horiz_acq_scale: ', _window02_horiz_acq_scale);		
		write(*, '_window02_horiz_acq_delay: ', _window02_horiz_acq_delay);		
		write(*, '_window02_horiz_acq_position: ', _window02_horiz_acq_position);	
		write(*, '_window02_acq_duration: ', _window02_acq_duration);
		write(*, '_window02_record_length: ', _window02_record_length);
			
		write(*, '_window02_vertical_ch01_scale: ', _window02_vertical_ch01_scale);	
		write(*, '_window02_vertical_ch02_scale: ', _window02_vertical_ch02_scale);	
		write(*, '_window02_vertical_ch03_scale: ', _window02_vertical_ch03_scale);	
		write(*, '_window02_vertical_ch04_scale: ', _window02_vertical_ch04_scale);
	}


	DPO7054->SetAcq(float(_common_horiz_acq_sample_rate));
	DPO7054->SetTrigger(_common_trigger_source,_common_trigger_coupling,_common_trigger_slope,float(_common_trigger_level),float(_common_trigger_holdoff));

	DPO7054->SetCH01(
				val(_ch01_state),
				_common_vertical_ch01_coupling,
				float(_common_vertical_ch01_termination),
				float(_common_vertical_ch01_position),
				float(_common_vertical_ch01_offset),
				_common_vertical_ch01_bw_limit
			);
	DPO7054->SetCH02(
				val(_ch02_state),
				_common_vertical_ch02_coupling,
				float(_common_vertical_ch02_termination),
				float(_common_vertical_ch02_position),
				float(_common_vertical_ch02_offset),
				_common_vertical_ch02_bw_limit
			);
	DPO7054->SetCH03(
				val(_ch03_state),
				_common_vertical_ch03_coupling,
				float(_common_vertical_ch03_termination),
				float(_common_vertical_ch03_position),
				float(_common_vertical_ch03_offset),
				_common_vertical_ch03_bw_limit
			);
	DPO7054->SetCH04(
				val(_ch04_state),
				_common_vertical_ch04_coupling,
				float(_common_vertical_ch04_termination),
				float(_common_vertical_ch04_position),
				float(_common_vertical_ch04_offset),
				_common_vertical_ch04_bw_limit
			);


	DPO7054->SetWindow01(
				val(_ON),
				float(_window01_horiz_acq_scale),
				float(_window01_horiz_acq_delay),
				float(_window01_horiz_acq_position),
				val(_window01_record_length),
				float(_window01_vertical_ch01_scale),
				float(_window01_vertical_ch02_scale),
				float(_window01_vertical_ch03_scale),
				float(_window01_vertical_ch04_scale)
				);

	DPO7054->SetWindow02(
				val(_window02_state),
				float(_window02_horiz_acq_scale),
				float(_window02_horiz_acq_delay),
				float(_window02_horiz_acq_position),
				val(_window02_record_length),
				float(_window02_vertical_ch01_scale),
				float(_window02_vertical_ch02_scale),
				float(_window02_vertical_ch03_scale),
				float(_window02_vertical_ch04_scale)
				);
	
	DPO7054->INIT();

	
	
	return(1);
}

