public fun BUILD_VERSION__add(in _path, out _nidout)
{
    DevAddStart(_path, 'BUILD_VERSION', 104, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VERSION', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SHOTS', 'NUMERIC', *, *, _nid);

    _cs = _path // '.EXP_01';
    DevAddNode(_cs, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cs // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_cs // ':NAME', 'TEXT', 'A', *, _nid);
    DevAddNode(_cs // ':PRE_PROGRAM', 'ANY', *, *, _nid);
    DevAddNode(_cs // ':PROGRAMS', 'ANY', *, *, _nid);

    for (_s = 2; _s <=20; _s++)
    {
		if( _s < 10)
			_cs = _path // '.EXP_0' // TEXT(_s, 1);
		else
			_cs = _path // '.EXP_' // TEXT(_s, 2);

		DevAddNode(_cs, 'STRUCTURE', *, *, _nid);
    		DevAddNode(_cs // ':COMMENT', 'TEXT', *, *, _nid);
		DevAddNode(_cs // ':NAME', 'TEXT', *, *, _nid);
    		DevAddNode(_cs // ':PRE_PROGRAM', 'ANY', *, *, _nid);
		DevAddNode(_cs // ':PROGRAMS', 'ANY', *, *, _nid);
	}

    DevAddEnd();
}
