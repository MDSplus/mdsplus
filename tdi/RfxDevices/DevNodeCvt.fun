public fun DevNodeCvt(in _nid, in _offset, in _table1,  in _table2, out _converted)

{
    _d = data(DevNodeRef(_nid, _offset));
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

	

  