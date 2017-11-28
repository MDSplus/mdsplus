public fun VMEWaves__add(in _path, out _nidout)
{
  DevAddStart(_path,'VMEWaves',165,_nidout);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':VME_IP','TEXT',*,*,_nid);
  DevAddNode(_path//':BASE_FREQ', 'NUMERIC', 5000., '/noshot_write', _nid);
  for (_c=1;_c<=32;_c++)
  {
    _cn = _path//'.CHANNEL_'//TEXT(_c, _c<10?1:2);
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    TreeTurnOff(_nid);
    DevAddNode(_cn//':TRIGGER_TIME', 'NUMERIC', 0, *,_nid);
    DevAddNode(_cn//':FREQUENCY', 'NUMERIC', 5000., *,_nid);
    DevAddNode(_cn//':WAVE_X', 'NUMERIC', *, *,_nid);
    DevAddNode(_cn//':WAVE_Y', 'NUMERIC', *, *,_nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'VME_SERVER',_path,_nid);
  DevAddEnd();
}