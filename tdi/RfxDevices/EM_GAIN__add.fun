public fun EM_GAIN__add(in _path, out _nidout)
{
    DevAddStart(_path, 'EM_GAIN', 181, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':BIRA_CTRLR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CARD_ADDR', 'NUMERIC', *, *, _nid);
	for (_c = 1; _c <=16; _c++)
	{
		if(_c < 10)
		    _cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
	    else
		    _cn = _path // '.CHANNEL_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE',                *,  *, _nid);
      	DevAddNode(_cn // ':LIN_NAME',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':LIN_CALIB',  'NUMERIC', 1., *, _nid);
   	    DevAddNode(_cn // ':LIN_GAIN',   'NUMERIC', 0., *, _nid);
	    DevAddNode(_cn // ':LIN_INPUT',  'NUMERIC', compile('('//_cn//':LIN_OUTPUT) / ( ('//_cn//':LIN_GAIN ) * ( '//_cn//':LIN_CALIB) )'), *, _nid);
   	    DevAddNode(_cn // ':LIN_OUTPUT', 'NUMERIC', 0., *, _nid);

	    DevAddNode(_cn // ':INT_NAME',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':INT_CALIB',  'NUMERIC', 1., *, _nid);   	    
   	    DevAddNode(_cn // ':INT_GAIN',   'NUMERIC', 0., *, _nid);
  	    DevAddNode(_cn // ':INT_INPUT',  'NUMERIC', compile('('//_cn//':INT_OUTPUT) / ( ('//_cn//':INT_GAIN ) * ( '//_cn//':INT_CALIB) )'), *, _nid);
   	    DevAddNode(_cn // ':INT_OUTPUT', 'NUMERIC', 0., *, _nid);
	}
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 55, 'EM_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
