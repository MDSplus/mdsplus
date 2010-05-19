public fun IPControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'IPControl',25, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TSTART', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TEND', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':KP', 'NUMERIC', *, *, _nid);
    _cn = _path //'.TOKAMAK'; 
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':VMAX', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
    _cn = _path //'.RFP'; 
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn // ':IPSTAR', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':DELTAIPSTAR', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':DELTARAMPD', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':POHMMAX', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':TAUZ', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':TAUP', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
    DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TYPE', 'NUMERIC', *, *, _nid);
    DevAddEnd();
}

