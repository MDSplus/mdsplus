public fun RFXAxiControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXAxiControl',68, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	_names = ["BV_ADD", "DELTA_IP", "REF_DELTA_H", "I_FS1_ADD", "I_FS2_ADD", 
	"I_FS3_ADD", "I_FS4_ADD", "I_FS5_ADD", "I_FS6_ADD", "I_FS7_ADD", "I_FS8_ADD"]; 

	for(_i = 0; _i < 11; _i++)
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
