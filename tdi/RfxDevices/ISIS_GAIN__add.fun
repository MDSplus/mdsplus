public fun ISIS_GAIN__add(in _path, out _nidout)
{
    DevAddStart(_path, 'ISIS_GAIN', 203, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':IP_ADDRESS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PORT', 'NUMERIC', 1007, *, _nid);

	for (_c = 1; _c <=18; _c++)
	{
		if(_c < 10)
		    _cn = _path // '.CARD_0' // TEXT(_c, 1);
	    else
		    _cn = _path // '.CARD_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE',                *,  *, _nid);
      	DevAddNode(_cn // ':NAME_1',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':CALIB_1',  'NUMERIC', 1., *, _nid);
      	DevAddNode(_cn // ':GAIN_1',   'NUMERIC', 1,  *, _nid);
	    DevAddNode(_cn // ':INPUT_1',  'NUMERIC', compile('('//_cn//':OUTPUT_1) / ('//_cn//':GAIN_1 ) *  ( '//_cn//':CALIB_1) '),  '/nomodel_write', _nid);
   	    DevAddNode(_cn // ':OUTPUT_1', 'NUMERIC', *,  *, _nid);

      	DevAddNode(_cn // ':NAME_2',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':CALIB_2',  'NUMERIC', 1., *, _nid);
      	DevAddNode(_cn // ':GAIN_2',   'NUMERIC', 1,  *, _nid);
	    DevAddNode(_cn // ':INPUT_2',  'NUMERIC', compile('('//_cn//':OUTPUT_2) / ('//_cn//':GAIN_2 ) *  ( '//_cn//':CALIB_2) '),  '/nomodel_write', _nid);
   	    DevAddNode(_cn // ':OUTPUT_2', 'NUMERIC', *,  *, _nid);

	}
    DevAddAction(_path//':INIT_ACTION', 'PULSE_PREPARATION', 'INIT', 55, 'ISIS_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
