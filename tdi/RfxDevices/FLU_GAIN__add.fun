public fun FLU_GAIN__add(in _path, out _nidout)
{
    DevAddStart(_path, 'FLU_GAIN', 181, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':ADC', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':ACQ_SIGNALS', 'TEXT', 'INTEGRAL', *, _nid);

	for (_c = 1; _c <=16; _c++)
	{
		if(_c < 10)
		    _cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
	    else
		    _cn = _path // '.CHANNEL_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE',                *,  *, _nid);
      	DevAddNode(_cn // ':ATTENUATION','NUMERIC', 1,  *, _nid);
      	DevAddNode(_cn // ':LIN_NAME',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':LIN_CALIB',  'NUMERIC', 1., *, _nid);
	    DevAddNode(_cn // ':LIN_INPUT',  'NUMERIC', compile('('//_cn//':LIN_OUTPUT) * ('//_cn//':ATTENUATION ) /  ( '//_cn//':LIN_CALIB) '),  '/nomodel_write', _nid);
   	    DevAddNode(_cn // ':LIN_OUTPUT', 'NUMERIC', *,  *, _nid);

	    DevAddNode(_cn // ':INT_NAME',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':INT_CALIB',  'NUMERIC', 1., *, _nid);   	    
   	    DevAddNode(_cn // ':INT_GAIN',   'NUMERIC', 1., *, _nid);
  	    DevAddNode(_cn // ':INT_INPUT',  'NUMERIC', compile('('//_cn//':INT_OUTPUT) * ('//_cn//':ATTENUATION ) / ( ('//_cn//':INT_GAIN ) * ( '//_cn//':INT_CALIB) )'),  '/nomodel_write', _nid);
   	    DevAddNode(_cn // ':INT_OUTPUT', 'NUMERIC', *,  *, _nid);
	}
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 55, 'EM_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
