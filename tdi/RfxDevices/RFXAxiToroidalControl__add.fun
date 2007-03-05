public fun RFXAxiToroidalControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXAxiToroidalControl',26, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	_names = ["WallBt", "F", "Q", "Flux"]; 

	for(_i = 0; _i < 4; _i++)
	{
		_cn = _path // "." // trim(_names[_i]);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
		DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
    }

    DevAddEnd();
}
