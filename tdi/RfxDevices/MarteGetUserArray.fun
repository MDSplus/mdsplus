public fun MarteGetUserArray(as_is _marteRoot, in _name, in _idx, optional in _signal, optional in _dimension)
{
    if(present(_signal))
    {
	return (make_signal(_signal[_idx],,_dimension));
    }
    _rootName = getnci(_marteRoot, 'FULLPATH');
    _names = data(build_path(_rootName // '.SIGNALS.USER:NAMES'));
    _numNames = size(_names);
    for(_i = 0; _i < _numNames; _i++)
    {
	if(trim(_names[_i]) == _name)
	    break;
    }
    if(_i == _numNames)
	return('NODE NOT FOUND');
    _i = _i + 1;
    if(_i < 10)
	_samples = build_path(_rootName // '.SIGNALS.USER.USER_00' // text(_i,1) // ':DATA');
    else
    {
	if(_i < 100)
	    _samples = build_path(_rootName // '.SIGNALS.USER.USER_0' // text(_i,2) // ':DATA');
	else
	    _samples = build_path(_rootName // '.SIGNALS.USER.USER_' // text(_i,3) // ':DATA');
    }
    _sig =  (make_signal(data(_samples)[_idx],,dim_of(_samples)));
    return (_sig);
}

