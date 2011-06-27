public fun LASER_YAG__add(in _path, out _nidout)
{
    DevAddStart(_path, 'LASER_YAG', 14, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);

    DevAddNode(_path // ':ENERGY', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':REP_RATE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_PULSE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_LAMP', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_DIODE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':WAIT_SIMON', 'NUMERIC', 35, *, _nid);
    
	/******** ACTIONS ********/
    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION',  'INIT', 50,'THOMSON_SERVER_1', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION',   'INIT',			   'ARM', 50,'THOMSON_SERVER_1', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ABORT_ACTION', 'ABNORMAL',		   'ABORT', 70,'THOMSON_SERVER_1', getnci(_path, 'fullpath'), _nid);
    
    DevAddNode(_path // ':RS232_PORT',  'TEXT', *, *, _nid);

    DevAddEnd();
}
