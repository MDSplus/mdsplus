public fun spider_init()
{
    write(*, 'SPIDER INIT');
    _shotid = TreeShr->TreeGetCurrentShotId('spider');

/* Pulse file creation in ready state */

    _shotid = _shotid + 1; 
    tcl('set tree spider');
    tcl('create pulse '//_shotid);
    tcl('set current spider ' //_shotid);

/**************************************/

    tcl('dispatch/command/server=scpsl.nbtf:8001 set tree spider ' // _shotid);
    tcl('dispatch/command/server=scpsl.nbtf:8001 dispatch/build');
    tcl('dispatch/command/server=scpsl.nbtf:8001 dispatch/phase pulse_preparation');
    wait(2);
    tcl('dispatch/command/server=scpsl.nbtf:8001 dispatch/phase init');
    wait(2);
    tcl('dispatch/command/server=scpsl.nbtf:8001 dispatch/phase ready');
}
