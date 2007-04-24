public fun PutSegment(as_is _node, in _idx, in _data) {
   _nid=getnci(_node,"NID_NUMBER");
   return(TreeShr->TreeBeginSegment(val(_nid),val(_idx),descr(data(_data))));
}
