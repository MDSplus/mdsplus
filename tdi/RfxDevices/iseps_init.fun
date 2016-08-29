public fun iseps_init()
{
    write(*, 'ISEPS INIT');
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    tcl('dispatch/command/server=localhost:8001 set tree spider ' // _shotid);
    tcl('dispatch/command/server=localhost:8001 dispatch/build PLANTS.POWER.ISEPS');
/*
    tcl('dispatch/command/server=localhost:8001 dispatch/phase finish_pulse');
*/
    tcl('dispatch/command/server=localhost:8001 dispatch/phase init');
    tcl('dispatch/command/server=localhost:8001 dispatch/phase ready');
}
