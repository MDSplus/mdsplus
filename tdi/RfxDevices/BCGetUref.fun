public fun BCGetUref(in _chan_id, in _u_repeat, in _u1, in _u2, in _u_delay, in _u_points)
{

write(*, "BCGetUref ");

   _u1r        =  zero(_u_repeat, 0.0);
   _u2r        =  zero(_u_repeat, 0.0);
   _u2r_u1r    =  zero(_u_repeat, 0.0);
   _ur_signals = zero(_u_repeat * _u_points, 0.0);

   _status = Tomo->BCGetUref( val(_chan_id), val(_u_repeat), float(_u1), float(_u2), float(_u_delay), val(_u_points), ref(_u1r), ref(_u2r), ref(_u2r_u1r), ref(_ur_signals) );

   if(!_status)
     return ([_u1r, _u2r, _u2r_u1r, _ur_signals]);

   return(_status);
}
