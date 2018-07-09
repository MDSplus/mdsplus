public fun GetSegmentScale(as_is _node) {
  _nid=getnci(_node,"nid_number");
  _scale=*;
  _status=TreeShr->TreeGetSegmentScale(val(_nid),xd(_scale));
  if (_status & 1) {
     return(_scale);
  } else return(*);
}
