public fun GetSegmentLimits(as_is _node, in _idx, optional out _start, optional out _end) {
  _out = present(_start) || present(_end);
  _nid=getnci(_node,"nid_number");
  _start=0;
  _end=0;
  _status=TreeShr->TreeGetSegmentLimits(val(_nid),val(_idx),xd(_start),xd(_end));
  if (_out) {
    return(_status);
  } else if (_status & 1) {
     return([if_error(evaluate(_start),-1),if_error(evaluate(_end),-1)]);
  } else return(*);
}
