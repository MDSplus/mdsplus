Public fun dtao32__add(in _path, out _nidout)
{
  DevAddPyStart(_path,'CP7452',80,['from MitDevices import DTAO32'],_nidout);
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
  DevAddNode(_path//':MAX_SAMPLES', 'NUMERIC', 16384., '/noshot_write', _nid);
  DevAddNode(_path//':CONTINUOUS', 'NUMERIC', 0, '/noshot_write', _nid);

  /* dim for the channels */
  DevAddNode(_path//':DIM', 'DIM', compile('Build_Dim(Build_Window(0, '//_path//':MAX_SAMPLES - 1, '//_path//':TRIGGER), BEGIN_OF('//_path//':CLOCK) : END_OF('//_path//':CLOCK) : SLOPE_OF('//_path//':CLOCK) * '//_path//':CLOCK_DIV)'), '/noshot_write', _nid);

  /* waveforms */
  for (_c=1;_c<=32;_c++)
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
