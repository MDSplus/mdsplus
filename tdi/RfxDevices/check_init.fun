public fun check_init(_increment)
{
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    if(!_increment)
 	return (_shotid);
    _shotid = _shotid + 1;
    tcl('set tree spider');
    tcl('create pulse '//_shotid);
    tcl('set current spider ' //_shotid);
    return (_shotid);
}
