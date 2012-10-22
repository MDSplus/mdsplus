FUN PUBLIC JavaGetMinMax(in _sig)
{
	write(*, 'JavaGetMinMax ', _sig);
    _xmin = 0.;
    _xmax = 0.;
    _status = JavaMds->JavaGetMinMax(_sig, ref(_xmin), ref(_xmax));
	write(*, 'JavaGetMinMax status: ', _status);
    if(_status == 0)
        return([]);
    return([_xmin, _xmax]);
}
