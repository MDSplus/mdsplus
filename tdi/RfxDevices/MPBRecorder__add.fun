public fun MPBRecorder__add(in _path, out _nidout)
{
    DevAddStart(_path, 'MPBRecorder', 8, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':START_EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':REC_EVENTS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':REC_TIMES', 'NUMERIC', *, *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 20,'TIME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 55,'TIME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

       