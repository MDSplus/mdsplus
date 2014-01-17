public fun RFXPoloidal__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPoloidal', 45, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':LOAD_INDUCT', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RESIST', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PLASMA_IND', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PM_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PM_CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PM_CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PM_ENABLED', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PM_WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PC_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PC_CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PC_CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PC_ENABLED', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PC_WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PV_UNITS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PV_CONFIG', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PV_CONTROL', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PV_ENABLED', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PV_WINDOW', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_TRANSFER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':PTCB_VOLTAGE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RA', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RB', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RF', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':R_TRANSFER2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RA2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RB2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RF2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_TRANSFER3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RA3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RB3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RF3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_TRANSFER4', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RA4', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RB4', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':LOAD_RF4', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':R_COIL_F1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F4', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F5', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F6', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F7', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':R_COIL_F8', 'NUMERIC', *, *, _nid);

    DevAddEnd();
}

 
