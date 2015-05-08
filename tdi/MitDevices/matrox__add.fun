public fun matrox__add(in _path, out _nidout)
{
  DevAddStart(_path,'matrox',19,_nidout);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':MAX_FRAMES', 'NUMERIC', 60, '/noshot_write', _nid);
  DevAddNode(_path//':DCF_NAME','TEXT','C:\\Camera\\Josh.DCF','/noshot_write',_nid);
  for (_c=1;_c<4;_c++)
  {
    _cn = _path//':CAMERA_'//TEXT(_c,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':REQUESTED', 'NUMERIC', 0 : 59 : 1, '/noshot_write', _nid);
    DevAddNode(_cn//':MPG_FILE', 'TEXT', *, '/noshot_write', _nid);
    DevAddNode(_cn//':CAMERA', 'TEXT', *, '/noshot_write', _nid);
  }
  DevAddNode(_path//':TRIGGER', 'NUMERIC', 0.0, *, _nid); 
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return(1);
}
