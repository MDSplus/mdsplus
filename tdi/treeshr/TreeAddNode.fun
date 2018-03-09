public fun TreeAddNode(in _nodename, out _nid, in _usage)
{
  _usage = DATA(_usage);
  if (KIND(_usage)==14BU) {
   _usage = DICT(*,
    'ANY',0,
    'STRUCTURE',1,
    'NONE',1,
    'ACTION',2,
    'DEVICE',3,
    'DISPATCH',4,
    'NUMERIC',5,
    'SIGNAL',6,
    'TASK',7,
    'TEXT',8,
    'WINDOW',9,
    'AXIS',10,
    'SUBTREE',11,
    'COMPOUND_DATA',12)[UPCASE(_usage)];
  }
  _nid = 0;
  _status = TreeShr->TreeAddNode(ref(_nodename//"\0"),ref(_nid),val(_usage));
  return(_status);
}
