fun public MdsValue(in _expr)
{
  if (MdsIsRemote())
    _ans = mdsremote->RemoteMdsValue:dsc( REF(_expr//char(0b)));
  else
    _ans = Execute(_expr);
  return(_ans);
}
