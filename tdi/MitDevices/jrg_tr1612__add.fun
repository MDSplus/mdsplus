public fun jrg_tr1612__add(in _path, out _nidout)
{
  DevAddStart(_path,'JRG_TR1612',150,_nidout);
  DevAddNode(_path//':NAME','TEXT',*,*,_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':MODULE_ID','NUMERIC',*,'/nomodel_write',_nid);
  DevAddNode(_path//':SAMP_INTRVAL','NUMERIC',1.0E-7,'/noshot_write',_nid);
  DevAddNode(_path//':EXT_CLOCK','AXIS',*,'/noshot_write',_nid);
  DevAddNode(_path//':TRIGGER','NUMERIC',0.,'/noshot_write',_nid);
  DevAddNode(_path//':SAMP_2_STORE','NUMERIC',65536,'/noshot_write',_nid);
  DevAddNode(_path//':PRETRIG_SAMP','NUMERIC',1024,'/noshot_write',_nid);
  DevAddNode(_path//':ACTIVE_CHANS','NUMERIC',16,'/noshot_write',_nid);
  DevAddNode(_path//':SAMP_AVERAGE','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':TRIG_BY_CHAN','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':TRIG_DELAY','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':VALID_DATA','NUMERIC',*,'/nomodel_write',_nid);
  DevAddNode(_path//':VALID_TRIG','NUMERIC',*,'/nomodel_write',_nid);
  DevAddNode(_path//':LAM_SETUP','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':MUL_PRE_POST','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':RTC_ENABLE','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':RTC_TRIGS','NUMERIC',*,'/nomodel_write/write_once/compress_on_put',_nid);
  DevAddNode(_path//':MEMORY_SHARE','NUMERIC',0,'/noshot_write',_nid);
  for (_c=1;_c<17;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c/10,1)//TEXT(_c mod 10,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':COMMENT','TEXT',*,*,_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':RANGE','NUMERIC',4.096,'/noshot_write',_nid);
    DevAddNode(_cn//':OFFSET','NUMERIC',2048,'/noshot_write',_nid);
    DevAddNode(_cn//':TRIG_THRESH','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':VALID_THRESH','NUMERIC',*,'/noshot_write',_nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
