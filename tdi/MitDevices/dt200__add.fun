Public fun dt200__add(in _path, out _nidout)
{
/*  DevAddStart(_path,'DT200',231,_nidout); */
  DevAddStart(_path,'DT200',191,_nidout, 'MitDevices');
  DevAddNode(_path//':NODE','TEXT',*,*,_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);

  /* trigger routing */
  for (_c=0;_c<=5;_c++)
  {
    _name=_path//':DI'//TEXT(_c, 1 );
    _wval='MI'//TEXT(_c,1);
    DevAddNode(_name, 'AXIS', *,'/noshot_write', _nid);
    DevAddNode(_name//':WIRE', 'TEXT', _wval,'/noshot_write', _nid);
    DevAddNode(_name//':BUS', 'TEXT', *,'/noshot_write', _nid);
  }
  /* internal clock / clock divider (or int clock freq) */
  DevAddNode(_path//':CLOCK_SRC', 'TEXT', 'INT', '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK_DIV', 'NUMERIC', 200000., '/noshot_write', _nid);

  /* data acquisition */

  DevAddNode(_path//':DAQ_MEM', 'NUMERIC', 64, '/noshot_write', _nid);
  DevAddNode(_path//':ACTIVE_CHAN', 'NUMERIC', 32, '/noshot_write', _nid);
  DevAddNode(_path//':TRIG_SRC', 'TEXT', 'DI2', '/noshot_write', _nid);
  DevAddNode(_path//':POST_TRIG','NUMERIC',64,'/noshot_write',_nid);
  DevAddNode(_path//':PRE_TRIG','NUMERIC',1,'/noshot_write',_nid);
   for (_c=1;_c<=32;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':INC','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':FILTER_COEFS','NUMERIC',*,'/noshot_write',_nid);
  }

  /* Digital Outputs

  _dpath = _path//'.DIG_OUT';
  DevAddNode(_dpath, 'STRUCTURE', *, *, _nid);
  DevAddNode(_dpath//':TRIG_SRC', 'TEXT', 'DI1', '/noshot_write', _nid);
  for (_c=1;_c<=8;_c++)
  {
    _cn = _dpath//':OUTPUT_'//TEXT(_c, 1);
    DevAddNode(_cn, 'SIGNAL', *,'/noshot_write', _nid);
    DevAddNode(_cn//':SET_POINTS', 'NUMERIC', *,'/noshot_write', _nid);
    DevAddNode(_cn//':EDGES_R', 'NUMERIC', *,'/noshot_write', _nid);
    DevAddNode(_cn//':EDGES_F', 'NUMERIC', *,'/noshot_write', _nid);
  }
  */
  /* Analog Outputs
  _apath = _path//'.ANALOG_OUT';
  DevAddNode(_apath, 'STRUCTURE', *, *, _nid);
  DevAddNode(_apath//':TRIG_SRC', 'TEXT', 'DI1', '/noshot_write', _nid);
  DevAddNode(_apath//':NSAMP', 'TEXT', 'DI1', '/noshot_write', _nid);
  for (_c=1;_c<=2;_c++)
  {
    _cn = _apath//':OUTPUT_'//TEXT(_c, 1);
    DevAddNode(_cn, 'SIGNAL', *,'/noshot_write', _nid);
    DevAddNode(_cn//':SET_POINTS', 'NUMERIC', *,'/noshot_write', _nid);
  }
  */

  /* and the default actions */
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return (1);
}
