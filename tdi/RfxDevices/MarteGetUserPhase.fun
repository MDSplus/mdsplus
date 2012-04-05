public fun MarteGetUserPhase(as_is _marteRoot, in _name, in _idx)
{
    _rootName = getnci(_marteRoot, 'FULLPATH');
    _re = MarteGetUserArrayFromName(_rootName, _name//'_re', _idx);
    _im = MarteGetUserArrayFromName(_rootName, _name//'_im', _idx);
    _reVals = data(_re);
    _imVals = data(_im);
    _size = size(_reVals);
    return (make_signal(atan2(_imVals, _reVals),,dim_of(_im)));
}

