public fun GetSegment(as_is _node, in _idx) {
  _nid=getnci(_node,"NID_NUMBER");
  _data=0;
  _dim=0;
  _status=TreeShr->TreeGetSegment(val(_nid),val(_idx),xd(_data),xd(_dim));
  if (_status & 1) {
    return(make_signal(_data,*,_dim));
  } else {
    return(*);
  }
}
