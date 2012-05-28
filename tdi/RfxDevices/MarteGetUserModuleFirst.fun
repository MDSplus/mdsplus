public fun MarteGetUserModuleFirst(as_is _marteRoot, in _name, in _idx, out _sig_re, out _sig_im, out _sig_dim)
{
    _rootName = getnci(_marteRoot, 'FULLPATH');
    _sig_re = data(MarteGetUserDataFromName(_rootName, _name//'_re'));
    _sig_im = data(MarteGetUserDataFromName(_rootName, _name//'_im'));
    _sig_dim = data(dim_of(MarteGetUserDataFromName(_rootName, _name//'_im')));
    _re = MarteGetUserArrayFromName(_rootName, _name//'_re', _idx);
    _im = MarteGetUserArrayFromName(_rootName, _name//'_im', _idx);
   _reVals = data(_re);
    _imVals = data(_im);
    return (make_signal(sqrt(_reVals*_reVals + _imVals * _imVals),,dim_of(_im)));
}

