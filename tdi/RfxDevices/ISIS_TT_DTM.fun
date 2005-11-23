public fun ISIS_TT_DTM(in _dt)
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
	
	_sig = ["\\EDAV::TT021_DT", "\\EDAV::TT081_DT", "\\EDAV::TT141_DT", "\\EDAV::TT321_DT", "\\EDAV::TT381_DT", "\\EDAV::TT441_DT", "\\EDAV::TT501_DT", "\\EDAV::TT681_DT"];
	_pos = [5, 35, 65, 155, 185, 215, 245, 335];
	_val = [];
	
	for(_i = 0; _i < 8; _i++)
	{
		_val = [ _val, ISISTMaxTemp( _sig[_i] , _dt) ];
	}
	
	return ( make_signal(make_with_units(_val, "K"),, make_with_units(_pos, "Deg" )) );
	
}