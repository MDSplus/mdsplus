public fun GetSegmentInfo(as_is _node, optional _dtype, optional _dimct, optional _dims, optional _idx, optional _next_row) {
  _nid=getnci(_node,"NID_NUMBER");
  _dtype=0b;
  _dimct=0b;
  _dims=zero(8,0);
  _idx=0;
  _next_row=0;
  _status=TreeShr->TreeGetSegmentInfo(val(_nid),ref(_dtype),ref(_dimct),ref(_dims),ref(_idx),ref(_next_row));
  if (_status & 1) {
        _rowsize=1;
	for (_i=0;_i<(_dimct-1);_i++) _rowsize *= _dims[_i];
	_remainder=(_dims[_dimct-1]-_next_row)*_rowsize;
  } else _remainder=*;
  return(_remainder);
}

