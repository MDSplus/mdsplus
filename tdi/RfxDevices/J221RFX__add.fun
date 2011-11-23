public fun J221RFX__add(in _path, out _nidout)
{
    DevAddStart(_path, 'J221RFX', 66, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', 1E6, *, _nid);
    for (_c = 1; _c <=9; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':MODE', 'TEXT', 'PULSE', *, _nid);
        DevAddNode(_cn // ':REPETITION', 'NUMERIC', 1, *, _nid);
        DevAddNode(_cn // ':DELTA', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':TIME', 'NUMERIC', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=12; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 2);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':MODE', 'TEXT', 'TRIGGER', *, _nid);
        DevAddNode(_cn // ':REPETITION', 'NUMERIC', 1, *, _nid);
        DevAddNode(_cn // ':DELTA', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':TIME', 'NUMERIC', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
