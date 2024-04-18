public fun BeginSegmentResampled(as_is _node, in _start, in _end, optional as_is _dim, in _array, optional _idx, as_is _res_node, in _res_factor) {
  _nid=getnci(_node,"NID_NUMBER");
  _res_nid=getnci(_res_node,"NID_NUMBER");
  if (!present(_idx)) {
    _idx=-1;
  }
  if (!present(_dim)) {
    return(TreeShr->TreeBeginSegmentResampled(val(_nid),descr(_start),descr(_end), val(0) ,descr(data(_array)),val(_idx), val(_res_nid), val(_res_factor)));
  } else {
    return(TreeShr->TreeBeginSegmentResampled(val(_nid),descr(_start),descr(_end),xd(_dim),descr(data(_array)),val(_idx)));
  }
}

