public fun baseline(in _sig, optional in _start, optional in _end)
{

	_x = data(dim_of(_sig));
 	_y = data(_sig);


	if(!present(_end)) /* Number of points passed, or none */
	{
		if(!present(_start)) /* 100 baseline points */ 
			_in_points = 10;
		else
			_in_points = _start;

		_points = _in_points;
		if(_points > size(_y))
			_points = size(_y);

		if(_points < 1)
			_points = 1;

		_sum = 0.;
		
		/*
		for(_i = 0; _i < _points; _i++)
			_sum = _sum + _y[_i];
		*/
		
		_sum = sum(_y[0.._points-1]);
		_offset = _sum / _points;
			
		_offset = _sum / _points;
	}
	else /* Start and end passed, offset computed on this interval */
	{
		_ofs_x = pack(_x, ((_x ge _start) and (_x le _end)));
		_offset = sum(_sig[_ofs_x])/size(_ofs_x);
	}

	_y = _y - _offset;

	return(make_signal(_y, * , _x));
}

