public fun TreePutRecord(as_is _nodename, in _data, optional _utility_update)
{
  if (!present(_utility_update)) _utility_update = 0;
  Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),xd(_data),val(_utility_update)));
}
