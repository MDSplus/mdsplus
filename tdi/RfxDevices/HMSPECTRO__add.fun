public fun HMSPECTRO__add(in _path, out _nidout)
{
    DevAddStart(_path, 'HMSPECTRO', 19, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TYPE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_EDGE', 'TEXT', 'NO_FUNCTION', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':NUM_SCAN', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':GAIN', 'TEXT', 'LOW', *, _nid);
    DevAddNode(_path // ':INTEG_TIME', 'NUMERIC', 0.1, *, _nid);
    DevAddNode(_path // ':CALIBRATION', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION', 'INIT', 40,'CCD_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION', 'PULSE_PREPARATION', 'ARM', 50,'CCD_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'CCD_SERVER',getnci(_path, 'fullpath'), _nid);
write(*, 'Build_Signal(transpose(DATA('//_path//':DATA)), *, DIM_OF('//_path//':DATA),'//_path//':LAMBDA)' );
    DevAddNode(_path // ':LAMBDA', 'NUMERIC', *, '/compress_on_put/nomodel_write', _nid);
_a = compile( 'Make_Signal(TRANSPOSE(DATA('//_path//':DATA)), *, DIM_OF('//_path//':DATA),'//_path//':LAMBDA)' );
    DevAddNode(_path // ':SPECTRA', 'NUMERIC',  _a   , *, _nid);
    DevAddEnd();
}
