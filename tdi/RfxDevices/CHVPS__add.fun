public fun CHVPS__add(in _path, out _nidout)
{

    DevAddStart(_path, 'CHVPS', 487, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CRATE_NUMBER', 'NUMERIC', *, *, _nid);

	for (_c = 0; _c < 40; _c++)
	{
		if(_c < 10)
		    _cn = _path // '.CHAN_SET_0' // TEXT(_c, 1);
	    else
		    _cn = _path // '.CHAN_SET_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
     	DevAddNode(_cn // ':COMMENT',   'TEXT', *, *, _nid);
   	    DevAddNode(_cn // ':ADDRESS',   'NUMERIC', 0., *, _nid);
     	DevAddNode(_cn // ':CHAN_TYPE',   'TEXT', *, *, _nid);
   	    DevAddNode(_cn // ':I0',   'NUMERIC', 0., *, _nid);
   	    DevAddNode(_cn // ':I1',   'NUMERIC', 0., *, _nid);
   	    DevAddNode(_cn // ':RAMP_DOWN',   'NUMERIC', 0., *, _nid);
  	    DevAddNode(_cn // ':RAMP_UP',   'NUMERIC', 0., *, _nid);
     	DevAddNode(_cn // ':RESET',     'TEXT', *, *, _nid);
  	    DevAddNode(_cn // ':TRIP',   'NUMERIC', 0., *, _nid);
  	    DevAddNode(_cn // ':V0',   'NUMERIC', 0., *, _nid);
  	    DevAddNode(_cn // ':V1',   'NUMERIC', 0., *, _nid);
	}

    DevAddAction(_path//':INIT_ACTION', 'INIT', 'PULSE_PREPARATION', 15, 'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':TRIG_ACTION', 'TRIG', 'PULSE_PREPARATION', 50, 'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':RESET_ACTION', 'RESET', 'STORE', 50, 'CAMAC_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
