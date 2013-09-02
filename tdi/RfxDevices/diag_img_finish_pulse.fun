public fun diag_img_finish_pulse()
{
    write(*, 'DIAG_IMG FINISH PULSE');
    tcl('dispatch/command/server=localhost:8007 dispatch/phase finish_shot');
}
