public fun SerializeOut(as_is _in)
{
  _ans = 0;
  MdsShr->MdsSerializeDscOut(xd(_in),xd(_ans));
  return(byte_unsigned(_ans));
}
