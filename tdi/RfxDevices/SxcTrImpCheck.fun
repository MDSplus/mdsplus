public fun SxcTrImpCheck(in _logico, in _trImp)
{
	_m = SxcChMapping();

	_id = TomoChanId(_logico, 1, _m);
	
	if(TomoAmpType(_id) == 21)
	{
		if(_trImp >= 1e9)
			return (0);
	}
	 
	if(TomoAmpType(_id) == 22)
	{
		if(_trImp <= 1e4)
			return (0);
	}

	return (1);
}
