public fun TR10__add(in _path, out _nidout)
{
    DevAddStart(_path, 'TR10', 111, _nidout);
    DevAddNode(_path // ':BOARD_ID', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', 1E4, *, _nid);
    DevAddNode(_path // ':USE_TIME', 'TEXT', 'TRUE', *, _nid);
    DevAddNode(_path // ':PTS', 'NUMERIC', *, *, _nid);
    for (_c = 1; _c <=9; _c++)
    {
        _cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':START_TIME', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn // ':END_TIME', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':START_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':END_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=16; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 2);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':START_TIME', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn // ':END_TIME', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':START_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':END_IDX', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
	DevAddNode(_path // ':TRIG_EDGE', 'TEXT', 'RISING', *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
