public fun EndWave(as_is _node, _delta)
{
  _nid=getnci(_node,"NID_NUMBER");
  _num=0;
  _status = TreeShr->TreeGetNumSegments(val(_nid),ref(_num));
  if (!(_status & 1)) return(*);
  _start=0;
  _end=0;
  _status=TreeShr->TreeGetSegmentLimits(val(_nid),val(_num-1),xd(_start),xd(_end));
  return (_end - _delta);
}
