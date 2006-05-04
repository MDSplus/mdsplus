public fun K3115__init(as_is _nid, optional _method)
{
	private __CAMAC_NAME = 1;
	private __COMMENT = 2;
	private __CONTROL_MODE = 3;
	private __WAVE_MODE = 4;

	private __CLOCK_SOURCE = 5;
	private __CLOCK_MODE = 6;
	private __CLOCK_FREQ = 7;



	private __NODES_PER_CHANNEL = 7;
	private __CHANNEL_1 = 8;
	private __RANGE = 1;
	private __RANGE_POL = 2;
	private __TIME_MODE = 3;
	private __OUTPUT = 4;
	private __VOLTAGES = 5;
	private __TIMES = 6;

	private _MASTER = 0;
	private _SLAVE = 1;

	private _CYCLIC = 1;
	private _ACYCLIC = 0;

	private _INTERNAL = 0;
	private _EXTERNAL = 1;

	private _BIPOLAR = 1;
	private _UNIPOLAR = 0;

	private _ASIS = 0;
	private _VARIABLE = 1;

	private _MDS$K_FHUGE = 10E10;






	_data_ok = 1;

	_camac_name = if_error(data(DevNodeRef(_nid, __CAMAC_NAME)), _data_ok = 0);
	if(!_data_ok)
	{
		DevLogErr(_nid, "Invalid CAMAC name");
		abort();
	}
/*	write(*, "Camac Name: ", _camac_name); */

	DevNodeCvt(_nid, __CONTROL_MODE, ['MASTER', 'SLAVE'], [_MASTER, _SLAVE], _control_mode = _MASTER);
/*	write(*, "Control Mode: ", (_control_mode == _MASTER ? "MASTER" : "SLAVE")); */

	DevNodeCvt(_nid, __WAVE_MODE, ['CYCLIC', 'ACYCLIC'], [_CYCLIC, _ACYCLIC], _wave_mode = _CYCLIC);
/*	write(*, "Wave Mode: ", (_wave_mode == _CYCLIC ? "CYCLIC" : "ACYCLIC")); */

	DevNodeCvt(_nid, __CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [_INTERNAL, _EXTERNAL], _clock_mode = _INTERNAL);
/*	write(*, "Clock Mode: ", (_clock_mode == _INTERNAL ? "INTERNAL" : "EXTERNAL")); */


	write(*, "OK 1");

	if(_clock_mode == _EXTERNAL)
	{
		_status = 1;
		_clk = if_error(DevNodeRef(_nid, __CLOCK_SOURCE), _status = 0);
		if(!_status)
		{
			DevLogErr(_nid, "Invalid Clock specification");
			abort();
		}

		_clock_source = execute('`_clk');
		_clock_delta = dscptr(_clock_source, 2);
		_clock_freq = 1. / _clock_delta;
		_clock_rate = 0;
		if((_clock_start = dscptr(_clock_source, 0)) == *)
		{
			_clock_start = 0.;
		}
		if((_clock_end = dscptr(_clock_source, 1)) == *)
		{
			_clock_end = _MDS$K_FHUGE;
		}
		write(*, "OK 2");
	}
	else
	{
		DevNodeCvt(_nid, __CLOCK_FREQ, [10E3, 5E3, 2.5E3, 1E3, 500, 250, 100, 50], [7, 6, 5, 4, 3, 2, 1, 0], _clock_rate = 4);
		_clock_freq = data(DevNodeRef(_nid, __CLOCK_FREQ));
		_clock_delta = 1. / _clock_freq;
		_clock_source = make_range(*, *, _clock_delta);
		_clock_start = 0;
		_clock_end = _MDS$K_FHUGE;
	}
/*
	write(*, "Clock Start: ", _clock_start);
	write(*, "Clock End: ", _clock_end);
	write(*, "Clock Frequency (Hz): ", _clock_freq);
*/

	_clock_control = word(_control_mode << 4 | _clock_mode << 3 | _clock_rate);
/*	write(*, "Clock Control: ", _clock_control); */

	DevCamChk(_camac_name, CamPiow(_camac_name, 0, 17, _clock_control, 16), 1, 1); 

	DevCamChk(_camac_name, CamPiow(_camac_name, 2, 9, _zero = 0, 16), 1, 1);

	write(*, "OK 3");

	if(_wave_mode == _CYCLIC)
	{
		DevCamChk(_camac_name, CamPiow(_camac_name, 0, 26, _dummy=0, 16), 1, 1);
	}
	else
	{
		DevCamChk(_camac_name, CamPiow(_camac_name, 0, 24, _dummy=0, 16), 1, 1);
	}

	write(*, "OK 4");	

	_max_end_time = _clock_start + 1023 * _clock_delta;
/*	write(*, "Max end time: ", _max_end_time); */

	for(_n_chan = 0; _n_chan < 6; _n_chan++)
	{
		write(*, "CHAN ", _n_chan);		
		if( DevIsOn(DevNodeRef(_nid, __CHANNEL_1 + (_n_chan * __NODES_PER_CHANNEL))) )
		{
		/*	write(*, "Channel ", _n_chan + 1, " is ON"); */
			DevNodeCvt(_nid, __CHANNEL_1 + __RANGE + (_n_chan * __NODES_PER_CHANNEL), [20, 10, 5], [20, 10, 5], _range = 20);
			/* write(*, "Channel ", _n_chan + 1, " range (Vpp): ", _range); */
			DevNodeCvt(_nid, __CHANNEL_1 + __RANGE_POL + (_n_chan * __NODES_PER_CHANNEL), ['BIPOLAR', 'UNIPOLAR'], [_BIPOLAR, _UNIPOLAR], _range_pol = _BIPOLAR);
			/* write(*, "Channel ", _n_chan + 1, " range polarity: ", (_range_pol == _BIPOLAR ? "BIPOLAR" : "UNIPOLAR")); */
			if((_range_pol == _UNIPOLAR) && (_range == 20))
			{
				write(*, "Channel ", _n_chan + 1, " invalid range");
				abort();
			}
			_low_voltage = (_range_pol == _BIPOLAR) ? -_range / 2. : 0.;
			_high_voltage = (_range_pol == _BIPOLAR) ? _range / 2. : _range;


/*
			write(*, "Channel ", _n_chan + 1, " low voltage (V): ", _low_voltage);
			write(*, "Channel ", _n_chan + 1, " high voltage (V): ", _high_voltage);
*/


			_data_ok = 1;
			_voltages = if_error(data(DevNodeRef(_nid, __CHANNEL_1 + __VOLTAGES + (_n_chan * __NODES_PER_CHANNEL))), _data_ok = 0);
			if(!_data_ok)
			{
				DevLogErr(_nid, "Invalid voltages specification for channel "// _chan);
				abort();
			}

			_n_voltages = size(_voltages);
	/*		write(*, "Channel ", _n_chan + 1, " voltages before: ", _voltages); */



			_new_voltages = [];
			_clipped = 0;
			_trucated = 0;
			for(_j = 0; _j < _n_voltages; _j++)
			{
				if(_voltages[_j] > _high_voltage)
				{
/*
					if( _j == (_n_voltages-1) )
					{
						_new_voltages = [_new_voltages, 0];
						_truncated = 1;
					}
					else
					{
						_new_voltages = [_new_voltages, _high_voltage];
					}
*/

					_new_voltages = [_new_voltages, _high_voltage];
					_clipped = 1;
				}
				else
				{
					if(_voltages[_j] < _low_voltage)
					{
/*
						if( _j == (_n_voltages-1) )
						{
							_new_voltages = [_new_voltages, 0];
							_truncated = 1;
						}
						else
						{
							_new_voltages = [_new_voltages, _low_voltage];
						}
*/

						_new_voltages = [_new_voltages, _low_voltage];
						_clipped = 1;
					}
					else
					{
/*
						if( _j == (_n_voltages-1) )
						{
							_new_voltages = [_new_voltages, 0];
							_truncated = 1;
						}
						else
						{
							_new_voltages = [_new_voltages, _voltages[_j]];
						}
*/
						_new_voltages = [_new_voltages, _voltages[_j]];
					}
				}
			}
			if(_clipped == 1)
			{
				write(*, "Channel ", _n_chan + 1, " clipped");
			}



			DevNodeCvt(_nid, __CHANNEL_1 + __TIME_MODE + (_n_chan * __NODES_PER_CHANNEL), ['AS IS', 'VARIABLE'], [_ASIS, _VARIABLE], _time_mode = _ASIS);
		/*	write(*, "Channel ", _n_chan + 1, " time mode: ", (_time_mode == _ASIS ? "AS IS" : "VARIABLE")); */

			if(_time_mode == _VARIABLE)
			{


				_data_ok = 1;
				_times = if_error(data(DevNodeRef(_nid, __CHANNEL_1 + __TIMES + (_n_chan * __NODES_PER_CHANNEL))), _data_ok = 0);
				if(!_data_ok)
				{
					DevLogErr(_nid, "Invalid CAMAC name");
					abort();
				}
				_n_times = size(_times);
				if(_n_voltages <> _n_times)
				{
					write(*, "Times <> Voltages");
					abort();
				}


				_status = 1;
				for(_j = 0; (_j < (_n_times - 1)); _j++)
				{
					if(_times[_j] > _times[_j + 1])
					{
 						_status = 0;
						break;
					}
				}

				/* write(*, "Channel ", _n_chan + 1, " times: ", _times); */
				if(_status == 0)
				{
					write(*, "Temp [", _j, "] errato", _times);
					abort();
				}


				if(_times[_n_times-1] < _clock_end)
				{
					_end_time = _times[_n_times-1];
				}
				else
				{
					_end_time = _clock_end;
				}
				if(_end_time > _max_end_time)
				{
					_end_time = _max_end_time;
				}

				_sgn = make_signal(_new_voltages, , _times);
				_rsgn = resample(_sgn, _clock_start, _end_time, _clock_delta);

				_new_rsgn =  data(_rsgn);
				if(_wave_mode == _ACYCLIC)
				{
				    _len = size(_new_rsgn);
				    if(_len > 1020)
					_len = 1020;
				    _new_rsgn = [ _new_rsgn[*: _len ], zero(1023 - _len, 0) ];                             
				} 
		
				_rsgn_new = make_signal(_new_rsgn, , dim_of(_rsgn));
	

				DevPut(_nid, __CHANNEL_1 + __OUTPUT + (_n_chan * __NODES_PER_CHANNEL), _rsgn_new);
				_fifo = word((4095. / _range) * _new_rsgn);

			}
			else
			{
				if(_n_voltages > 1024)
				{
					write(*, "Channel ", _n_chan + 1, " troppi valori");
					abort();
				}

				_tensioni = [];
				if(_n_voltages < 1024)
				{
					for(_j = 0; _j < _n_voltages; _j++)
						_tensioni = [_tensioni, _new_voltages[_j]];
					for(_j = _n_voltages; _j < 1024; _j++)
						_tensioni = [_tensioni, 0];
				}
				else
				{
					_tensioni = _new_voltages[*: 1022];
					_tensioni = [_tensioni, 0];
				}

				_fifo = word((4095. / _range) * _tensioni);
				/* write(*, _fifo); */

				_times = make_range(0, 1023. * _clock_delta, _clock_delta);
				_sgn = make_signal(_tensioni, , _times);
				DevPut(_nid, __CHANNEL_1 + __OUTPUT + (_n_chan * __NODES_PER_CHANNEL), _sgn);
			}
		}
		else
		{
		    write(*, "OFF CHAN ", _n_chan);
			/* write(*, "Channel ", _n_chan + 1, " is OFF"); */
			_fifo = word(zero(1024, 0));
		}

/*
		write(*, "FIFO", word(_fifo));
		write(*, "Size fifo ", size(_fifo));
*/

		DevCamChk(_camac_name, CamQStopw(_camac_name, _n_chan, 16, size(_fifo), _fifo, 16), 1, 1);

		write(*, "OK CHAN ", _n_chan);
	}
		


	DevCamChk(_camac_name, CamPiow(_camac_name, 0, 25, _dummy=0, 16), 1, 1);

	write(*, "OK 5");

	return(1);
}

