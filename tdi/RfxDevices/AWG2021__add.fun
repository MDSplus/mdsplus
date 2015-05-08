public fun AWG2021__add(in _path, out _nidout)
{
	DevAddStart(_path, 'AWG2021', 18, _nidout);

	/******** GENERAL ********/

	DevAddNode(_path // ':GPIB_ADDR', 'NUMERIC', 3, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

	/******** TRIGGER ********/

	_cn = _path // '.TRIGGER';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':LEVEL', 'NUMERIC', 1, *, _nid);
	DevAddNode(_cn // ':SLOPE', 'TEXT', 'POSITIVE', *, _nid);
	DevAddNode(_cn // ':IMPEDANCE', 'TEXT', 'HIGH', *, _nid);
	DevAddNode(_cn // ':SOURCE', 'NUMERIC', 0, *, _nid);

	/******** WFM ********/

	_cn = _path // '.WFM';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':AMPLITUDE', 'NUMERIC', 1, *, _nid);
	DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':FILTER', 'TEXT', 'OFF', *, _nid);
	DevAddNode(_cn // ':DURATION', 'NUMERIC', 1, *, _nid);
	DevAddNode(_cn // ':FREQUENCY', 'NUMERIC', 1000, *, _nid);
	DevAddNode(_cn // ':POINTS', 'NUMERIC', 1000, *, _nid);




	/******** ACTIONS ********/

	DevAddAction(_path //':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path //':ARM_ACTION', 'INIT', 'ARM', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path //':RESET_ACTION', 'STORE', 'RESET', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);

    DevAddEnd();
}
