public fun XrayRealGain(in _gainPath, in _trPath, in _ampType,  in _gainCalPath)  
{
	_gain_out = -1;

    _gain = data(_gainPath);
    _tr   = data(_trPath);

	_gains = data(_gainCalPath);

	_tableGain = [1, 2, 5, 10];
	_tableTr21 = [1e4, 1e5, 1e6, 1e7, 1e8];
	_tableTr22 = [1e5, 1e6, 1e7, 1e8, 1e9];

    _gainIdx = -1;

    for(_i = 0; _i < size(_tableGain); _i++)
    {
		if(_tableGain[_i] == _gain) _gainIdx = _i;
    }

    _trIdx = -1;

	if(_ampType == 22)
	{

		for(_i = 0; _i < size(_tableTr22); _i++)
		{
			if(_tableTr22[_i] == _tr) _trIdx = _i;
		}

	}
	else
	{
		for(_i = 0; _i < size(_tableTr21); _i++)
		{
			if(_tableTr21[_i] == _tr) _trIdx = _i;
		}
	}

	if(_trIdx != -1 && _gainIdx != -1)
	{
		_gain_out = _gains[ _gainIdx + _trIdx * 4 ];
	}

	write(*,"Gain ", _gain, " Tr ", _tr, " calib ", _gain_out);


	return (_gain_out);

}
