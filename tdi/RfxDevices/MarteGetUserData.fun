public fun MarteGetUserData(as_is _marteRoot, in _name)
{
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
	return (build_path(_rootName // '.SIGNALS.USER.USER_00' // text(_i,1) // ':DATA'));
    if(_i < 100)
	return (build_path(_rootName // '.SIGNALS.USER.USER_0' // text(_i,2) // ':DATA'));
    return (build_path(_rootName // '.SIGNALS.USER.USER_' // text(_i,3) // ':DATA'));
}

