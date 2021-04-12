public fun check_init(_increment)
{
    _shotid = TreeShr->TreeGetCurrentShotId('spider');
    if(!_increment)
 	return (_shotid);
    _shotid = _shotid + 1;
    return (_shotid);
}
