public fun cvttime(optional in _qtime)
{
/* cvttime will convert MDSplus quadword time to seconds since the Epoch */
   _t = 0D0;
   _t2=present(_qtime) ? MdsShr->LibCvtTim(_qtime,ref(_t)) : 
                            MdsShr->LibCvtTim(val(0),ref(_t));
   return(_t);
}
