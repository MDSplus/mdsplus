public fun RFXChopperSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXChopperSetup',9, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OUT_GAIN', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_path // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAX_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':OUT_CALIB', 'NUMERIC', [1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0], *, _nid);
    DevAddEnd();
}



       
