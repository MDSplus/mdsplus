public fun TSEdgeCCD__add(in _path, out _nidout)
{
    DevAddStart(_path, 'TSEdgeCCD', 12, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':INTERFACE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_ARM', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIG_LASER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DATA', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':BACKGROUND', 'SIGNAL', *, *, _nid);
    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION', 'INIT',  50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION',   'PULSE_PREPARATION', 'ARM',   50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE',             'STORE', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
