public fun DEMOADC__add(in _path, out _nidout)
{
    DevAddPyStart(_path,'DEMOADC',24,['from RfxDevices import DEMOADC'],_nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_FREQ', 'NUMERIC', 10000, *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PTS', 'NUMERIC', 1000, *, _nid);
    for (_c = 1; _c <=4; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn // ':START_IDX', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':END_IDX', 'NUMERIC', 1000, *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
