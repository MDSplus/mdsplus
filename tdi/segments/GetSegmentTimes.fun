public fun GetSegmentTimes(as_is _node, out _num, out _start, out _end) {
_nid=getnci(_node,"NID_NUMBER");
_num=0L;
_start=*;
_end=*;
return(XTreeShr->TreeGetSegmentTimesXd(val(_nid),ref(_num),xd(_start),xd(_end)));
}
