public fun K3115__init_new ( as_is _nid, optional _method )
{
	private 	__CAMAC_NAME = 1 ;
	private 	__COMMENT = 2 ;
	private 	__CONTROL_MODE = 3 ;
	private 	__WAVE_MODE = 4 ;

	private 	__CLOCK_SOURCE = 5 ;
	private 	__CLOCK_MODE = 6 ;
	private 	__CLOCK_FREQ = 7 ;

	private 	__NODES_PER_CHANNEL = 7 ;
	private 	__CHANNEL_1 = 8 ;
	private 	__RANGE = 1 ;
	private 	__RANGE_POL = 2 ;
	private 	__TIME_MODE = 3 ;
	private 	__OUTPUT = 4 ;
	private 	__VOLTAGES = 5 ;
	private 	__TIMES = 6 ;


	private 	_MASTER = 0 ;
	private 	_SLAVE = 1 ;

	private 	_CYCLIC = 1 ;
	private 	_ACYCLIC = 0 ;

	private 	_INTERNAL = 0 ;
	private 	_EXTERNAL = 1 ;

	private 	_BIPOLAR = 1 ;
	private 	_UNIPOLAR = 0 ;

	private 	_ASIS = 0 ;
	private 	_VARIABLE = 1 ;

	private 	_MDS$K_FHUGE = 10E10 ;
	private		_FIFO_DEPTH = 1024 ;




	_status = 1 ;
	_camac_name = if_error ( DevNodeRef ( _nid, __CAMAC_NAME ), _status = 0 ) ;
	if ( _status == 0 )
	{
		DevLogErr ( _nid, "Invalid camac name." ) ;
		abort (  ) ;
	}

	/* check camac name with abort sampling cycle function */

	DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 9, _zero = 0, 16 ), 1, 1 ) ;


	DevNodeCvt ( _nid, __CONTROL_MODE, [ 'MASTER', 'SLAVE' ], [ _MASTER, _SLAVE ], _control_mode = _MASTER ) ;
	DevNodeCvt ( _nid, __WAVE_MODE, [ 'CYCLIC', 'ACYCLIC' ], [ _CYCLIC, _ACYCLIC ], _wave_mode = _CYCLIC ) ;
	DevNodeCvt ( _nid, __CLOCK_MODE, [ 'INTERNAL', 'EXTERNAL' ], [ _INTERNAL, _EXTERNAL ], _clock_mode = _INTERNAL ) ;

	if ( _clock_mode == _EXTERNAL )
	{
		_status = 1 ;
		_clk = if_error ( DevNodeRef ( _nid, __CLOCK_SOURCE ), _status = 0 ) ;
		if ( _status == 0 )
		{
			DevLogErr ( _nid, "Invalid external clock." ) ;
			abort (  ) ;
		}
		_clock_source = execute ( '`_clk' ) ;
		_clock_delta = dscptr ( _clock_source, 2 ) ;
		_clock_freq = 1. / _clock_delta ;
		_clock_rate = 0 ;
		if ( ( _clock_start = dscptr( _clock_source, 0 ) ) == * )
		{
			_clock_start = 0. ;
		}
		if ( ( _clock_end = dscptr ( _clock_source, 1 ) ) == * )
		{
			_clock_end = _MDS$K_FHUGE ;
		}
	}
	else	/* internal clock */
	{
		DevNodeCvt ( _nid, __CLOCK_FREQ, [ 10E3, 5E3, 2.5E3, 1E3, 500, 250, 100, 50 ], [ 7, 6, 5, 4, 3, 2, 1, 0 ], _clock_rate = 4 ) ;

		_status = 1 ;
		_clock_freq = if_error ( data ( DevNodeRef ( _nid, __CLOCK_FREQ ) ), _status = 0 ) ;
		if ( _status == 0 )
		{
			DevLogErr ( _nid, "Invalid internal clock frequency." ) ;
			abort (  ) ;
		}
		_clock_delta = 1. / _clock_freq ;
		_clock_source = make_range ( *, *, _clock_delta ) ;
		_clock_start = 0 ;
		_clock_end = _MDS$K_FHUGE ;
	}


	_max_end_time = _clock_start + ( _FIFO_DEPTH - 1 ) * _clock_delta ;
	_n_clock_edges = ( _clock_end - _clock_start ) / _clock_delta ;

	_clock_control = word ( _control_mode << 4 | _clock_mode << 3 | _clock_rate ) ;
	

	write ( *, "K3115 GLOBAL SETUP" ) ;
	write ( *, "	Camac Name: ", data ( _camac_name ) ) ;
	write ( *, "	Control Mode: ", ( _control_mode == _MASTER ? "MASTER" : "SLAVE" ) ) ;
	write ( *, "	Wave Mode: ", ( _wave_mode == _CYCLIC ? "CYCLIC" : "ACYCLIC" ) ) ;
	write ( *, "	Clock Mode: ", ( _clock_mode == _INTERNAL ? "INTERNAL" : "EXTERNAL" ) ) ;
	write ( *, "	Clock Start Time (s): ", Trim ( AdjustL ( _clock_start ) ) ) ;
	write ( *, "	Clock End Time (s): ", Trim ( AdjustL ( _clock_end ) ) ) ;
	write ( *, "	Clock Frequency (Hz): ", Trim ( AdjustL ( _clock_freq ) ) ) ;
	write ( *, "	Clock Period (s): ", Trim ( AdjustL ( _clock_delta ) ) ) ;
	write ( *, "	Maximum End Time (s): ", Trim ( AdjustL ( _max_end_time ) ) ) ;
	write ( *, "	Clock Edges Number: ", Trim ( AdjustL ( _n_clock_edges ) ) ) ;
	write ( *, "	Clock Control Register: ", Trim ( AdjustL ( _clock_control ) ) ) ;




	/* write clock control register */

	DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 17, _clock_control, 16 ), 1, 1 ) ; 




	/* sampling cycle abort and outputs to zero */

	DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 9, _zero = 0, 16 ), 1, 1 ) ;


	/* reset fifo memory pointer */

	DevCamChk ( _camac_name, CamPiow ( _camac_name, 2, 9, _zero = 0, 16 ), 1, 1 ) ;







	for ( _n_chan = 0 ; _n_chan < 6 ; _n_chan++ )
	{
		write ( *, "K3115 CHANNEL ", Trim ( AdjustL ( _n_chan + 1 ) ), " SETUP" ) ;

		if ( DevIsOn ( DevNodeRef ( _nid, __CHANNEL_1 + ( _n_chan * __NODES_PER_CHANNEL ) ) ) )
		{
			write ( *, "	Status: ON" ) ;

			DevNodeCvt ( _nid, __CHANNEL_1 + __RANGE + ( _n_chan * __NODES_PER_CHANNEL ), [ 20, 10, 5 ], [ 20, 10, 5 ], _range = 20 ) ;
			DevNodeCvt ( _nid, __CHANNEL_1 + __RANGE_POL + ( _n_chan * __NODES_PER_CHANNEL ), [ 'BIPOLAR', 'UNIPOLAR' ], [ _BIPOLAR, _UNIPOLAR ], _range_pol = _BIPOLAR ) ;
			DevNodeCvt ( _nid, __CHANNEL_1 + __TIME_MODE + ( _n_chan * __NODES_PER_CHANNEL ), [ 'AS IS', 'VARIABLE' ], [ _ASIS, _VARIABLE ], _time_mode = _ASIS ) ;	

			if ( ( _range_pol == _UNIPOLAR ) && ( _range == 20 ) )
			{
				_msg = "Range value in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " too high." ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}

			_high_voltage = ( _range_pol == _BIPOLAR ) ? _range / 2 : _range ;
			_low_voltage = ( _range_pol == _BIPOLAR ) ? -_range / 2 : 0 ;

			write ( *, "	Range (Vpp): ", Trim ( AdjustL ( _range ) ) ) ;
			write ( *, "	Range polarity: ", ( _range_pol == _BIPOLAR ? "BIPOLAR" : "UNIPOLAR" ) ) ;
			write ( *, "	Time mode: ", ( _time_mode == _ASIS ? "AS IS" : "VARIABLE" ) ) ;
			write ( *, "	High Voltage (V): ", Trim ( AdjustL ( _high_voltage ) ) ) ;
			write ( *, "	Low Voltage (V): ", Trim ( AdjustL ( _low_voltage ) ) ) ;

			_status = 1 ;
			_voltages = if_error ( data ( DevNodeRef ( _nid, __CHANNEL_1 + __VOLTAGES + ( _n_chan * __NODES_PER_CHANNEL ) ) ), _status = 0 ) ;
			if ( _status == 0 )
			{
				_msg = "Invalid voltages array in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // "." ;
				DevLogErr ( _nid, _msg ) ;
				abort (  ) ;
			}

			/* Check if voltages array has more than _FIFO_DEPTH points */

			if ( _FIFO_DEPTH < size ( _voltages ) )
			{
				_voltages = _voltages [* : _FIFO_DEPTH - 1 ] ;
				_msg = "Voltages array in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " has more than " // Trim ( AdjustL ( _FIFO_DEPTH ) ) // " points. Signal will be truncated." ;
				DevLogErr ( _nid, _msg ) ;
			}


			/* Check if voltages array has some points outside the range */

			_tmp = [  ] ;
			_clipped = 0 ;
			for ( _j = 0 ; _j < size ( _voltages ) ; _j++ )
			{
				if( _voltages [ _j ] > _high_voltage )
				{
					_clipped = 1 ;
					_tmp = [ _tmp, _high_voltage ] ;
				}
				else
				{
					if ( _voltages[ _j ] < _low_voltage )
					{
						_clipped = 1 ;
						_tmp = [ _tmp, _low_voltage ] ;
					}
					else
					{
						_tmp = [ _tmp, _voltages [ _j ] ] ;
					}
				}
			}

			if ( _clipped == 1 )
			{
				_voltages = _tmp ;
				_msg = "Some voltages in channel " // Trim ( AdjustL ( _n_chan + 1 ) ) // " are outside the range. Signal will be clipped." ;
				DevLogErr ( _nid, _msg ) ;
			}

			if( _time_mode == _VARIABLE )
			{
				_status = 1 ;
				_times = if_error ( data ( DevNodeRef ( _nid, __CHANNEL_1 + __TIMES + ( _n_chan * __NODES_PER_CHANNEL ) ) ), _status = 0 ) ;
				if ( _status == 0 )
				{
					_msg = "Invalid times array in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // "." ;
					DevLogErr ( _nid, _msg ) ;
					abort (  ) ;
				}



				if ( size ( _voltages ) > size ( _times ) )
				{
					_voltages = _voltages [* : size ( _times ) - 1 ] ;
					_msg = "Voltages array in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " has more values than times array. Signal will be truncated." ;
					DevLogErr ( _nid, _msg ) ;
				}

				_times = _times [* : size ( _voltages ) - 1 ] ;



				_status = 1 ;
				for ( _j = 0 ; ( _j < ( size ( _times ) - 1 ) ) ; _j++ )
				{
					if ( _times [ _j ] >= _times [ _j + 1 ] )
					{
 						_status = 0 ;
						break ;
					}
				}

				if ( _status == 0 )
				{
					_msg = "Values in Times array of channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " not in increasing order." ;
					DevLogErr ( _nid, _msg ) ;
					abort (  ) ;
				}

				write ( *, "Voltages: ", _voltages ) ;
				write ( *, "Times: ", _times ) ;


				if ( _times [ size ( _times ) - 1 ] < _clock_end )
				{
					_end_time = _times [ size ( _times ) - 1 ] ;
				}
				else
				{
					if ( _times [ size ( _times ) - 1 ] > _clock_end )
					{
						_msg = "Clock duration is too low. Signal in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " will be truncated." ;
						DevLogErr ( _nid, _msg ) ;
					}
					/* _end_time = _clock_end ; */
					_end_time = _clock_end - _clock_delta ;
				}
				if ( _end_time > _max_end_time )
				{
					_msg = "Signal in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " has more than " // Trim ( AdjustL ( _FIFO_DEPTH ) ) // " points. It will be truncated." ;
					DevLogErr ( _nid, _msg ) ;
					_end_time = _max_end_time ;
				}

				write ( *, "	Signal End Time (s): ", Trim ( AdjustL ( _end_time ) ) ) ;


				if ( _clock_start >= _end_time )
				{

					_msg = "Clock is out the signal in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // "." ;
					DevLogErr ( _nid, _msg ) ;
					_voltages = zero ( _FIFO_DEPTH, 0 ) ;
					write ( *, "Voltages: ", _voltages ) ;
					_times = make_range ( 0, ( _FIFO_DEPTH - 1 ) * _clock_delta, _clock_delta ) ;
					_sgn = make_signal ( _voltages, , _times ) ;
					DevPut ( _nid, __CHANNEL_1 + __OUTPUT + ( _n_chan * __NODES_PER_CHANNEL ), _sgn ) ;
					_fifo = word ( zero ( _FIFO_DEPTH, 0 ) ) ;
				}
				else
				{


					_sgn = make_signal ( _voltages, , _times ) ;
					_rsgn = resample ( _sgn, _clock_start, _end_time, _clock_delta ) ;
					_vrsgn = data ( _rsgn ) ;
					_size_vrsgn = size ( _vrsgn ) ;
					_vrsgn = pack( [ _vrsgn [ * : _size_vrsgn - 2 ], zero ( _FIFO_DEPTH - _size_vrsgn + 1, 0 ) ], 1 ) ;
					write ( *, "Voltages: ", _vrsgn ) ;

					_rsgn = make_signal ( _vrsgn, , dim_of ( _rsgn ) ) ;
	

					DevPut (_nid, __CHANNEL_1 + __OUTPUT + ( _n_chan * __NODES_PER_CHANNEL ), _rsgn ) ;
					_fifo = word ( ( 4095. / _range ) * _rsgn ) ;
				}
			}
			else	/* time mode AS IS */
			{
				_size_voltages = size ( _voltages ) ;
				if ( _n_clock_edges >= _size_voltages )
				{
					if(_wave_mode != _CYCLIC)
						_voltages = pack( [ _voltages [ * : _size_voltages - 2 ], zero ( _FIFO_DEPTH - _size_voltages + 1, 0 ) ], 1 ) ; 
				}
				else
				{
					_voltages = pack( [ _voltages [ * : _n_clock_edges - 2 ], zero ( _FIFO_DEPTH - _n_clock_edges + 1, 0 ) ], 1 ) ;
					_msg = "Clock duration is too low. Signal in channel " // Trim ( AdjustL( _n_chan + 1 ) ) // " will be truncated." ;
					DevLogErr ( _nid, _msg ) ;
				}
				write ( *, "SIZE: ", size ( _voltages ) ) ;
	/*			
				write ( *, "VOLTAGES: ", _voltages ) ;
         */
				_fifo = word ( ( 4095. / _range ) * _voltages ) ;

				_times = make_range ( 0, ( _FIFO_DEPTH - 1 ) * _clock_delta, _clock_delta ) ;
				_sgn = make_signal ( _voltages, , _times ) ;
				DevPut ( _nid, __CHANNEL_1 + __OUTPUT + ( _n_chan * __NODES_PER_CHANNEL ), _sgn ) ;
			}
		}
		else
		{
			write ( *, "	Status: OFF" ) ;

			_fifo = word ( zero ( _FIFO_DEPTH, 0 ) ) ;
		}

		DevCamChk ( _camac_name, CamQStopw ( _camac_name, _n_chan, 16, size ( _fifo ), _fifo, 16 ), 1, 1 ) ;

	}
	/* enable/disable recycle feature */ 

	if( _wave_mode == _CYCLIC )
	{
		DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 26, _dummy = 0, 16 ), 1, 1 ) ;
	}
	else
	{
		DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 24, _dummy = 0, 16 ), 1, 1 ) ;
	}
	DevCamChk ( _camac_name, CamPiow ( _camac_name, 0, 25, _dummy = 0, 16 ), 1, 1 ) ;
	return ( 1 ) ;
}