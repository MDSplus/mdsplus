public fun MCU__add(in _path, out _nidout)
{
    DevAddStart(_path, 'MCU', 6, _nidout);
    DevAddNode(_path // ':COMMENT',		'TEXT',	   *,	*, _nid);
    DevAddNode(_path // ':IP_ADDR',		'TEXT',	   *,	*, _nid);
    DevAddNode(_path // ':FILTER_POS',   'NUMERIC', [0,0,0,0,0,0,0],	*, _nid);
    DevAddNode(_path // ':MAN_POS',      'NUMERIC', [0,0,0,0,0,0,0],	*, _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'TOMO_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
