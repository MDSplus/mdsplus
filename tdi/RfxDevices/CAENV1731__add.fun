public fun CAENV1731__add(in _path, out _nidout)
{
    write(*, _path);
    DevAddStart(_path, 'CAENV1731', 78, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VME_ADDRESS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE', 'TEXT', 'OVER', *, _nid);
    DevAddNode(_path // ':TRIG_SOFT', 'TEXT', 'ENABLED', *, _nid);
    DevAddNode(_path // ':TRIG_EXT', 'TEXT', 'ENABLED', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':INPUT_OFS', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':CLOCK_MODE', 'TEXT', '500 MHz', *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MONITOR_MODE', 'TEXT', 'BUFFER OCCUPANCY', *, _nid);
    DevAddNode(_path // ':MONITOR_LEV', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':NUM_SEGMENTS', 'NUMERIC', 1024, *, _nid);
    DevAddNode(_path // ':USE_TIME', 'TEXT', 'TRUE', *, _nid);
    DevAddNode(_path // ':PTS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':START_IDX', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':END_IDX', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':START_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':END_TIME', 'NUMERIC', 1E-6, *, _nid);
    DevAddNode(_path // ':BOARD_ID', 'NUMERIC', 0, *, _nid);
	for (_c = 1; _c <=8; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn // ':STATE', 'TEXT', 'ENABLED', *, _nid);
        DevAddNode(_cn // ':TRIG_STATE', 'TEXT', 'DISABLED', *, _nid);
        DevAddNode(_cn // ':TR_TRESH_LEV', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':TRIG_TRESH', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}






