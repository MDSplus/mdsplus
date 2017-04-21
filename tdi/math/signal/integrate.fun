public fun integrate(in _sig, optional in _base_points)
{

/* Trapezoidal integration. If _base_point passed, the average of the first _base_points samples is subtracted 
to the signal before integration 
*/

    if(present(_base_points))
		_curr_sig = baseline(_sig, _base_points);
	else
		_curr_sig = _sig;

	_x = data(dim_of(_curr_sig));
 	_y = data(_curr_sig);


	_y1 = _y[0:(size(_y) - 2)];
	_y2 = _y[1:(size(_y) - 1)];

	_x1 = _x[0:(size(_y) - 2)];
	_x2 = _x[1:(size(_y) - 1)];
	_delta = _x2 - _x1;

	_integ = (_y1 + _y2)*_delta/2;

	return(make_signal(accumulate(_integ), * , _x));
}

