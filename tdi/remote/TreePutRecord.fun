public fun TreePutRecord(as_is _nodename, in _data)
{
  Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),xd(_data),val(0)));
}
