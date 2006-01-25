public fun RfxRunsMaxTemp(in _sonda, in _runs)
{
	_numRun = size(_runs);
	_maxTemp=[];
	for(_i = 0; _i < _numRun; _i++)
	{
		_status = tcl("set tree rfx_run/shot="//_runs[_i]);
		if(_status & 1)
		{
			_t = [];
			for(_j = 1; _j <= 8; _j++)
			{
				_t = [ _t, maxval(RfxMaxTemperature(_sonda, _j)) ];
			}
		    _maxTemp = [_maxTemp, maxval( _t)];
		} else
			_maxTemp = [_maxTemp, 0];
			
		tcl("close");
		
	}
	return (make_signal(_maxTemp,,_runs));
	
}