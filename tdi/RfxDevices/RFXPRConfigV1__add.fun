public fun RFXPRConfigV1__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPRConfigV1', 43, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

/*Parameters to be sent */
    DevAddNode(_path // ':BAR_VOLT_1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_CURR_1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_VOLT_2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_CURR_2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_VOLT_3', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_CURR_3', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_VOLT_4', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BAR_CURR_4', 'NUMERIC', 0, *, _nid);
		
    DevAddNode(_path // ':SENT_1_12', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_13_24', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_25_36', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_37_48', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_49_60', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_61_72', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_73_84', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_85_96', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_97_108', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_109_120', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_121_132', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_133_144', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_145_156', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_157_168', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_169_180', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':SENT_181_192', 'NUMERIC', ZERO(168, 0.), *, _nid);
    DevAddNode(_path // ':CHOP_STATE', 'NUMERIC', ZERO(192, 1), *, _nid);

/* Parameters Received */
	
    DevAddNode(_path // ':REC_1_12', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_13_24', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_25_36', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_37_48', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_49_60', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_61_72', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_73_84', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_85_96', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_97_108', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_109_120', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_121_132', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_133_144', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_145_156', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_157_168', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_169_180', 'NUMERIC', ZERO(552, 0.), *, _nid);
    DevAddNode(_path // ':REC_181_192', 'NUMERIC', ZERO(552, 0.), *, _nid);
 
   DevAddEnd();
}
       
