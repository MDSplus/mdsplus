public fun CHVPS_SET__add(in _path, out _nidout)
{
    DevAddStart(_path, 'CHVPS_SET', 14, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TARGET', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':SET', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':RESET_MODE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':GATE_MODE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OVERLOAD_DET', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIP_TIME', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VOLTAGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CURRENT', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VMAX', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':RAISE_RATE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FALL_RATE', 'NUMERIC', *, *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'PULSE_PREPARATION', 10, 'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
