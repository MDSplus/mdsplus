public fun ServerGetInfo()
{
  _ans=*;
  _status = MdsServerShr->ServerQAction(0,0,8,0,0,xd(_ans));
  if (_status & 1)
    return(_ans);
  else
    Abort();
}
