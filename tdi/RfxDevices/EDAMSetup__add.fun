public fun EDAMSetup__add(in _path, out _nidout)
{
    write(*, _path);
    DevAddStart(_path, 'EDAMSetup', 61, _nidout);
    DevAddNode(_path // '.PBMC_IF', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMC_IF';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
  	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.PBMC_IM', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMC_IM';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.PBMV_UF', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMV_UF';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
  	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // '.PBMV_UM', 'STRUCTURE', *, *, _nid);
	_cn = _path // '.PBMV_UM';
   	DevAddNode(_cn // ':EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_1', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':FREQ_2', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':TRIG_EVENT', 'TEXT', 'T_START_RFX', *, _nid);
   	DevAddNode(_cn // ':TRIG_DELAY', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':START_ACQ', 'NUMERIC', *, *, _nid);
   	DevAddNode(_cn // ':END_ACQ', 'NUMERIC', *, *, _nid);
	 
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
	 
    DevAddEnd();
}


