public fun dt100__add(in _path, out _nidout)
{
  DevAddStart(_path,'DT100',108,_nidout);
  DevAddNode(_path//':NODE','TEXT',*,*,_nid);
  DevAddNode(_path//':BOARD','NUMERIC',*,*,_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':EXT_CLOCK_IN','AXIS',*,'/noshot_write',_nid);
  DevAddNode(_path//':STOP_TRIG','AXIS',0.,'/noshot_write',_nid);
  DevAddNode(_path//':MEM_SIZE','NUMERIC',64,'/noshot_write',_nid);
  DevAddNode(_path//':ACTIVE_CHANS','NUMERIC',32,'/noshot_write',_nid);
  DevAddNode(_path//':MAX_SAMPLES','NUMERIC',32,'/noshot_write',_nid);
  DevAddNode(_path//':MODE','NUMERIC',0,'/noshot_write',_nid);
  

  for (_c=1;_c<=32;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
