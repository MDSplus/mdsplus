public fun RFXParameters__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXParameters', 20, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COILS_E_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':COILS_I_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':COILS_RES', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':GRPHTE_E_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':GRPHTE_I_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LINER_E_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LINER_I_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':L_P_LINER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':L_T_LINER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAJOR_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':POLVLOOP_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_P_LINER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_T_LINER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SHELL_E_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SHELL_I_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SHELL_RES', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TORVLOOP_RAD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':WALL_RAD', 'NUMERIC', *, *, _nid);
    DevAddEnd();
}

