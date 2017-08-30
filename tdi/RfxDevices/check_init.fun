public fun check_init(_increment)
{
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    if(!_increment)
 	return (_shotid);
    _shotid = _shotid + 1;

/*

Pulse file creation moved in spider_init.fun
The function is colled in BOS check state 
This modification is required to allow redo of BOS check State
without the creation of pulse file

    tcl('set tree spider');
    tcl('create pulse '//_shotid);
    tcl('set current spider ' //_shotid);
*/
    return (_shotid);
}
