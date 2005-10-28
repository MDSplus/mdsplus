public fun RfxShotsPowerMax(in _shots)
{
	_numShot = size(_shots);
	_potMax=[];
	_enerMax=[];
	for(_i = 0; _i < _numShot; _i++)
	{
	
	write(*, _shots[_i]);
	
		_status = tcl("set tree rfx/shot="//_shots[_i]);
		if(_status & 1)
		{
			_vmt = execute('resample(\\dequ::vmvt001_vd2va, -0.1, 0.3, 2e-4)');
			_vmt001 = if_error( _vmt == "bad resample signal in" ? 0 : 1, _vmt);
			_it     = if_error( execute('resample(\\a::it, -0.1, 0.3, 2e-4)'), 0);
			_p = (_vmt001 * _it) / (40e6);
			_potMax = [_potMax, maxval( _p)];
		}
		else
			_potMax = [_potMax, 0];
		
		tcl("close");
		
	}
	return ( make_signal(_potMax,,_shots) );
	
}