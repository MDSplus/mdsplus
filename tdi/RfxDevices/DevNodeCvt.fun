public fun DevNodeCvt(in _nid, in _offset, in _table1,  in _table2, out _converted)

{
	_status = 1;
	_d = if_error(data(DevNodeRef(_nid, _offset)), _status = 0);
	if(0 == _status)
	{
    		_converted = 0;
		return(0);
	}




    _found = -1;

    for(_i = 0; _i < size(_table1); _i++)
    {
	if(_table1[_i] == _d) _found = _i;
    }
    if(_found != -1)
    {
        _converted = _table2[_found];
        return (1);
    }
    _converted = 0;
    return (0);
}

	

  