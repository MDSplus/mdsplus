public fun WEGroup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'WEGroup', 18, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CHANS_PATH', 'ANY', *, *, _nid);

	_cn = _path // '.WE7116_PARAM';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':START_TIME', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':END_TIME', 'NUMERIC', 1., *, _nid);
	DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
	DevAddNode(_cn // ':RANGE', 'NUMERIC', 1.0, *, _nid);
	DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0.0, *, _nid);
	DevAddNode(_cn // ':FILTER', 'NUMERIC', 0, *, _nid);

	_cn = _path // '.WE7275_PARAM';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':START_TIME', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':END_TIME', 'NUMERIC', 1., *, _nid);
	DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
	DevAddNode(_cn // ':RANGE', 'NUMERIC', 1.0, *, _nid);
	DevAddNode(_cn // ':AAF', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':FILTER', 'NUMERIC', 0, *, _nid);
	
    DevAddAction(_path//':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 45,'ISIS_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
