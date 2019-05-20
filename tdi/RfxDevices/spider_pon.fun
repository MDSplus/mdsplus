public fun spider_pon()
{
   write(*, 'SPIDER PON');
   tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/phase pulse_on');
}
