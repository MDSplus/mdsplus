public fun RFXPSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPSetup',8, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_Y', 'NUMERIC', *, *, _nid);
    DevAddEnd();
}

       
