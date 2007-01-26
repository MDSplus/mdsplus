public fun testsegments(as_is _node, in _num, in _segsize, in _row)
{
  _nid=text(getnci(_node,"NID_NUMBER"));
  _vals=_row*0.+1;
  execute("TreePutRecord("//_nid//")");
  _pts="PutTimestampedSegment("//_nid//",_i,_vals*_i)";
  if (rank(_row)==0) {
	_bts="BeginTimestampedSegment("//_nid//
             ",zero(["//text(_segsize)//"]))";
  } else {
    _bts="BeginTimestampedSegment("//_nid//",zero([";
    for (_i=0;_i<rank(_row);_i++) {
      _bts=_bts//text(ubound(_row)+1)//",";
    }
    _bts=_bts//text(_segsize)//"]))";
  }
  write(*,"_bts=",_bts);
  write(*,"_pts=",_pts);
  tcl('init timer');
  _status=execute(_bts);
  if (!(_status & 1)) write(*,"BeginTimestampedSegment failed: ",_status);
  for (_i=0;_i<_num;_i++) {
    _status = execute(_pts);
    if (_status == 265388306) {
      execute(_bts);
      execute(_pts);
    } else if (!(_status & 1)) {
	write(*,"PutTimestamedSegment failed: ",_status);
    }
  }
  tcl('show timer');
  write(*,"Total of ",_numseg=execute("GetNumSegments("//_nid//")")," written");
  return(_numseg);
}
