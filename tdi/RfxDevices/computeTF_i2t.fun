public fun computeTF_i2t()
{

	_it = resample(\RFX::TF_SETUP:WAVE, \t_START_TF, \T_STOP_TF,0.001) * 2;
	_it2 = make_signal( _it * _it,  *, dim_of(_it) );

	_I2max = maxval( _it );	

	_iqt = maxval( integrate ( _it2 ) ) + _I2max * 150e-3 / 2;
	
	return ( _iqt );
}
