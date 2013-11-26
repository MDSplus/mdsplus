public fun RFXAxiControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXAxiControl',142, _nidout); 
/*    DevAddStart(_path, 'RFXAxiControl',140, _nidout); */
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	_names = ["BV_ADD", "DELTA_IP", "REF_DELTA_H", "I_FS1_ADD", "I_FS2_ADD", 
	"I_FS3_ADD", "I_FS4_ADD", "I_FS5_ADD", "I_FS6_ADD", "I_FS7_ADD", "I_FS8_ADD",
	"DELTA_I_FS1", "DELTA_I_FS2", "DELTA_I_FS3", "DELTA_I_FS4", "DELTA_I_FS5", 
	"DELTA_I_FS6", "DELTA_I_FS7", "DELTA_I_FS8", "RPLA_COS2", "RPLA_COS3", "AVGPLASMAREF", "REF_DELTA_V"]; 

/*	for(_i = 0; _i < 21; _i++) */
	for(_i = 0; _i < 23; _i++)
	{
		_cn = _path // "." // trim(_names[_i]);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
		DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);
    }
	DevAddNode(_path // ':BV_GAIN', 'NUMERIC', 1., *, _nid);
	DevAddNode(_path // ':T_START_INT', 'NUMERIC', 1., *, _nid);

    DevAddEnd();
}
