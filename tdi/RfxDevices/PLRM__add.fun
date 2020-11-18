public fun PLRM__add(in _path, out _nidout)
{
	DevAddStart(_path, 'PLRM', 23, _nidout);

	/******** GENERAL ********/

    	DevAddNode(_path // ':IP_ADDR', 'TEXT', '192.168.34.26:6660', *, _nid);
    	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);


	/******** ANGLES ********/

	_cn = _path // '.ANGLE_01';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);

	_cn = _path // '.ANGLE_02';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);

	_cn = _path // '.ANGLE_03';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);

	_cn = _path // '.ANGLE_04';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);

	_cn = _path // '.ANGLE_05';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);

	_cn = _path // '.ANGLE_06';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':SA', 'NUMERIC', 0., *, _nid);
	DevAddNode(_cn // ':GA', 'NUMERIC', 0., *, _nid);


	/******** ACTIONS ********/
		
	DevAddAction(_path //':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path //':STORE_ACTION', 'STORE', 'STORE', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);

    	DevAddEnd();
}