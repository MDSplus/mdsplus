fun public MdsValue(in _expr)
{
  if (MdsIsRemote())
    _ans = MdsRemote->RemoteMdsValue:dsc( REF(_expr//char(0b)));
  else
    _ans = Execute(_expr);
  return(_ans);
}
