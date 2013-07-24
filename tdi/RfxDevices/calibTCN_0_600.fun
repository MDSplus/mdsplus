/*
* Polinomial calibration for thermocouple type N 
* in the range 0 - 600 C  
* Coefficients in  ITS-90 Thermocouple Direct and Inverse Polynomials document

*/
public fun calibTCN_0_600(as_is _sig)
{

	_K  = 47.513 * 1.e-3  / 5.;

	_data = data(_sig) * _K * 1.e6;
	_dim = dim_of(_sig);


	_C1 =  3.86896e-2;
	_C2 = -1.08267e-6;
	_C3 =  4.70205e-11;
	_C4 = -2.12169e-18;
	_C5 = -1.17272e-19;
	_C6 =  5.39280e-24;
	_C7 = -7.98156e-29;

	_calData = _C1 *_data + _C2 *power( _data, 2) + _C3 *power( _data, 3) + _C4 * power( _data, 4) + _C5 * power( _data, 5) + _C6 * power( _data, 6) + _C7 * power( _data, 7); 	

	return ( make_signal( _calData,, _dim ) );

}
