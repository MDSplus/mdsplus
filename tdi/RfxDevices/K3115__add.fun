public fun K3115__add(in _path, out _nidout)
{
	DevAddStart(_path, 'K3115', 52, _nidout);

    	DevAddNode(_path // ':CAMAC_NAME', 'TEXT', *, *, _nid);
    	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    	DevAddNode(_path // ':CONTROL_MODE', 'TEXT', 'MASTER', *, _nid);
    	DevAddNode(_path // ':WAVE_MODE', 'TEXT', 'CYCLIC', *, _nid);

    	DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    	DevAddNode(_path // ':CLOCK_MODE', 'TEXT', 'INTERNAL', *, _nid);
    	DevAddNode(_path // ':CLOCK_FREQ', 'NUMERIC', 1E3, *, _nid);

    	for (_c = 1; _c <=6; _c++)
    	{
		_cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
        	DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	  	
        	DevAddNode(_cn // ':RANGE', 'NUMERIC', 10, *, _nid);
    	  	DevAddNode(_cn // ':RANGE_POL', 'TEXT', 'BIPOLAR', *, _nid);
    	  	DevAddNode(_cn // ':TIME_MODE', 'TEXT', 'AS IS', *, _nid);
        	DevAddNode(_cn // ':OUTPUT', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
        	DevAddNode(_cn // ':VOLTAGES', 'NUMERIC', *, *, _nid);
        	DevAddNode(_cn // ':TIMES', 'NUMERIC', *, *, _nid);
    	}

	DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    	DevAddAction(_path//':RESET_ACTION', 'STORE','RESET', 50,'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    	DevAddEnd();
}