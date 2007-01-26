public fun PutTimestampedSegment(as_is _node, in _timestamp, in _data) {
  _nid = getnci(_node,"NID_NUMBER");
  return(TreeShr->TreePutTimestampedSegment(val(_nid),quadword(_timestamp),descr(data(_data))));
}
