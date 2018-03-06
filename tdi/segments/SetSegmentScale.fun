public fun SetSegmentScale(as_is _node, as_is _scale) {
  _nid=getnci(_node,"nid_number");
  return(TreeShr->TreeSetSegmentScale(val(_nid),xd(_scale)));
}

    
