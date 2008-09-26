public fun Amplitude_M0()
{
    _k = (4.0 * 48.0);
	_times = dim_of(\MHD_BR::CONTROL.SIGNALS:MODE_MOD_1);
	_nTime = size(_times);
	
	_data  = [  data(    (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_1  ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_5  ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_9  ),				
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_13 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_17 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_21 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_25 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_29 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_33 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_37 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_41 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_45 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_49 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_53 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_57 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_61 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_65 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_69 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_73 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_77 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_81 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_85 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_89 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_93 ),
				data(    (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_97 )];
	
				
	return ( make_signal(set_range(_ntime, 25, _data),, _times, [0..24] ) );
				

}