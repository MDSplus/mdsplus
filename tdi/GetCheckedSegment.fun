/* A customized version of GetSegment for use in EFDA ITM */

public fun GetCheckedSegment(as_is _node, in _idx) {
  _nid=getnci(_node,"NID_NUMBER");
  _data=0;
  _dim=0;
  
  _caching = 1;
  if_error(_is_cached, _caching = 0);
  if(_caching)
  {
      _caching = _is_cached;
  }
  if(_caching)
  	_status=CacheShr->RTreeGetSegment(val(_nid),val(_idx),xd(_data),xd(_dim));
  else
      _status=TreeShr->TreeGetSegment(val(_nid),val(_idx),xd(_data),xd(_dim));
  
  if (_status & 1) {
    return(make_signal(_data,*,_dim));
  } else {
    return(*);
  }
}
