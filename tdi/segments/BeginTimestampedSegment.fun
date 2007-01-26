public fun BeginTimestampedSegment(as_is _node, in _array, optional _idx) {
  _nid=getnci(_node,"NID_NUMBER");
  if (!present(_idx)) {
    _idx=-1;
  }
  return(TreeShr->TreeBeginTimestampedSegment(val(_nid),descr(data(_array)),val(_idx)));
}
