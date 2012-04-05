public fun MarteGetModule(in _re, in _im, in _idx)
{
    _reVals = data(_re)[_idx];
    _imVals = data(_im)[_idx];
write(*, size(_reVals));
    return (make_signal(sqrt(_reVals*_reVals + _imVals * _imVals),,dim_of(_im)));
}

