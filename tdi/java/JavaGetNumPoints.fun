FUN PUBLIC JavaGetNumPoints(in _sig, in _xmin, in _xmax)
{
	write(*, 'JavaGetNumPoints', _sig);
    	_num_points = JavaMds->JavaGetNumPoints(_sig, float(_xmin), float(_xmax));
	write(*, 'NUM POINTS: ', _num_points);
	
      return([_num_points]);
}
