public fun Amplitude_M1()
{
    _k = (4.0 * 48.0);
	_times = dim_of(\MHD_BR::CONTROL.SIGNALS:MODE_MOD_98);
	_nTime = size(_times);
	
	_data  = [  data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_98 ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_94 ),
			    data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_90 ),				
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_86 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_82 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_78 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_74 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_70 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_66 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_62 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_58 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_54 ),
				
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_50 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_46 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_42 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_38 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_34 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_30 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_26 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_22 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_18 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_14 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_10 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_6  ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_2  ),
				
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_190 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_186 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_182 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_178 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_174 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_170 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_166 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_162 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_158 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_154 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_150 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_146 ),

				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_142 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_138 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_134 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_130 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_126 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_122 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_118 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_114 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_110 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_106 ),
				data(2 * (1 / _k) * \MHD_BR::CONTROL.SIGNALS:MODE_MOD_102 ) ];
	
				
	return ( make_signal(set_range(_ntime, 48, _data),, _times, [-24..23] ) );
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
}