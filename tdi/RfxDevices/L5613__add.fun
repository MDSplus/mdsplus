public fun L5613__add(in _path, out _nidout)
{
    DevAddStart(_path, 'L5613', 13, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':MODEL', 'TEXT', *, *, _nid);
    _cn = _path // '.CHANNEL_A';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':OUTPUT', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    _cn = _path // '.CHANNEL_B';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':OUTPUT', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
