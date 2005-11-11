fun public square_wave(in _end, in _dt, in _freq, in _amp)
{
    _nsamples = long(_end/_dt + 0.5);
    _rsamples = 1./(_freq * _dt);
    _x = (0 : _nsamples : 1);
    _ramp = mod(_x, _rsamples);
    return(merge(spread(_amp, 0, _nsamples), spread(-_amp, 0, _nsamples), _ramp < (_rsamples/2.)));
}
