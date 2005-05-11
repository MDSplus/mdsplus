public fun RFXPCSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPCSetup',17, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':OUT_GAINS', 'NUMERIC', [1,1,1], *, _nid);
  	DevAddNode(_path // ':PERT_AMP', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_path // ':PERT_FREQ', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_path // ':PERT_PHASE', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_path // ':PERT_START', 'NUMERIC', 0, *, _nid);
  	DevAddNode(_path // ':PERT_STOP', 'NUMERIC', 0, *, _nid);
    DevAddEnd();
}



       

