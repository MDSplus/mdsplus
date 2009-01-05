Public fun dt196a__add(in _path, out _nidout)
{
/*  DevAddStart(_path,'DT200',660,_nidout); */
  DevAddStart(_path,'DT196A',660,_nidout, 'MitDevices');
  DevAddNode(_path//':NODE','TEXT',*,*,_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);

  /* trigger routing */
  for (_c=0;_c<=5;_c++)
  {
    _name=_path//':DI'//TEXT(_c, 1 );
    _wval='MI'//TEXT(_c,1);
    DevAddNode(_name, 'AXIS', *,'/noshot_write', _nid);
    DevAddNode(_name//':WIRE', 'TEXT', (_c==0) ? 'mezz' : ((_c==3) ? 'mezz' : 'fpga'),'/noshot_write', _nid);
    DevAddNode(_name//':BUS', 'TEXT', ((_c==0) || (_c==3)) ? 'fpga' : ' ','/noshot_write', _nid);
  }
  /* internal clock / clock divider (or int clock freq) */
  DevAddNode(_path//':CLOCK_SRC', 'TEXT', 'INT', '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK_DIV', 'NUMERIC', 200000., '/noshot_write', _nid);

  /* data acquisition */

  DevAddNode(_path//':DAQ_MEM', 'NUMERIC', 512, '/noshot_write', _nid);
  DevAddNode(_path//':ACTIVE_CHAN', 'NUMERIC', 96, '/noshot_write', _nid);
  DevAddNode(_path//':TRIG_SRC', 'TEXT', 'DI3', '/noshot_write', _nid);
  DevAddNode(_path//':POST_TRIG','NUMERIC',64,'/noshot_write',_nid);
  DevAddNode(_path//':PRE_TRIG','NUMERIC',0,'/noshot_write',_nid);

  /* channel calibration and firmware status */
  DevAddNode(_path//':RANGES', 'NUMERIC', *, '/nomodel_write', _nid);
  DevAddNode(_path//':STATUS_CMDS', 'TEXT', ['cat /proc/cmdline', 'get.d-tacq.release'], '/noshot_write', _nid);
  DevAddNode(_path//':BOARD_STATUS','SIGNAL', *, '/nomodel_write', _nid);
  for (_c=1;_c<=96;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':INC','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':FILTER_COEFS','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':RAW', 'NUMERIC', *, '/write_once/compress_on_put/nomodel_write', _nid);
  }

  /* and the default actions */
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return (1);
}
