public fun BCGetUmeas(in _chan_id, in _u_repeat, in _u1, in _u2, in _u_delay, in _u_points)
{
write(*, "BCGetUmeas");

   _u1m = zero(_u_repeat, 0.0);
   _u2m = zero(_u_repeat, 0.0);
   _u2m_u1m = zero(_u_repeat, 0.0);
   _um_signals = zero(_u_points * _u_repeat, 0.0);

   _status = Tomo->BCGetUmeas(val(_chan_id), val(_u_repeat), _u1, _u2, _u_delay, val(_u_points), ref(_u1m), ref(_u2m), ref(_u2m_u1m), ref(_um_signals));

   if(!_status)
     return ([_u1m, _u2m, _u2m_u1m, _um_signals]);

   return(_status);
}
