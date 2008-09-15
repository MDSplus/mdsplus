public fun DPO7054__add(in _path, out _nidout)
{
	/******** START ********/

		DevAddStart(_path, 'DPO7054', 76, _nidout);




	/******** GENERAL ********/

 /* nid 1 */   	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);



	/******** COMMON ********/

		_cn = _path // '.COMMON';
/* nid 2 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

	/******** COMMON HORIZONTAL - ACQ ********/

		_cn = _path // '.COMMON' // '.HORIZ_ACQ';
/* nid 3 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SAMPLE_RATE', 'NUMERIC', 100E6, *, _nid);

	/******** COMMON TRIGGER ********/

		_cn = _path // '.COMMON' // '.TRIGGER';
/* nid 5 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SOURCE', 'TEXT', 'CH1', *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
		DevAddNode(_cn // ':SLOPE', 'TEXT', 'RISE', *, _nid);
		DevAddNode(_cn // ':LEVEL', 'NUMERIC', 1, *, _nid);
		DevAddNode(_cn // ':HOLDOFF', 'NUMERIC', 250E-9, *, _nid);
		DevAddNode(_cn // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid); /* nid 11 */

	/******** COMMON VERTICAL ********/

		_cn = _path // '.COMMON' // '.VERTICAL';
/* nid 12 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid); /* nid 12 */

	/******** COMMON VERTICAL CH01 ********/

		_cn = _path // '.COMMON' // '.VERTICAL' // '.CH01';
/* nid 13 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
		DevAddNode(_cn // ':TERMINATION', 'NUMERIC', 1E6, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':BW_LIMIT', 'TEXT', 'FULL', *, _nid);

	/******** COMMON VERTICAL CH02 ********/

		_cn = _path // '.COMMON' // '.VERTICAL' // '.CH02';
/* nid 19 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
		DevAddNode(_cn // ':TERMINATION', 'NUMERIC', 1E6, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':BW_LIMIT', 'TEXT', 'FULL', *, _nid);

	/******** COMMON VERTICAL CH03 ********/

		_cn = _path // '.COMMON' // '.VERTICAL' // '.CH03';
/* nid 25 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
		DevAddNode(_cn // ':TERMINATION', 'NUMERIC', 1E6, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':BW_LIMIT', 'TEXT', 'FULL', *, _nid);

	/******** COMMON VERTICAL CH04 ********/

		_cn = _path // '.COMMON' // '.VERTICAL' // '.CH04';
/* nid 31 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':COUPLING', 'TEXT', 'DC', *, _nid);
		DevAddNode(_cn // ':TERMINATION', 'NUMERIC', 1E6, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':BW_LIMIT', 'TEXT', 'FULL', *, _nid);



	/******** WINDOW01 ********/

		_cn = _path // '.WINDOW01';
/* nid 37 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

	/******** WINDOW01 HORIZONTAL - ACQ ********/

		_cn = _path // '.WINDOW01' // '.HORIZ_ACQ';
/* nid 38 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 10E-3, *, _nid);
		DevAddNode(_cn // ':DELAY', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);

	/******** WINDOW01 VERTICAL ********/

		_cn = _path // '.WINDOW01' // '.VERTICAL';
/* nid 42 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

	/******** WINDOW01 VERTICAL CH01 ********/

		_cn = _path // '.WINDOW01' // '.VERTICAL' // '.CH01';
/* nid 43 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW01 VERTICAL CH02 ********/

		_cn = _path // '.WINDOW01' // '.VERTICAL' // '.CH02';
/* nid 46 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW01 VERTICAL CH03 ********/

		_cn = _path // '.WINDOW01' // '.VERTICAL' // '.CH03';
/* nid 49 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW01 VERTICAL CH04 ********/

		_cn = _path // '.WINDOW01' // '.VERTICAL' // '.CH04';
/* nid 52 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);



	/******** WINDOW02 ********/

		_cn = _path // '.WINDOW02';
/* nid 55 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

	/******** WINDOW02 HORIZONTAL - ACQ ********/

		_cn = _path // '.WINDOW02' // '.HORIZ_ACQ';
/* nid 56 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 10E-3, *, _nid);
		DevAddNode(_cn // ':DELAY', 'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':POSITION', 'NUMERIC', 0, *, _nid);

	/******** WINDOW02 VERTICAL ********/

		_cn = _path // '.WINDOW02' // '.VERTICAL';
/* nid 60 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

	/******** WINDOW02 VERTICAL CH01 ********/

		_cn = _path // '.WINDOW02' // '.VERTICAL' // '.CH01';
/* nid 61 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW02 VERTICAL CH02 ********/

		_cn = _path // '.WINDOW02' // '.VERTICAL' // '.CH02';
/* nid 64 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW02 VERTICAL CH03 ********/

		_cn = _path // '.WINDOW02' // '.VERTICAL' // '.CH03';
/* nid 67 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

	/******** WINDOW02 VERTICAL CH04 ********/

		_cn = _path // '.WINDOW02' // '.VERTICAL' // '.CH04';
/* nid 70 */	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':SCALE', 'NUMERIC', 5, *, _nid);
		DevAddNode(_cn // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);



	/******** ACTIONS ********/

/* nid 73 */	DevAddAction(_path //':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
		DevAddAction(_path //':ARM_ACTION', 'INIT', 'ARM', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
		DevAddAction(_path //':STORE_ACTION', 'STORE', 'STORE', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);



	/******** END ********/

    		DevAddEnd();
}