public fun GetExtendedAttribute(as_is _node, in _name) {
/* Get extended node attribute value */
  _nid=getnci(_node,"NID_NUMBER");
  _value=*;
  _status = TreeShr->TreeGetXNci(val(_nid),_name,xd(_value));
  return(_status & 1 ? _value : *);
}
