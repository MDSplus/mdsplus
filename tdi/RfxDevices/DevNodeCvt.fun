public fun DevNodeCvt(in _nid, in _offset, in _table1,  in _table2, out _converted)
{
    _d = data(DevNodeRef(_nid, _offset));

    for(_i = 0; _i < size(_table1) && (!(_table1[_i] == _d)); _i++) ;
    if(_i < size(_table1))
    {
        _converted = _table2[_i];
        return (1);
    }
    return (0);
}
	
  