public fun GetSegmentInfo(as_is _node, in _idx, optional out _dtype, optional out _dimct, optional out _dims, optional out _next_row) {
  _nid=getnci(_node,"NID_NUMBER");
  _dtype=0b;
  _dimct=0b;
  _dims=zero(8,0);
  _next_row=0;
  _status=TreeShr->TreeGetSegmentInfo(val(_nid),val(_idx),ref(_dtype),ref(_dimct),ref(_dims),ref(_next_row));
  if (_status & 1) {
        _rowsize=1;
	for (_i=0;_i<(_dimct-1);_i++) _rowsize *= _dims[_i];
	_remainder=(_dims[_dimct-1]-_next_row)*_rowsize;
  } else _remainder=*;
  return(_remainder);
}

