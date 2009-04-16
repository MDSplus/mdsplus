public fun RFXPC4Setup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPC4Setup',54, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':UNITS', 'TEXT', *, *, _nid);
    for (_c = 1; _c <=4; _c++)
    {
        _cn = _path // '.WAVE_' // TEXT(_c, 1);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
		DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':OUT_GAINS', 'NUMERIC', [1,1,1], *, _nid);
  		DevAddNode(_cn // ':PERT_AMP', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_FREQ', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_PHASE', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_START', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_STOP', 'NUMERIC', 0, *, _nid);
	}
    DevAddEnd();
}
