public fun RFXVISetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXVISetup',15, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FILLING_TYPE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PRESSURE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':IMP_LEVEL', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILL_WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':FILL_MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILL_MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILL_MIN_Y', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':FILL_MAX_Y', 'NUMERIC', 100., *, _nid);
    DevAddNode(_path // ':PUFF_WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':PUFF_MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PUFF_MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PUFF_MIN_Y', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PUFF_MAX_Y', 'NUMERIC', 170., *, _nid);
    DevAddEnd();
}



       
