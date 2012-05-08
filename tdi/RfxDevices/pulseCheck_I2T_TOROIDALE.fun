public fun pulseCheck_I2T_TOROIDALE()
{
	_error = 0;

	_i2t = computeTF_i2t();
	if( _i2t >  300e6 )
	{
		write(*, " Valore di I2T  Toroidale superiore al limite di 300MA2s ("// _i2t //")");
		_error = 1;
	}
	
	return ( _error );
}
