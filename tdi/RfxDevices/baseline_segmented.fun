public fun baseline_segmented(as_is _node, optional in _npoints)
{
      _nid=getnci(_node,"NID_NUMBER");
      _first_seg=*;
      _dim=*;
      _status=TreeShr->TreeGetSegment(val(_nid),val(0),xd(_first_seg),xd(_dim));
      if(!(_status & 1))
         return (*);
      _x = data(dim_of(_node));
      _y = data(_node);


	if(present(_npoints)) /* Number of points passed, or none */
	{
	    _actpoints = _npoints;
	}
	else
	{
	    _actpoints = 100;
	}
        if(_actpoints > size(_first_seg))
	    _actpoints = size(_first_seg);


	_sum = 0.;
	_sum = sum(_first_seg[0.._actpoints-1]);
	_offset = _sum / _actpoints;

	_y = _y - _offset;

	return(make_signal(_y, * , _x));
}

