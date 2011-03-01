public fun LASER_SS__add(in _path, out _nidout)
{
    DevAddStart(_path, 'LASER_SS', 12, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);

    DevAddNode(_path // ':ENERGY', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':NUM_POWER', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':REP_RATE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_LAMP', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_DIODE', 'NUMERIC', 0, *, _nid);

	/******** ACTIONS ********/

    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION',  'INIT', 50,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION',   'INIT',   'ARM', 20,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ABORT_ACTION', 'STORE', 'ABORT', 70,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
