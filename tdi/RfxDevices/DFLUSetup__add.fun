public fun DFLUSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'DFLUSetup', 35, _nidout);
    
    DevAddNode(_path // '.POLOIDAL', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.POLOIDAL';

    DevAddNode(_cn // '.CLOCK', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.CLOCK';
   	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':FREQ_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_cnn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':STOP_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
    DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);


    DevAddNode(_cn // '.AUTO_ZERO', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.AUTO_ZERO';
    DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.TOROIDAL', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TOROIDAL';

    DevAddNode(_cn // '.CLOCK', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.CLOCK';
   	DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':FREQ_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_cnn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':STOP_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_cn // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.TRIGGER';
    DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);


    DevAddNode(_cn // '.AUTO_ZERO', 'STRUCTURE', *, *, _nid);
	_cnn = _cn // '.AUTO_ZERO';
    DevAddNode(_cnn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cnn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cnn // ':DURATION', 'NUMERIC', *, *, _nid);


	 
    DevAddEnd();
}


