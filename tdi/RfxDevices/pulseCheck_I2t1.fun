public fun pulseCheck_I2t1()
{
	_I2T_PM_MARGIN = -23e9 * 0.07;	
	_i2t_res_estimate = I2t_PM("PRE_PULSE", 1);	
	_i2t_res = I2t_PM("INIT");

	write( *, _i2t_res_estimate);
	
	write (*, _i2t_res);
	
	if( ( _i2t_res >  0 )  &&  ( _i2t_res_estimate <  _I2T_PM_MARGIN ) )
	{
		return ( 1 );
	}
	else
	{
		return ( 0 );
	}
}