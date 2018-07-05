public fun EM_FLU_TEST__add(in _path, out _nidout)
{

    DevAddStart(_path, 'EM_FLU_TEST', 86, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

	DevAddNode(_path // '.POLOIDAL', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.POLOIDAL';

    DevAddNode(_cn // ':DECODER_1',   'TEXT', *, *, _nid);
    DevAddNode(_cn // ':DECODER_2',   'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SIG_DELAY',   'NUMERIC', 0, *, _nid);
    DevAddNode(_cn // ':SIG_DURATION','NUMERIC', 5.E-3, *, _nid);

	for (_c = 1; _c <= 12; _c++)
	{
		if(_c < 10)
		    _cnn = _cn // '.CARD_0' // TEXT(_c, 1);
	    else
		    _cnn = _cn // '.CARD_' // TEXT(_c, 2);

        DevAddNode(_cnn, 'STRUCTURE', *, *, _nid);
     	DevAddNode(_cnn // ':GAIN',   'TEXT', *, *, _nid);
     	DevAddNode(_cnn // ':ADC',    'TEXT', *, *, _nid);

	}

	DevAddNode(_path // '.TOROIDAL', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TOROIDAL';

    DevAddNode(_cn // ':DECODER_1',   'TEXT', *, *, _nid);
    DevAddNode(_cn // ':DECODER_2',   'TEXT', *, *, _nid);
    DevAddNode(_cn // ':SIG_DELAY',   'NUMERIC', 0, *, _nid);
    DevAddNode(_cn // ':SIG_DURATION',   'NUMERIC', 5.E-3, *, _nid);

	for (_c = 1; _c <= 12; _c++)
	{
		if(_c < 10)
		    _cnn = _cn // '.CARD_0' // TEXT(_c, 1);
	    else
		    _cnn = _cn // '.CARD_' // TEXT(_c, 2);

        DevAddNode(_cnn, 'STRUCTURE', *, *, _nid);
     	DevAddNode(_cnn // ':GAIN',   'TEXT', *, *, _nid);
     	DevAddNode(_cnn // ':ADC',    'TEXT', *, *, _nid);

	}


    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 55, 'EM_SERVER', getnci(_path, 'fullpath'), _nid);

    DevAddNode(_path // ':POINT_ELAB', 'NUMERIC', 15, *, _nid);
 
    DevAddEnd();
}
