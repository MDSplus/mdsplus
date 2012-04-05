public fun iseps_pon()
{
   write(*, 'ISEPS PON');
   tcl('dispatch/command/server=localhost:8001 dispatch/phase pulse_on');
}
