public fun RFXTiming__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXTiming', 3, _nidout);
    DevAddNode(_path // ':EVENT_NAMES', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':EVENT_TIMES', 'TEXT', *, *, _nid);
    DevAddEnd();
}

       

