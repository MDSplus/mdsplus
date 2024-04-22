public fun UpdateSegment(as_is _node, in _start, in _end, in _time, in _idx) {
  _nid=getnci(_node,"NID_NUMBER");
  if(_idx == -1)
  {
      _segIdx = GetNumSegments(_node) - 1;
  }
  else
  {
    _seg_idx = _idx;
  }
  return(TreeShr->TreeUpdateSegment(val(_nid),descr(_start),descr(_end), descr(_time), val(_seg_idx)));
}
