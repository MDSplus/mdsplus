public fun WE7000__add(in _path, out _nidout)
{
    DevAddStart(_path, 'WE7000', 38, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CNTRL_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':STATION_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':BUSTRIG_1', 'TEXT', 'EXTIO', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BUSTRIG_2', 'TEXT', 'TRGIN', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG_SLOPE', 'TEXT', 'POSITIVE', *, _nid);
    DevAddNode(_path // ':CMN_CLOCK', 'TEXT', 'EXTIO', *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':ARM_SOURCE', 'TEXT', 'SOFTWARE', *, _nid);

    for (_s = 1; _s <=8; _s++)
    {
        _cs = _path // '.SLOT_0' // TEXT(_s, 1);
		DevAddNode(_cs, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cs // ':TYPE_MODULE', 'TEXT', *, *, _nid);
		DevAddNode(_cs // ':LINK_MODULE', 'NUMERIC', 1, *, _nid);
	}

    DevAddAction(_path//':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 50,'ISIS_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION',  'PULSE_PREPARATION', 'ARM', 70,'ISIS_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
