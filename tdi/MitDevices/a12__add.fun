public fun a12__add(in _path, out _nidout)
{
  DevAddStart(_path,'A12',25,_nidout);
  DevAddNode(_path//':NAME','TEXT',*,*,_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':EXT_CLOCK_IN','AXIS',*,'/noshot_write',_nid);
  DevAddNode(_path//':STOP_TRIG','NUMERIC',0.,'/noshot_write',_nid);
  for (_c=1;_c<7;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
