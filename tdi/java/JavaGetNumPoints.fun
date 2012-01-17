FUN PUBLIC JavaGetMinMax(in _sig, in _xmin, in _xmax)
{
	write(*, 'JavaGetNumPoints', _sig);
    	_num_points = JavaMds->JavaGetNumPoints(_sig, float(_xmin), float(_xmax));
      return(num_points);
}
