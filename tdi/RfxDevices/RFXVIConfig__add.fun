public fun RFXVIConfig__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXVIConfig',16, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':NCRIO_ON', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':NTURBO_ON', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':N_FILL_VALV', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':N_PUFF_VALV', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VACUUM_LEV', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VIK1_GAS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VIK1_PRESS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VIK2_GAS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VIK2_PRESS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VIK3_GAS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VIK3_PRESS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VVMC01_INI', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VVMC01_PPC', 'NUMERIC', *, *, _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}



       
