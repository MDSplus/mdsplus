public fun BCGetTauOpt(in _module, in _zero_flag, in _gains, in _filter, in _refPhase, in _num_points)
{

write(*, "BCGetTauOpt", _num_points);

    _signals = zero(4 * _num_points, 0.0);

    write(*," gains ", _gains);
    write(*," filter ", _filter);
    write(*," refPhases ", _refPhase );	

    _status = Tomo->BCGetTauOpt(val(_module), val(_zero_flag), _gains, _filter, _refPhase, val(_num_points), ref(_signals));

   if(!_status)
     return ( _signals );

   return(_status);
}
