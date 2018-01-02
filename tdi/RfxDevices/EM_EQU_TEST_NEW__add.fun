public fun EM_EQU_TEST_NEW__add(in _path, out _nidout)
{

	DevAddStart(_path, 'EM_EQU_TEST_NEW', 55, _nidout);
	DevAddNode(_path // ':COMMENT', 	'TEXT', *, *, _nid);
	DevAddNode(_path // ':DECODER', 	'TEXT', *, *, _nid);
	DevAddNode(_path // ':SIG_DELAY',	'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':SIG_DURATION','NUMERIC', 5.E-3, *, _nid);
	DevAddNode(_path // ':POINT_ELAB',	'NUMERIC', 15, *, _nid);
	for (_c = 1; _c <= 12; _c++)
	{
		if(_c < 10)
			_cn = _path // '.CARD_0' // TEXT(_c, 1);
		else
			_cn = _path // '.CARD_' // TEXT(_c, 2);
		DevAddNode(_cn, 		'STRUCTURE', *, *, _nid);
	 	DevAddNode(_cn // ':GAIN',	'TEXT', *, *, _nid);
	 	DevAddNode(_cn // ':ADC_LIN',	'TEXT', *, *, _nid);
	 	DevAddNode(_cn // ':ADC_INT',	'TEXT', *, *, _nid);
	}
	DevAddAction(_path //':INIT_ACTION', 'INIT', 'INIT', 55, 'EM_SERVER', getnci(_path, 'fullpath'), _nid);
	DevAddEnd();
}
