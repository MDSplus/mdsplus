public fun spider_pon()
{
   write(*, 'SPIDER PON');
   tcl('dispatch/command/server=scpsl.nbtf:8001 dispatch/phase pulse_on');
}
