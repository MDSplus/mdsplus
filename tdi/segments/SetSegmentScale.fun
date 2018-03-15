public fun SetSegmentScale(as_is _nodename, optional _scale) {
  if (present(_scale))
    Return(TreeShr->TreeSetSegmentScale(val(getnci(_nodename,'nid_number')),xd(_scale)));
  else
    Return(TreeShr->TreeSetSegmentScale(val(getnci(_nodename,'nid_number')),val(0)));
}

    
