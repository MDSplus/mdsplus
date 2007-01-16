Public fun dt196ao__add(in _path, out _nidout)
{
  DevAddStart(_path,'DT196AO',49,_nidout, 'MitDevices');
  DevAddNode(_path//':NODE','TEXT',*,*,_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);

  for (_c=0;_c<=5;_c++)
  {
    _name = ':DI'//TEXT(_c,1);
    DevAddNode(_path//_name, 'AXIS', *, '/noshot_write', _nid);
  }

  /* internal clock / clock divider (or int clock freq) */
  DevAddNode(_path//':AO_CLK', 'TEXT', 'DI0', '/noshot_write', _nid);
  DevAddNode(_path//':AO_TRIG', 'TEXT', 'DI3', '/noshot_write', _nid);
  DevAddNode(_path//':FAWG_DIV', 'NUMERIC', 20, '/noshot_write', _nid);
  DevAddNode(_path//':CYCLE_TYPE', 'TEXT', 'ONCE', '/noshot_write', _nid);
  DevAddNode(_path//':TRIG_TYPE', 'TEXT', 'HARD_TRIG', '/noshot_write', _nid);
  DevAddNode(_path//':MAX_SAMPLES', 'NUMERIC', 16384., '/noshot_write', _nid);


  /* data acquisition */

  for (_c=1;_c<=16;_c++)
  {
    _cn = _path//':OUTPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/noshot_write',_nid);
    DevAddNode(_cn//':FIT','TEXT','LINEAR','/noshot_write',_nid);
  }

  /* and the default actions */
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return (1);
}
