public fun MarteGetUserArrayFromName(in _rootName, in _name, in _idx)
{
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
    _sig = data(_samples)[_idx];
    return (make_signal(set_range(size(_sig), _sig),,dim_of(_samples)));
}

