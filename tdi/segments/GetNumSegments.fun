public fun GetNumSegments(as_is _node) {
  _nid=getnci(_node,"NID_NUMBER");
  _num=0;
  _status = TreeShr->TreeGetNumSegments(val(_nid),ref(_num));
  if (!(_status & 1)) _num=0;
  return(_num);
}
