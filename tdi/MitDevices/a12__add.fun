public fun submit__add(in _path, out _nidout)
{
  DevAddStart(_path,'A12',5,_nidout);
  DevAddNode(_path//':HOST','TEXT',*,'/noshot_write',_nid);
  DevAddNode(_path//':SCRIPT','TEXT',*,'/noshot_write',_nid);
  DevAddNode(_path//':SUBMIT_ACTION','ACTION',MAKE_ACTION(MAKE_DISPATCH(2,'SUBMIT_SERVER','ANALYSIS',50,*),
  COMPILE('SUBMIT('//_PATH//':SCRIPT,'//_PATH//':HOST)'),*,'/noshot_write',_nid);
  DevAddEnd();
}
