public fun ComputeMaxPower()
{

	_power = 2.183105*(\GQSF01_IR01VA*\GQSF01_UR01VA+\GQSF01_IS01VA*\GQSF01_US01VA+\GQSF01_IT01VA*\GQSF01_UT01VA);

	_powerVal = data( _power );

	_maxPower = maxval( _powerVal );

	return( _maxPower );
}