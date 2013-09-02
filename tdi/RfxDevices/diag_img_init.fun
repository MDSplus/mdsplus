public fun diag_img_init()
{
    write(*, 'DIAG_IMG INIT');
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    tcl('dispatch/command/server=localhost:8007 set tree spider ' // _shotid);
    tcl('dispatch/command/server=localhost:8007 dispatch/build PLANTS.DIAGNOSTICS.DIAG_IMG');
    tcl('dispatch/command/server=localhost:8007 dispatch/phase init');
}
