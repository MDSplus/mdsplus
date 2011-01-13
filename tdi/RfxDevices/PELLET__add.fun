public fun PELLET__add(in _path, out _nidout)
{
    DevAddStart(_path,'PELLET', 146, _nidout);
    DevAddNode(_path//':RS232_NAME', 'TEXT','/dev/ttyaa15', *, _nid);
    DevAddNode(_path//':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path//':TSTBY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':TAFORM', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':TBFORM', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':THOLD', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':TBAKE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':DELAY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':PRESSURE', 'NUMERIC', *, *, _nid);

    for (_c = 1; _c <= 8; _c++)
    {
        _cn = _path//'.PELLET_'//TEXT(_c, 1);
		_tag = 'pel_t'//TEXT(_c, 1);

		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		
        DevAddNode(_cn//':TRIG_TIME', 'NUMERIC', 0, *, _nid);			 
		 tcl('add tag '//_cn //':TRIG_TIME '//_tag);
		 
        DevAddNode(_cn//':SPEED', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':MASS', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':DRIGAS', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':OHOLD', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':OFORM', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':OSTBY', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':STBY', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':AFORM', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':BFORM', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':HOLD', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':FIRE', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':BAKE', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':TOFA', 'NUMERIC', *, *, _nid);
    	DevAddNode(_cn//':TOFB', 'NUMERIC', *, *, _nid);
    }

    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'PC_SERVER',getnci(_path, 'fullpath'), _nid);

    DevAddNode(_path//':ANGLE', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':DI_MIN', 'NUMERIC', *, *, _nid);
    DevAddNode(_path//':GAS_TYPE', 'TEXT', *, *, _nid);
    DevAddNode(_path//':IGOR_NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path//':I_N_MAX', 'NUMERIC', *, *, _nid);
 
    DevAddNode(_path//':VALVE_STATE', 'TEXT', "DISABLE", *, _nid);
    DevAddAction(_path//':INIT_ACTION', 'PULSE_PREPARATION','INIT', 50,'PC_SERVER',getnci(_path, 'fullpath'), _nid);

	DevAddEnd();
}
