public fun chs_a14__add(in _path, out _nidout)
{
  /********************************************************************
     The CHS_A14 device was developed for the CHS experiment at NIFS in
     Japan. This system uses VME shared memory to communicate with a VME
     based CAMAC controller.
  ********************************************************************/
  DevAddStart(_path,'CHS_A14',33,_nidout);
  DevAddNode(_path//':DIGNUM','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':CLOCK_DIVIDE','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':EXT_CLOCK_IN','AXIS',*,'/noshot_write',_nid);
  DevAddNode(_path//':STOP_TRIG','NUMERIC',0.,'/noshot_write',_nid);
  DevAddNode(_path//':PTS','NUMERIC',0,'/write_once',_nid);
  DevAddNode(_path//':MODE','NUMERIC',0,'/write_once',_nid);
  DevAddNode(_path//':DIMENSION','AXIS',*,'/noshot_write',_nid);
  DevAddNode(_path//':CLK_POLARITY','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':STR_POLARITY','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':STP_POLARITY','NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//':GATED','NUMERIC',0,'/noshot_write',_nid);
  for (_c=1;_c<7;_c++)
  {
    _cn = _path//':INPUT_'//TEXT(_c,1);
    DevAddNode(_cn,'SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
    DevAddNode(_cn//':STARTIDX','NUMERIC',*,'/noshot_write',_nid);
    DevAddNode(_cn//':ENDIDX','NUMERIC',*,'/noshot_write',_nid);
  }
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'VME_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'VME_SERVER',_path,_nid);
  DevAddEnd();
}
