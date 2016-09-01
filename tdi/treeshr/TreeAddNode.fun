public fun TreeAddNode(in _nodename, out _nid, in _usage)
{
  _nid = 0;
  _status = TreeShr->TreeAddNode(ref(_nodename//"\0"),ref(_nid),val(_usage));
  return(_status);
}
