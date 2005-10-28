public fun RfxShotsPowerMean(in _shots)
{
	_numShot = size(_shots);
	_potMax=[];
	_enerMax=[];
	for(_i = 0; _i < _numShot; _i++)
	{
		_error = 0;

		_status = tcl("set tree rfx/shot="//_shots[_i]);
		if(_status & 1)
		{
			_vmt001 = execute('\\dequ::vmvt001_vd2va');
			_maxVal = if_error( maxval( _vmt001), _error = 1);
			if(_error == 1)
			{
				_potMax = [_potMax, 0];
				continue;
			}
			_it     = if_error( _it = execute('\\a::it'), _it, 0);

			_maxIdxs = ( data( _vmt001 ) > (_maxVal - 5) ); 
			_timeMax = sum( dim_of(_vmt001) * _maxIdxs ) / sum(_maxIdxs);
			_vmt = resample(_vmt001, _timeMax, 0.1, 2e-4);
			_vmt001 = if_error( _vmt == "bad resample signal in" ? 0 : 1, _vmt);
			
			if( ! _error)
			{
				_limIdxs = ( data( _vmt001 ) < 30 );
				_limIdxs = firstloc(_limIdxs);
				_firstTime = sum(dim_of(_vmt001) * _limIdxs);
			
				_vmt001 = resample(_vmt001,   _firstTime , _firstTime + 0.08 , 2e-4);
				_vmt001 = if_error( _vmt001 == "bad resample signal in" ? 0 : 1, _vmt001);
				_it     = resample( _it,   _firstTime , _firstTime + 0.08 , 2e-4);
				_it     = if_error( _it == "bad resample signal in" ? 0 : 1, _it);
			
				_p = (_vmt001 * _it) / (40e6);
				_potMax = [_potMax, mean( _p)];
			} else
				_potMax = [_potMax, 0];
		}
		else
			_potMax = [_potMax, 0];
		
		tcl("close");
		
	}
	return ( make_signal(_potMax,,_shots) );
	
}