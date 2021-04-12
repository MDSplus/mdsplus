public fun spider_init()
{
    write(*, 'SPIDER INIT');
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    _shotid = _shotid + 1; 

    tcl('dispatch/command/server=localhost:8001 set tree spider ');
    tcl('dispatch/command/server=localhost:8001 create pulse ' // _shotid);
    tcl('dispatch/command/server=localhost:8001 set current ' // _shotid);

    tcl('dispatch/command/server=soserver.nbtf:8001 set tree spider ' // _shotid);
    tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/build');
    tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/phase pulse_preparation');
    wait(2);
    tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/phase init');
    wait(2);
    tcl('dispatch/command/server=soserver.nbtf:8001 dispatch/phase ready');
}
