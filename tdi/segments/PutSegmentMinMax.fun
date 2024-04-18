public fun PutSegmentMinMax(as_is _node, optional in _idx, in _data, as_is _res_node, in _res_factor) {
  _nid=getnci(_node,"NID_NUMBER");
  _res_nid=getnci(_res_node,"NID_NUMBER");
  if (!present(_idx)) _idx=-1;
  return(TreeShr->TreePutSegmentMinMax(val(_nid),val(_idx),descr(data(_data)), val(_res_nid), val(_res_factor)));
}
