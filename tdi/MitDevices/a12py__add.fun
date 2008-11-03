public fun a12py__add(in _path, out _nidout)
{
  DevAddPyStart(_path,'A12',2,['from MitDevices import A12'],_nidout);
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
}
