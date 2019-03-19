public fun PutSegment(as_is _node, optional in _idx, in _data) {
  _nid=getnci(_node,"NID_NUMBER");
  if (!present(_idx)) _idx=-1;
  return(TreeShr->TreePutSegment(val(_nid),val(_idx),descr(data(_data))));
}
