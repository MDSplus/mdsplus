public fun cp7452__add(in _path, out _nidout)
{
  DevAddPyStart(_path,'CP7452',16,['from MitDevices import CP7452'],_nidout);
  DevAddNode(_path//':NODE','TEXT',*,'/noshot_write',_nid);
  DevAddNode(_path//':BOARD','NUMERIC',1,'/noshot_write',_nid);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//'.DIGITAL_OUTS','STRUCTURE',*,*,_nid);
  for (_i=0;_i<4;_i++) DevAddNode(_path//'.DIGITAL_OUTS:DO'//char(48+_i),'NUMERIC',0,'/noshot_write',_nid);
  DevAddNode(_path//'.DIGITAL_INS','STRUCTURE',*,*,_nid);
  for (_i=0;_i<4;_i++) DevAddNode(_path//'.DIGITAL_INS:DI'//char(48+_i),'NUMERIC',0,'/nomodel_write',_nid);
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
