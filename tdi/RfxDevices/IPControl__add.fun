public fun IPControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'IPControl',15, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TYPE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TSTART', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TEND', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VMAX', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PAR1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PAR2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PAR3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PAR4', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_Y', 'NUMERIC', *, *, _nid);
    DevAddEnd();
}

