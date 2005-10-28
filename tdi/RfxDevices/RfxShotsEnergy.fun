public fun RfxShotsEnergy(in _shots)
{
	_numShot = size(_shots);
	_energy=[];
	for(_i = 0; _i < _numShot; _i++)
	{
		_status = tcl("set tree rfx/shot="//_shots[_i]);
		if(_status & 1)
		{
			_e = if_error( execute('integrate(build_signal(\\dequ::vmvt001_vd2va*\\dequ::vmrg120_vi2va,,dim_of(\\dequ::vmvt001_vd2va)))*1e-6'), 0);
		    _energy = [_energy, maxval( _e)];
		} else
			_energy = [_energy, 0];
			
		tcl("close");
		
	}
	return (make_signal(_energy,,_shots));
	
}