public fun iseps_finish_pulse()
{
    write(*, 'ISEPS FINISH PULSE');
/*
    tcl('dispatch/command/server=localhost:8001 dispatch/phase finish_shot');
*/
    tcl('dispatch/command/server=localhost:8001 dispatch/phase post_pulse_check');
}
