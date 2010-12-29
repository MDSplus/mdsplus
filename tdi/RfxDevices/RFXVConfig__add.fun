public fun RFXVConfig__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXVConfig',61, _nidout);
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

    for (_c = 1; _c <=9; _c++)
    {
        _cn = _path // '.VALVE_0' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':POSITION', 'TEXT', *, *, _nid);
        DevAddNode(_cn // ':NAME', 'TEXT', *, *, _nid);
        DevAddNode(_cn // ':VD_MODULE', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':CTRL_STATE', 'NUMERIC', *, *, _nid);
    }

    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}



       
