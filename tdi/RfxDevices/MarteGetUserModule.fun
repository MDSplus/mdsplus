public fun MarteGetUserModule(as_is _marteRoot, in _name, in _idx)
{
    _rootName = getnci(_marteRoot, 'FULLPATH');
    _re = MarteGetUserArrayFromName(_rootName, _name//'_re', _idx);
    _im = MarteGetUserArrayFromName(_rootName, _name//'_im', _idx);
    _reVals = data(_re);
    _imVals = data(_im);
    return (make_signal(sqrt(_reVals*_reVals + _imVals * _imVals),,dim_of(_im)));
}

