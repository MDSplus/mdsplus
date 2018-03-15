public fun TreePutRecord(as_is _nodename, optional as_is _data, optional _utility_update)
{
  if (!present(_utility_update)) _utility_update = 0;
  if (present(_data))
    Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),xd(_data),val(_utility_update)));
  else
    Return(TreeShr->TreePutRecord(val(getnci(_nodename,'nid_number')),*,val(_utility_update)));
}
