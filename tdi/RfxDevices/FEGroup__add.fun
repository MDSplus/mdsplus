public fun FEGroup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'FEGroup', 5, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CHANS_PATH', 'ANY', *, *, _nid);
    DevAddNode(_path // ':GAIN', 'NUMERIC', *, *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 45,'ISIS_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
