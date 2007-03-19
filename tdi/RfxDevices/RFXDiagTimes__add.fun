public fun RFXDiagTimes__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXDiagTimes', 4, _nidout);

    DevAddNode(_path // ':ND_TRIGGER', 'NUMERIC', -0.025, *, _nid);
	tcl('add tag '//_path //':ND_TRIGGER T_ND_TRIGGER');

    DevAddNode(_path // ':ND_PERIOD', 'NUMERIC', -0.05, *, _nid);
	tcl('add tag ' // _path // ':ND_PERIOD T_ND_PERIOD');

    DevAddNode(_path // ':ND_SEQUENCE', 'NUMERIC', *, *, _nid);

    DevAddEnd();
}

       

