fun public triangle(in _end, in _dt, in _freq, in _amp)
{
    _nsamples = long(_end/_dt + 0.5);
    if(_freq <= 0)
    	return(zero(_nsamples, 0.));
    _rsamples = 1./(_freq * _dt);
    _x = (0 : _nsamples : 1);
    _ramp = mod(_x, _rsamples);
    _tria = merge(_ramp, _rsamples - _ramp, _ramp < (_rsamples/2.))-_rsamples/4.;
    return(_tria * 4. * _amp / _rsamples);
}
