public fun TR32__add(in _path, out _nidout)
{
    DevAddStart(_path, 'TR32', 45, _nidout);
    DevAddNode(_path // ':BOARD_ID', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', 1E4, *, _nid);
    DevAddNode(_path // ':CK_RESAMP', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':CK_TERM', 'TEXT', "NO", *, _nid);
    DevAddNode(_path // ':USE_TIME', 'TEXT', 'TRUE', *, _nid);
    DevAddNode(_path // ':PTS', 'NUMERIC', *, *, _nid);
 	DevAddNode(_path // ':TRIG_EDGE', 'TEXT', 'RISING', *, _nid);
    for (_c = 1; _c <=4; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':RANGE', 'NUMERIC', 0., *, _nid);
    	DevAddNode(_cn // ':START_TIME', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn // ':END_TIME', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':START_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':END_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'PCI_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
