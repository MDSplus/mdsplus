public fun T2Control__add(in _path, out _nidout)
{

write(*,'T2Control__add'); 

    DevAddStart(_path, 'T2Control', 147, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION', 'NUMERIC', 
		[1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':INIT_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG1_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG2_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG1_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DURATION', 'NUMERIC', 10., *, _nid);
    DevAddNode(_path // ':TRIG2_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PAR1_NAME', 'TEXT', "Kp", *, _nid);
    DevAddNode(_path // ':PAR1_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR2_NAME', 'TEXT', "Ki", *, _nid);
    DevAddNode(_path // ':PAR2_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR3_NAME', 'TEXT', "M", *, _nid);
    DevAddNode(_path // ':PAR3_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR4_NAME', 'TEXT', "N", *, _nid);
    DevAddNode(_path // ':PAR4_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR5_NAME', 'TEXT', "Amplitude", *, _nid);
    DevAddNode(_path // ':PAR5_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR6_NAME', 'TEXT', "AngVelocity", *, _nid);
    DevAddNode(_path // ':PAR6_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR7_NAME', 'TEXT', "InitPhase", *, _nid);
    DevAddNode(_path // ':PAR7_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR8_NAME', 'TEXT', "K1", *, _nid);
    DevAddNode(_path // ':PAR8_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR9_NAME', 'TEXT', "K2", *, _nid);
    DevAddNode(_path // ':PAR9_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR10_NAME', 'TEXT', "K3", *, _nid);
    DevAddNode(_path // ':PAR10_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR11_NAME', 'TEXT', "K4", *, _nid);
    DevAddNode(_path // ':PAR11_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR12_NAME', 'TEXT', "K5", *, _nid);
    DevAddNode(_path // ':PAR12_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR13_NAME', 'TEXT', "K6", *, _nid);
    DevAddNode(_path // ':PAR13_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR14_NAME', 'TEXT', "K7", *, _nid);
    DevAddNode(_path // ':PAR14_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR15_NAME', 'TEXT', "K8", *, _nid);
    DevAddNode(_path // ':PAR15_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR16_NAME', 'TEXT', "K9", *, _nid);
    DevAddNode(_path // ':PAR16_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR17_NAME', 'TEXT', "K10", *, _nid);
    DevAddNode(_path // ':PAR17_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR18_NAME', 'TEXT', "K11", *, _nid);
    DevAddNode(_path // ':PAR18_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR19_NAME', 'TEXT', "K12", *, _nid);
    DevAddNode(_path // ':PAR19_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR20_NAME', 'TEXT', "K13", *, _nid);
    DevAddNode(_path // ':PAR20_VAL', 'NUMERIC', 0., *, _nid);

    for (_c = 1; _c <=9; _c++)
    {
        DevAddNode(_path // ':INPUT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=64; _c++)
    {
        DevAddNode(_path // ':INPUT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <=9; _c++)
    {
        DevAddNode(_path // ':OUTPUT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=32; _c++)
    {
        DevAddNode(_path // ':OUTPUT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}

    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

       

