FUN PUBLIC JavaGetNumPoints(in _sig, in _xmin, in _xmax, in _nlimit)
{
	write(*, 'JavaGetNumPoints ', _sig);
    	_num_points = JavaMds->JavaGetNumPoints(_sig, float(_xmin), float(_xmax), _nlimit);
	write(*, 'NUM POINTS: ', _num_points);
	
      return([_num_points]);
}
