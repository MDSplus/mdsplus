public fun ISIS_TT_TAM(in _dt)
{
	private fun ISISTMaxTemp(in _sig, in _timeInterval)
	{
		_s1 = execute( _sig );
		
		_s = _s1[ 1.0 : * : * ];
		
		_sf = filter(_s, 3.);
		
		_maxIdx = maxloc(_sf);
		
		_timeMax = dim_of(_sf)[ _maxIdx ];
		
		_mean = mean(_sf[ _timeMax - _timeInterval : _timeMax + _timeInterval : *]);
		
		return ( _mean );
	}
	
	_sig = ["\\EDAV::TT021_TA", "\\EDAV::TT081_TA", "\\EDAV::TT141_TA", "\\EDAV::TT321_TA", "\\EDAV::TT381_TA", "\\EDAV::TT441_TA", "\\EDAV::TT501_TA", "\\EDAV::TT681_TA"];
	_pos = [5, 35, 65, 155, 185, 215, 245, 335];
	_val = [];
	
	for(_i = 0; _i < 8; _i++)
	{
		_val = [ _val, ISISTMaxTemp( _sig[_i] , _dt) ];
	}
	
	return ( make_signal(make_with_units(_val, "Celsius"),, make_with_units(_pos, "Deg") ) );
	
}