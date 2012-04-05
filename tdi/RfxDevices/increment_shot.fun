public fun increment_shot(_experiment)
{
    _shotid = TreeShr->TreeGetCurrentShotId(_experiment);
    tcl('set tree ' // _experiment);
    tcl('create pulse '//_shotid);
    _shotid = _shotid + 1;
    tcl('set current '//_experiment // '  ' //_shotid);
    return (_shotid);
}
