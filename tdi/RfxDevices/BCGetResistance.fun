public fun BCGetResistance(in _chan_id, in _u_repeat)
{

write(*, "BCGetResistance");

   _R12 = zero(_u_repeat, 0.0);
   _R23 = zero(_u_repeat, 0.0);
   _R24 = zero(_u_repeat, 0.0);
   _R14 = zero(_u_repeat, 0.0);

   _status = Tomo->BCGetResistance(val(_chan_id), val(_u_repeat), ref(_R12), ref(_R23), ref(_R24), ref(_R14));

   if(!_status)
   {
     return ([_R12, _R23, _R24, _R14]);
   }
   return (_status);  
}
