public fun DEQUSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'DEQUSetup', 25, _nidout);
    
		DevAddNode(_path // '.TR10_ACQ', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TR10_ACQ';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

        DevAddNode(_path // '.CADH_ACQ', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.CADH_ACQ';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
 	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

        DevAddNode(_path // '.AUTO_ZERO', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.AUTO_ZERO';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);

        DevAddNode(_path // '.TRIGGER', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TRIGGER';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
 
	 
    DevAddEnd();
}


