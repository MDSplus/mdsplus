FUN PUBLIC JavaResample(as_is(_y), as_is( _x), in _xmin, in _xmax, in _dt)
{

   private _MAX_POINT = 2000;
   write(*, 'SONO LA JAVA RESAMPLE');
   return (JavaMds->JavaResample:DSC(_y, _x, float(_xmin), float(_xmax), float(_dt)));
   
   _out = 0;

	_dt = ( _xmax - _xmin)/_MAX_POINT;
	TreeShr->TreeSetTimeContext(descr(_xmin),descr(_xmax), descr(_dt));

	_y = evaluate(_y);
 

	_already_resampled = XTreeShr->XTreeTestTimedAccessFlag();

	if(_already_resampled)
	{
		if( present(_x) )
		{
			return (make_signal(_y, *,_x));	
		}
		else
		{
			return (_y);
		}
	}
	else
	{
 
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
	}
}



