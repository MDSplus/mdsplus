public fun TOMOSetup__add(in _path, out _nidout)
{
	DevAddStart(_path, 'TOMOSetup', 51, _nidout);
    
	DevAddNode(_path // '.DBOT', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.DBOT';

	DevAddNode(_cn // ':ACQ_DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.ZERO', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.ZERO';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.RESET', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.RESET';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);



	DevAddNode(_path // '.DSXT', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.DSXT';

	DevAddNode(_cn // ':ACQ_DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.ZERO', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.ZERO';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.RESET', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.RESET';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);


	DevAddNode(_path // '.DSXC', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.DSXC';

	DevAddNode(_cn // ':ACQ_DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.ZERO', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.ZERO';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

	DevAddNode(_cn // '.RESET', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.RESET';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);
	
	DevAddNode(_path // '.DSXM', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.DSXM';
	
	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
	DevAddNode(_cn // ':STOP_ACQ', 'NUMERIC', *, *, _nid);
	DevAddNode(_cn // ':FEQUENCY', 'NUMERIC', *, *, _nid);
	
	DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);
	 
	DevAddEnd();
}


