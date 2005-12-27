public fun GEN_GAIN__add(in _path, out _nidout)
{
    DevAddStart(_path, 'GEN_GAIN', 83, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':ADC', 'TEXT', *, *, _nid);

	for (_c = 1; _c <=16; _c++)
	{
		if(_c < 10)
		    _cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
	    else
		    _cn = _path // '.CHANNEL_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE',            *,  *, _nid);
      	DevAddNode(_cn // ':NAME',   'TEXT',    *,  *, _nid);
	    DevAddNode(_cn // ':CALIB',  'NUMERIC', 1., *, _nid);
	    DevAddNode(_cn // ':INPUT',  'NUMERIC', compile( '('//_cn//':OUTPUT) * ('//_cn//':CALIB)' ), *, _nid);
   	    DevAddNode(_cn // ':OUTPUT', 'NUMERIC', 0., *, _nid);

	}
    DevAddEnd();
}
