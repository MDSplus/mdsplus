public fun pulseCheck_I2t()
{
	_I2T_PM_MARGIN = -23e9 * 0.5;
	
	_i2t_res = I2t_PM("PRE_PULSE", 1);
	
	write (*, _i2t_res);
	
	if( _i2t_res <  0  )
		return ( 1 );
	else
	{
		return ( 0 );
	}
}