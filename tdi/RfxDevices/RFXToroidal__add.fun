public fun RFXToroidal__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXToroidal', 21, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);

/*Parameters to be sent */

    DevAddNode(_path // ':REG_MODE1', 'TEXT',*, *, _nid);
    DevAddNode(_path // ':MODE_REM1', 'TEXT',*, *, _nid);
    DevAddNode(_path // ':BANK_SELECT1', 'TEXT', 'NONE', *, _nid);

    DevAddNode(_path // ':REG_MODE2', 'TEXT',*, *, _nid);
    DevAddNode(_path // ':MODE_REM2', 'TEXT',*, *, _nid);
    DevAddNode(_path // ':BANK_SELECT2', 'TEXT', 'NONE', *, _nid);

    DevAddNode(_path // ':SEND_PARAMS1', 'NUMERIC', 
	[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], *, _nid);

    DevAddNode(_path // ':SEND_PARAMS2', 'NUMERIC', 
	[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], *, _nid);

/* Parameters Received */
	
   DevAddNode(_path // ':COMMUTATORE1', 'TEXT', *, *, _nid);
   DevAddNode(_path // ':COMMUTATORE2', 'TEXT', *, *, _nid);
   DevAddNode(_path // ':DC8MF1', 'NUMERIC', [0,0,0,0,0,0], *, _nid);
   DevAddNode(_path // ':DC8MF2', 'NUMERIC', [0,0,0,0,0,0], *, _nid);
   DevAddNode(_path // ':REC_PARAMS1', 'NUMERIC', ZERO(450, 0.), *, _nid);
   DevAddNode(_path // ':REC_PARAMS2', 'NUMERIC', ZERO(450, 0.), *, _nid);

   DevAddNode(_path // ':TF_UNITS', 'TEXT', *, *, _nid);
   DevAddNode(_path // ':TF_CONFIG', 'TEXT', *, *, _nid);
   DevAddNode(_path // ':TF_CONTROL', 'TEXT', *, *, _nid);
   DevAddNode(_path // ':TF_ENABLED', 'NUMERIC', *, *, _nid);
   DevAddNode(_path // ':TF_WINDOW', 'NUMERIC', *, *, _nid);
 
   DevAddEnd();
}
       