public fun GetSegmentInfo(as_is _node, in _start, in _end) {
_nid=getnci(_node,"NID_NUMBER");
_out=*;
XTreeShr->XTreeGetSegmentList(val(_nid),ref(_start),ref(_end),xd(_out));
return(_out);
}
