public fun RFXABUnits__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXABUnits', 8, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PM_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PC_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PV_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TF_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':A_PARS', 'NUMERIC', ZERO(288, 0.), *, _nid);
    DevAddNode(_path // ':B_PARS', 'NUMERIC', ZERO(192, 0.), *, _nid);
   DevAddEnd();
}

 