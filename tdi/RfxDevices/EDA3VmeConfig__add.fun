public fun EDA3VmeConfig__add(in _path, out _nidout)
{
    DevAddStart(_path, 'EDA3VmeConfig', 21, _nidout);
    DevAddNode(_path // ':DELTA_T', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':THETA_NOM', 'NUMERIC', *, *, _nid);
write(*, 'CICCIO');
    DevAddNode(_path // ':TAU_VTHETA', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TAU_VTOR', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TAU_D', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':KP', 'NUMERIC', *, *, _nid);
write(*, 'CICCIO');
    DevAddNode(_path // ':KI', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':KD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VCOMP_ON', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':ICOMP_ON', 'NUMERIC', *, *, _nid);
write(*, 'CICCIO');
    DevAddNode(_path // ':IFB_ON', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':KVCOMP', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':IP_TRIG', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':T_TRIG', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DELTAVTORTR', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DELTATBT1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DELTATBT2', 'NUMERIC', *, *, _nid);
write(*, 'CICCIO');
    DevAddNode(_path // ':TUNING1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    DevAddAction(_path //':INIT_ACTION', 'INIT', 'INIT', 50,'VME_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

