public fun RFXDClock__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXDClock', 15, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':DECODER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':GATE_CHAN', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CLOCK_CHAN', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIGGER_MODE', 'TEXT', 'EVENT', *, _nid);
    DevAddNode(_path // ':EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':DELAY', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':FREQUENCY_1', 'NUMERIC', 100., *, _nid);
    DevAddNode(_path // ':FREQUENCY_2', 'NUMERIC', 1000., *, _nid);
    DevAddNode(_path // ':DURATION', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':OUTPUT_MODE', 'TEXT', 'DOUBLE SWITCH: TOGGLE', *, _nid);
    DevAddNode(_path // ':CLOCK', 'AXIS', *, *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'TIME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

       