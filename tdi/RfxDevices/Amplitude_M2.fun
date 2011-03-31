public fun Amplitude_M2()
{
    _k = (4.0 * 48.0);
	_times = dim_of(\MHD_BR::CONTROL.SIGNALS:MODE_MOD_7);
	_nTime = size(_times);
	
	_data  = [  data(    (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_3 ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_7  ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_11 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_15 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_19 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_23 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_27 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_31 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_35 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_39 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_43 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_47 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_51 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_55 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_59 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_63 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_67 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_71 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_75 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_79 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_83 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_87 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_91 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_95 ),
				data(    (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_99 )];
	
				
	return ( make_signal(set_range(_ntime, 25, _data),, _times, [0..24] ) );
				
}