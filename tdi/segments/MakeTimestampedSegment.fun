public fun MakeTimestampedSegment(as_is _node, in _timestamp, in _data) {
  _nid = getnci(_node,"NID_NUMBER");
  return(TreeShr->TreeMakeTimestampedSegment(val(_nid),quadword(_timestamp),descr(data(_data)), val(-1), val(shape(_data, 0))));
}
