public fun SerializeIn(in _in)
{
  _ans = 0;
  MdsShr->MdsSerializeDscIn(ref(_in),xd(_ans));
  return(_ans);
}
