public fun RFXPVSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPVSetup',55, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OUT_GAINS', 'NUMERIC', [1,1,1], *, _nid);
	for (_c = 1; _c <=8; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
		DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
    }
    DevAddEnd();
}
