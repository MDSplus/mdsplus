public fun diag_img_pon()
{
    write(*, 'DIAG_IMG PON');
    tcl('dispatch/command/server=localhost:8007 dispatch/phase pulse_on');
}
