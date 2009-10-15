public fun EDAM_CPCISetup__add(in _path, out _nidout)
{
	write(*, "EDAM_CPCI");
	DevAddStart(_path, 'EDAM_CPCISetupV1', 55, _nidout);
    DevAddNode(_path // '.TBMC', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TBMC';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
  	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.TBMV', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.TBMV';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.PBMC', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMC';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
  	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.PBMV', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMV';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);
	 
    DevAddNode(_path // '.ZERO', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.ZERO';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
	
    DevAddNode(_path // '.PBV_TBV_FAST', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBV_TBV_FAST';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);
	
	 	 
    DevAddEnd();
}


