public fun MakeSegmentMinMax(as_is _node, in _start, in _end, optional as_is _dim, in _array, optional _idx, in _rows_filled, as_is _res_node, in _res_factor) {
  _nid=getnci(_node,"NID_NUMBER");
  _res_nid=getnci(_res_node,"NID_NUMBER");
  if (!present(_idx)) {
    _idx=-1;
  }
  if (kind(_array) == 217) { /*is opaque*/
    if (!present(_dim)) abort("Missing dim while storing OPAQUE segment.");
    return(TreeShr->TreeMakeSegmentMinMax(val(_nid),descr(_start),descr(_end),xd(_dim),xd(_array),val(_idx),val(_rows_filled), val(_res_nid), val(_res_factor)));
  } else {
    if (!present(_dim)) {
      return(TreeShr->TreeMakeSegmentMinMax(val(_nid),descr(_start),descr(_end), val(0) ,descr(data(_array)),val(_idx),val(_rows_filled), val(_res_nid), val(_res_factor)));
    } else {
      return(TreeShr->TreeMakeSegmentMinMax(val(_nid),descr(_start),descr(_end),xd(_dim),descr(data(_array)),val(_idx),val(_rows_filled), val(_res_nid), val(_res_factor)));
    }
  }
}
