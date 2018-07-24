/* FOR INTERNAL USE ONLY */
public fun GetDefaultNid()
{
  _nid = 0L;
  _status = TreeShr->TreeGetDefaultNid(ref(_nid));
  if (_status & 1)
    return(_nid);
  else
    return("error");
}
