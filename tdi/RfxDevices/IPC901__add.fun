public fun IPC901__add(in _path, out _nidout)
{
	DevAddStart(_path, 'IPC901', 37, _nidout);
	DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':WAVE_LEN_1', 'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':WAVE_LEN_2', 'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':TRIG_MODE', 'TEXT', "INTERNAL", *, _nid);
	DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':END_TIME', 'NUMERIC', 0.1, *, _nid);
	DevAddNode(_path // ':ACQ_MODE', 'TEXT', "MEASURE", *, _nid);
	DevAddNode(_path // ':STORE_FLAG', 'TEXT', "ENABLED", *, _nid);
	DevAddNode(_path // ':DAC_GAIN', 'NUMERIC', 1, *, _nid);
	DevAddNode(_path // ':DAC_OUTPUT', 'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':FIR_CUT_OFF', 'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':FIR_WINDOW', 'NUMERIC', 8, *, _nid);
	DevAddNode(_path // ':HARD_DECIM', 'NUMERIC', 1, *, _nid);
	DevAddNode(_path // ':SOFT_DECIM', 'NUMERIC', 0, *, _nid);
	DevAddNode(_path // ':OVFL_LEV_1', 'NUMERIC', 1.5, *, _nid);
	DevAddNode(_path // ':OVFL_LEV_2', 'NUMERIC', 1.5, *, _nid);
	DevAddNode(_path // ':UNFL_LEV_1', 'NUMERIC', 0.5, *, _nid);
 	DevAddNode(_path // ':UNFL_LEV_2', 'NUMERIC', 0.5, *, _nid);
	DevAddNode(_path // ':CORRECTION_1', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':CORRECTION_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':INPUT_IMP', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':OV_UNFL_1', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':OV_UNFL_2', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':LOW_PASS_1', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':LOW_PASS_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':GAIN_1', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':GAIN_2', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':STORE_FREQ', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':PHASE_DIFF', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	DevAddNode(_path // ':INITIAL_DIFF', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':PHASE_1', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	DevAddNode(_path // ':PHASE_2', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	DevAddNode(_path // ':FLAGS',   'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
	DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
	DevAddEnd();
}


