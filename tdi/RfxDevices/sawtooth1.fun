fun public sawtooth1(in _end, in _dt, in _freq, in _amp)
{
    _nsamples = long(_end/_dt + 0.5);
    _rsamples = 1./(_freq * _dt);
    if(_freq <= 0)
        return (zero(_nsamples, 0.));
    _x = (0 : _nsamples : 1);
    _ramp = _rsamples - mod(_x, _rsamples);
    return(2 * _amp *(_ramp/_rsamples - 0.5));
}
