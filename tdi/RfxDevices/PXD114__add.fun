public fun PXD114__add(in _path, out _nidout)
{
    DevAddStart(_path, 'PXD114', 41, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'TEXT', 'EXTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_LEVEL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG_EDGE', 'TEXT', 'POSITIVE', *, _nid);
    DevAddNode(_path // ':TRIG_COUPL', 'TEXT', 'DC', *, _nid);
    DevAddNode(_path // ':TRIG_IMP', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':NUM_SEGMENTS', 'NUMERIC', 10, *, _nid);
    DevAddNode(_path // ':SEGMENT_TIME', 'NUMERIC', 500E-9, *, _nid);
    DevAddNode(_path // ':SEG_TIME_OFS', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':ACT_SEG_LEN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':ACT_SAMP_FRQ', 'NUMERIC', 0, *, _nid);
    for (_c = 1; _c <=4; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':INPUT_IMP', 'NUMERIC', 1000000, *, _nid);
		DevAddNode(_cn // ':RANGE', 'NUMERIC', 1, *, _nid);
		DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddNode(_path // ':TRIGGERS', 'NUMERIC', *, *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'PCI_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}


