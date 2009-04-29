Public fun dtdo32__add(in _path, out _nidout)
{
  DevAddPyStart(_path,'DTDO32',77,['from MitDevices import DTDO32'],_nidout);
  DevAddNode(_path//':HOSTBOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':TRIG_SRC', 'TEXT', 'S_LEMO_TRG_OPTO', '/noshot_write', _nid);
  DevAddNode(_path//':TRIG_EDGE', 'TEXT', 'FALLING', '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK_SRC', 'TEXT', 'S_LEMO_CLK_OPTO', '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK_EDGE', 'TEXT', 'FALLING', '/noshot_write', _nid);
  DevAddNode(_path//':TRIGGER', 'NUMERIC', 0.0, '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK', 'AXIS', *, '/noshot_write', _nid);
  DevAddNode(_path//':CLOCK_DIV', 'NUMERIC', 1, '/noshot_write', _nid);
  DevAddNode(_path//':MODE', 'TEXT', 'M_AWGT', '/noshot_write', _nid);

  /* waveforms */
  for (_c=1;_c<=64;_c++)
  {
    _cn = _path//':OUTPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'NUMERIC',*,'/noshot_write',_nid);
  }

  /* and the default actions */
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return (1);
}
