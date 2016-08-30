public fun iseps_post_pulse_check()
{
    write(*, 'ISEPS FINISH PULSE');
    tcl('dispatch/command/server=localhost:8001 dispatch/phase post_pulse_check');
}
