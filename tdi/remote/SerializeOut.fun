public fun SerializeOut(in _in, optional _nid)
{
  if (present(_nid)) TreeShr->TreeGetRecord(val(_nid),xd(_in));
  _ans = 0;
  MdsShr->MdsSerializeDscOut(xd(_in),xd(_ans));
  return(byte_unsigned(_ans));
}
