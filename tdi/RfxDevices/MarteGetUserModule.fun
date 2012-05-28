public fun MarteGetUserModule(as_is _marteRoot, in _name, in _idx, optional in _sig_re, optional in _sig_im, optional in _sig_dim)
{
    if(present(_sig_re))
    {
        _reVals = data(_sig_re[_idx]);
        _imVals = data(_sig_im[_idx]);
        return (make_signal(sqrt(_reVals*_reVals + _imVals * _imVals),,_sig_dim));
    }
    else
    {
        _rootName = getnci(_marteRoot, 'FULLPATH');
        _re = MarteGetUserArrayFromName(_rootName, _name//'_re', _idx);
        _im = MarteGetUserArrayFromName(_rootName, _name//'_im', _idx);
        _reVals = data(_re);
        _imVals = data(_im);
        return (make_signal(sqrt(_reVals*_reVals + _imVals * _imVals),,dim_of(_im)));
    }
}

