public fun spider_finish_pulse()
{
    write(*, 'SPIDER FINISH PULSE');
    tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/phase post_pulse_check');
}
