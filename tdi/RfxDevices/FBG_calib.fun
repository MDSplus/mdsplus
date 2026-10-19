/*
* Polynomial calibration of FBG signals
* A0 + A1*L + A2*L^2 + A3*L^3 + A4*L^4
*
* Four-coefficient calibration vectors from older shots are supported by
* treating the missing A4 coefficient as zero.
*/
public fun FBG_calib(as_is _sig, in _calibArr)
{
	_data = d_float(data(_sig));
	_dim = dim_of(_sig);

	_a4 = 0D0;
	if (size(_calibArr) > 4)
		_a4 = _calibArr[4];

	_calData = _calibArr[0] + _calibArr[1] * _data
		+ _calibArr[2] * power(_data, 2)
		+ _calibArr[3] * power(_data, 3)
		+ _a4 * power(_data, 4);

	return (make_signal(_calData,, _dim));
}
