public fun baseline(in _sig, optional in _in_points)
{

    if(!present(_in_points))
		_in_points = 10;
	_x = data(dim_of(_sig));
 	_y = data(_sig);

	_points = _in_points;
	if(_points > size(_y))
		_points = size(_y);

	if(_points < 1)
		_points = 1;

	_sum = 0.;
	for(_i = 0; _i < _points; _i++)
		_sum = _sum + _y[_i];

	_sum = _sum / _points;
	_y = _y - _sum;

	return(make_signal(_y, * , _x));
}

