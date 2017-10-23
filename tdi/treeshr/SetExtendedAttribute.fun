public fun SetExtendedAttribute(as_is _node, in _name, in _value) {
/* Created or set extended node attribute */
  _nid=getnci(_node,"NID_NUMBER");
  return(TreeShr->TreeSetXNci(val(_nid),_name,xd(_value)));
}
