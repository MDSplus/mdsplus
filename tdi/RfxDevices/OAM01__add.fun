public fun OAM01__add(in _path, out _nidout) {     DevAddStart(_path, 'OAM01', 55, _nidout);

    DevAddNode(_path // ':GPIB_CTRL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':GPIB_ADDR', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MODULE_TYPE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_1A';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_1B';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_2A';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_2B';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_3A';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_3B';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_4A';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);

    _cn = _path // '.CHANNEL_4B';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':RANGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':COUPLING', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SOURCE', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':INPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_cn // ':OUTPUT', 'TEXT', *, *, _nid);  



    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'GPIB_SERVER', getnci(_path, 'fullpath'), _nid);     
    DevAddAction(_path//':OFF_ACTION', 'STORE','OFF', 50,'GPIB_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
} 