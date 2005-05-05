public fun ST133__add(in _path, out _nidout)
{
    DevAddStart(_path, 'ST133', 30, _nidout);
    DevAddNode(_path // ':COMMENT',      'TEXT',    *, *, _nid);
    DevAddNode(_path // ':IP_ADDRESS',   'TEXT',    *, *, _nid);
	DevAddNode(_path // ':CLOCK_MODE',   'TEXT',    'EXTERNAL', *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SPEC_GROOVES', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SPEC_WAV_MID', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SPEC_MIR_POS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':GLUE_FLAG',    'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':X_DIM_DET',    'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':Y_DIM_DET',    'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':X_DIM',        'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':Y_DIM',        'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DET_TEMP',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':EXP_TIME',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':READ_OUT',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CLEANS',       'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':N_SKIP_CLEAN', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':N_FRAMES',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':NROI',         'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':ROI',          'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':NUM_ACC',      'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':XCAL_VALID',   'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':XCAL_STING',   'TEXT',    *, *, _nid);
    DevAddNode(_path // ':XCAL_POL_ORD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':XCAL_COEFF',   'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':HEADER',       'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':DATA',         'SIGNAL' , *, *, _nid);

    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'CCD_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'STORE', 70,'CCD_SERVER', getnci(_path, 'fullpath'), _nid);

    DevAddEnd();
}
