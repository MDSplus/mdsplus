public fun BCGetTauEl(in _chan_id, in _period, in _u1, in _u2, in _delayStart, in _num_points)
{

write(*, "BCGetTauEl", _num_points);

   _signals = zero(_num_points, 0.0);

   _status = Tomo->BCGetTauEl(val(_chan_id), val(_period),  _u1, _u2, _delayStart, val(_num_points), ref(_signals));

   if(!_status)
     return (_signals);

   return(_status);
}

