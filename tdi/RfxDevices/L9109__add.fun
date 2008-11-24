public fun L9109__add(in _path, out _nidout)
{
	DevAddStart(_path, 'L9109', 37, _nidout);

	/******** GENERAL ********/

    	DevAddNode(_path // ':GPIB_ADDR', 'NUMERIC', 9, *, _nid);
    	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

	/******** CLOCK ********/

	_cn = _path // '.CLOCK';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':SOURCE', 'TEXT', 'INTERNAL', *, _nid);
	DevAddNode(_cn // ':FREQUENCY', 'NUMERIC', 100E6, *, _nid);
	DevAddNode(_cn // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);

	/******** TRIGGER ********/

	_cn = _path // '.TRIGGER';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':SOURCE', 'TEXT', 'EXTERNAL + BUS', *, _nid);
	DevAddNode(_cn // ':LEVEL', 'NUMERIC', 1, *, _nid);
	DevAddNode(_cn // ':SLOPE', 'TEXT', 'POSITIVE', *, _nid);
	DevAddNode(_cn // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);

	/******** CH01 ********/

	_cn = _path // '.CH01';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':AMPLITUDE', 'NUMERIC', 10, *, _nid);
	DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':ZERO_REF', 'NUMERIC', 127.5, *, _nid);
	DevAddNode(_cn // ':FILTER', 'TEXT', 'OFF', *, _nid);


	_cn = _path // '.CH01' // '.WAVEFORM';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':POINTS', 'NUMERIC', *, *, _nid);
	DevAddNode(_cn // ':REPETITIONS', 'NUMERIC', 1, *, _nid);
	DevAddNode(_cn // ':DELAY', 'NUMERIC', 0, *, _nid);

	/******** CH02 ********/

	_cn = _path // '.CH02';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':AMPLITUDE', 'NUMERIC', 10, *, _nid);
	DevAddNode(_cn // ':OFFSET', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':ZERO_REF', 'NUMERIC', 127.5, *, _nid);
	DevAddNode(_cn // ':FILTER', 'TEXT', 'OFF', *, _nid);

	_cn = _path // '.CH02' // '.TRIGGER01';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':DELAY', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':DURATION', 'NUMERIC', 200E-6, *, _nid);
	DevAddNode(_cn // ':OUT', 'NUMERIC', compile(_path // '.TRIGGER:TRIG_SOURCE + ' // _path // '.CH02.TRIGGER01:DELAY'), *, _nid);

	_cn = _path // '.CH02' // '.TRIGGER02';
	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
	DevAddNode(_cn // ':DELAY', 'NUMERIC', 0, *, _nid);
	DevAddNode(_cn // ':DURATION', 'NUMERIC', 200E-6, *, _nid);
	DevAddNode(_cn // ':OUT', 'NUMERIC', compile(_path // '.TRIGGER:TRIG_SOURCE + ' // _path // '.CH02.TRIGGER02:DELAY'), *, _nid);

	/******** ACTIONS ********/

	DevAddAction(_path //':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path //':ARM_ACTION', 'INIT', 'ARM', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path //':RESET_ACTION', 'STORE', 'RESET', 50,'DMWR_SERVER', getnci(_path, 'fullpath'), _nid);

    	DevAddEnd();
}
