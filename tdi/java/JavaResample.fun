FUN PUBLIC JavaResample(in _y, optional in _x, in _xmin, in _xmax)
{

   private _MAX_POINT = 2000.0;
   
   _out = 0;
 
	if( present(_x) )
	{
		_in_sig = make_signal(_y, *,_x);	
	}
	else
	{
		_in_sig = _y;
		_x = data(dim_of(_y));
	}
	
	_xmax1 = maxval(_x);
	_xmin1 = minval(_x);
	

	if( _xmax >  _xmax1 ) _xmax = _xmax1;
	if( _xmin <  _xmin1 ) _xmin = _xmin1;
	
	_n_point = size(pack(_x, ((_x ge _xmin) and (_x le _xmax))));

	if( _n_point >  _MAX_POINT )
	{
		_dt = ( _xmax - _xmin)/_MAX_POINT;
		_out = resample(_in_sig, _xmin, _xmax, _dt);
	}
	else
	{
		_out = _in_sig;
	}

        return (_out);

};
