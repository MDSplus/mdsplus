public fun DIO2Encoder__add(in _path, out _nidout)
{
    write(*, _path);
    DevAddStart(_path, 'DIO2Encoder', 91, _nidout);
    DevAddNode(_path // ':BOARD_ID', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'TEXT', 'INTERNAL', *, _nid);

    for (_c = 1; _c <=9; _c++)
    {
        _cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':EVENT_NAME', 'TEXT', *, *, _nid);
        DevAddNode(_cn // ':EVENT', 'NUMERIC', 1, *, _nid);
        DevAddNode(_cn // ':EVENT_TIME', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':TERMINATION', 'TEXT', 'OFF', *, _nid);
    }
    for (_c = 10; _c <=16; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 2);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':EVENT_NAME', 'TEXT', *, *, _nid);
        DevAddNode(_cn // ':EVENT', 'NUMERIC', 1., *, _nid);
        DevAddNode(_cn // ':EVENT_TIME', 'NUMERIC', 0, *, _nid);
        DevAddNode(_cn // ':TERMINATION', 'TEXT', 'OFF', *, _nid);

	}
    _cn = _path // '.CHANNEL_SW';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':EVENT_NAME', 'TEXT', *, *, _nid);
    DevAddNode(_cn // ':EVENT', 'NUMERIC', 1., *, _nid);
    DevAddNode(_cn // ':EVENT_TIME', 'NUMERIC', 0, *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}


