public fun L6810__add(in _path, out _nidout)
{
  DevAddStart(_path,'L6810',36,_nidout);
  DevAddNode(_path//':NAME','TEXT',*,*,_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':FREQ', 'NUMERIC', 1000000., *, _nid);
  DevAddNode(_path//':EXT_CLOCK_IN','AXIS',*,*,_nid);
  DevAddNode(_path//':STOP_TRIG','AXIS',0.,*,_nid);
  DevAddNode(_path//':MEMORIES', 'NUMERIC', 1,*, _nid);
  DevAddNode(_path//':SEGMENTS', 'NUMERIC', 1,*, _nid);
  DevAddNode(_path//':ACTIVE_MEM', 'NUMERIC', 128,*, _nid);
  DevAddNode(_path//':ACTIVE_CHANS', 'NUMERIC', 4,*, _nid);
  for (_c=1;_c<=4;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c,1);
    DevAddNode(_cn,'SIGNAL',*,'/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':FULL_SCALE', 'NUMERIC', 10.24, *, _nid);
    DevAddNode(_cn//':SRC_CPLING', 'NUMERIC', 0, *, _nid);
    DevAddNode(_cn//':OFFSET', 'NUMERIC', 128BU, *, _nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
