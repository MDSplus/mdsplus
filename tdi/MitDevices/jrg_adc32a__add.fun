public fun JRG_ADC32A__add(in _path, out _nidout)
{

  private _N_NAME = 1;
  private _N_COMMENT = 2;
  private _N_ACTIVE_CHAN = 3;
  private _N_ACTIVE_MEM = 4;
  private _N_EXT_CLOCK = 5;
  private _N_TRIGGER = 6;
  private _N_CLOCK_OUT = 7;
  private _N_CHANNELS = 8;
  private _K_NODES_PER_CHANNEL = 4;
  private _K_CHAN_START_OFFSET = 1;
  private _K_CHAN_END_OFFSET = 2;
  private _K_CHAN_GAIN_OFFSET = 3;

  DevAddStart(_path,'JRG_ADC32A',138,_nidout);
  DevAddNode(_path//':NAME','TEXT',*,*,_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':ACTIVE_CHANS', 'NUMERIC', 32,*, _nid);
  DevAddNode(_path//':ACTIVE_MEM', 'NUMERIC', 128,*, _nid);
  DevAddNode(_path//':EXT_CLOCK','AXIS',*,*,_nid);
  DevAddNode(_path//':TRIGGER','NUMERIC',*,*,_nid);
  DevAddNode(_path//':CLOCK_OUT','AXIS',*,*,_nid);

  for (_c=1;_c<=32;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,*,_nid);
    DevAddNode(_cn//':GAIN', 'NUMERIC', 10.24, *, _nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
