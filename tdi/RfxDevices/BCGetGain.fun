public fun BCGetGain(in _chan_id, in _gain, in _filter, in _refPhase, in _excitation, in _num_points) 
{

write(*, "BCGetGain");

   _signal_zero = zero(_num_points, 0.0); 
   _signal      = zero(_num_points, 0.0); 

   _status = Tomo->BCGetGain(val(_chan_id), val(_gain), val(_filter), val(_refPhase), val(_excitation), val(_num_points), ref(_signal_zero), ref(_signal));

   if(!_status)
     return ([_signal_zero, _signal]);

   return( _status );
}
