public fun RFXClock__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXClock', 8, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':DECODER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CHANNEL', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path // ':DUTY_CYCLE', 'NUMERIC', 50, *, _nid);
    DevAddNode(_path // ':CLOCK', 'AXIS', *, *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'TIME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

       

