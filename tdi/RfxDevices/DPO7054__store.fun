public fun DPO7054__store(as_is _nid, optional _method)
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


	write(*, "STORE");


/******************* COMMON HORIZONTAL - ACQ ******************/

	_status = 1 ;
	_common_horiz_acq_sample_rate = if_error(data(DevNodeRef(_nid, _N_COMMON_HORIZ_ACQ_SAMPLE_RATE)), _status = 0) ;
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on COMMON HORIZ_ACQ SAMPLE_RATE');

		abort();
	}

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


	_trig = ft_float(data(DevNodeRef(_nid, _N_COMMON_TRIGGER_TRIG_SOURCE)));
	write(*,'_trig: ', _trig);
	_clock = make_range(*,*,ft_float(1./ _common_horiz_acq_sample_rate));


	_window01_record_length = long(ceiling(ft_float(10.) * ft_float(_window01_horiz_acq_scale) * ft_float(_common_horiz_acq_sample_rate)));
	_window02_record_length = long(ceiling(ft_float(10.) * ft_float(_window02_horiz_acq_scale) * ft_float(_common_horiz_acq_sample_rate)));



	
	_window01_horiz_acq_position_points = DPO7054->GetWindowPosition(val(1));
	_window02_horiz_acq_position_points = DPO7054->GetWindowPosition(val(2));


	write(*,'_window01_record_length',_window01_record_length);

	write(*,'_window02_record_length',_window02_record_length);

	write(*,'_window01_horiz_acq_position_points',_window01_horiz_acq_position_points);
	write(*,'_window02_horiz_acq_position_points',_window02_horiz_acq_position_points);



	if(_window01_record_length > _window02_record_length)
		_buffer_size = _window01_record_length;
	else
		_buffer_size = _window02_record_length;



	_status = DPO7054->IsAcqDone();
	if(_status)
	{

		_data = zero(_buffer_size, 0B);

		_y_off = 0.;
		_y_mult = 0.;
		_start_idx = - _window01_horiz_acq_position_points;
		_end_idx = _window01_record_length - _window01_horiz_acq_position_points;
	write(*,'_start_idx',_start_idx);
	write(*,'_end_idx',_end_idx);
		_dim = make_dim(make_window(_start_idx, _end_idx, _trig[0]), _clock);


		/************ WINDOW01 CH01 ******************/
		if(DPO7054->IsChannelAcquired(val(1),val(1)))
		{

			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01)))
			{


				DPO7054->ReadData(val(1),val(1), ref(_data));



				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW01_VERTICAL_CH01_DATA;

				DPO7054->GetPreamble(val(1),val(1),ref(_y_off),ref(_y_mult));


				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window01_record_length - 1, _dim);



				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}

			}
		}

		/************ WINDOW01 CH02 ******************/
		if(DPO7054->IsChannelAcquired(val(1),val(2)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02)))
			{
				DPO7054->ReadData(val(1),val(2), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW01_VERTICAL_CH02_DATA;
				DPO7054->GetPreamble(val(1),val(2),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window01_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
		/************ WINDOW01 CH03 ******************/
		if(DPO7054->IsChannelAcquired(val(1),val(3)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03)))
			{
				DPO7054->ReadData(val(1),val(3), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW01_VERTICAL_CH03_DATA;
				DPO7054->GetPreamble(val(1),val(3),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window01_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
		/************ WINDOW01 CH04 ******************/
		if(DPO7054->IsChannelAcquired(val(1),val(4)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04)))
			{
				DPO7054->ReadData(val(1),val(4), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW01_VERTICAL_CH04_DATA;
				DPO7054->GetPreamble(val(1),val(4),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window01_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}


	if(DevIsOn(DevNodeRef(_nid, _N_WINDOW02)))
	{
		_y_off = 0.;
		_y_mult = 0.;
		_start_idx = - _window02_horiz_acq_position_points;
		_end_idx = _window02_record_length - _window02_horiz_acq_position_points;
		_dim = make_dim(make_window(_start_idx, _end_idx, _trig[1]), _clock);

		_data = zero(_buffer_size, 0B);

		/************ WINDOW02 CH01 ******************/
		if(DPO7054->IsChannelAcquired(val(2),val(1)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH01)))
			{
				DPO7054->ReadData(val(2),val(1), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW02_VERTICAL_CH01_DATA;
				DPO7054->GetPreamble(val(2),val(1),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window02_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
		/************ WINDOW02 CH02 ******************/
		if(DPO7054->IsChannelAcquired(val(2),val(2)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH02)))
			{
				DPO7054->ReadData(val(2),val(2), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW02_VERTICAL_CH02_DATA;
				DPO7054->GetPreamble(val(2),val(2),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window02_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
		/************ WINDOW02 CH03 ******************/
		if(DPO7054->IsChannelAcquired(val(2),val(3)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH03)))
			{
				DPO7054->ReadData(val(2),val(3), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW02_VERTICAL_CH03_DATA;
				DPO7054->GetPreamble(val(2),val(3),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window02_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
		/************ WINDOW02 CH04 ******************/
		if(DPO7054->IsChannelAcquired(val(2),val(4)))
		{
			if(DevIsOn(DevNodeRef(_nid, _N_COMMON_VERTICAL_CH04)))
			{
				DPO7054->ReadData(val(2),val(4), ref(_data));
				_nid_head = getnci(_nid, 'nid_number');
				_sig_nid = _nid_head + _N_WINDOW02_VERTICAL_CH04_DATA;
				DPO7054->GetPreamble(val(2),val(4),ref(_y_off),ref(_y_mult));
				_status = DevPutSignal(_sig_nid, _y_off*_y_mult, _y_mult, _data, 0, _window02_record_length - 1, _dim);
				if(! _status )
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();

				}
			}
		}
	}

	}
	else
	{
		DevLogErr(_nid, 'Module is not in stop state. Store not done.');
		abort();
	}



	return(1);
}
