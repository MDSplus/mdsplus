public fun T2Control__add(in _path, out _nidout)
{
write(*,'T2Control__add'); 
    DevAddStart(_path, 'T2Control', 334, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':IN_CALIB', 'NUMERIC', 
		[1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':OUT_CALIB', 'NUMERIC', 
		[1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':INIT_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':CONTR_DURAT', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':SYS_DURAT', 'NUMERIC', 10., *, _nid);
    DevAddNode(_path // ':PRE_TIME', 'NUMERIC', -.5, *, _nid);
    DevAddNode(_path // ':POST_TIME', 'NUMERIC', .5, *, _nid);
    DevAddNode(_path // ':PAR1_NAME', 'TEXT', "Kp", *, _nid);
    DevAddNode(_path // ':PAR1_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR2_NAME', 'TEXT', "Ki", *, _nid);
    DevAddNode(_path // ':PAR2_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR3_NAME', 'TEXT', "Kd", *, _nid);
    DevAddNode(_path // ':PAR3_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR4_NAME', 'TEXT', "N1", *, _nid);
    DevAddNode(_path // ':PAR4_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR5_NAME', 'TEXT', "Amplitude1", *, _nid);
    DevAddNode(_path // ':PAR5_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR6_NAME', 'TEXT', "Omega1", *, _nid);
    DevAddNode(_path // ':PAR6_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR7_NAME', 'TEXT', "Phase1", *, _nid);
    DevAddNode(_path // ':PAR7_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR8_NAME', 'TEXT', "AngVelocity1", *, _nid);
    DevAddNode(_path // ':PAR8_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR9_NAME', 'TEXT', "InitPhase1", *, _nid);
    DevAddNode(_path // ':PAR9_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR10_NAME', 'TEXT', "N2", *, _nid);
    DevAddNode(_path // ':PAR10_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR11_NAME', 'TEXT', "Amplitude2", *, _nid);
    DevAddNode(_path // ':PAR11_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR12_NAME', 'TEXT', "Omega2", *, _nid);
    DevAddNode(_path // ':PAR12_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR13_NAME', 'TEXT', "Phase2", *, _nid);
    DevAddNode(_path // ':PAR13_VAL', 'NUMERIC', 0., *, _nid);
	DevAddNode(_path // ':PAR14_NAME', 'TEXT', "AngVelocity2", *, _nid);
    DevAddNode(_path // ':PAR14_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR15_NAME', 'TEXT', "InitPhase2", *, _nid);
    DevAddNode(_path // ':PAR15_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR16_NAME', 'TEXT', "N3", *, _nid);
    DevAddNode(_path // ':PAR16_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR17_NAME', 'TEXT', "Amplitude3", *, _nid);
    DevAddNode(_path // ':PAR17_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR18_NAME', 'TEXT', "Omega3", *, _nid);
    DevAddNode(_path // ':PAR18_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR19_NAME', 'TEXT', "Phase3", *, _nid);
    DevAddNode(_path // ':PAR19_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR20_NAME', 'TEXT', "AngVelocity3", *, _nid);
    DevAddNode(_path // ':PAR20_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR21_NAME', 'TEXT', "InitPhase3", *, _nid);
    DevAddNode(_path // ':PAR21_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR22_NAME', 'TEXT', "N4", *, _nid);
    DevAddNode(_path // ':PAR22_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR23_NAME', 'TEXT', "Amplitude4", *, _nid);
    DevAddNode(_path // ':PAR23_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR24_NAME', 'TEXT', "Omega4", *, _nid);
    DevAddNode(_path // ':PAR24_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR25_NAME', 'TEXT', "Phase4", *, _nid);
    DevAddNode(_path // ':PAR25_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR26_NAME', 'TEXT', "AngVelocity4", *, _nid);
    DevAddNode(_path // ':PAR26_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR27_NAME', 'TEXT', "InitPhase4", *, _nid);
    DevAddNode(_path // ':PAR27_VAL', 'NUMERIC', 0., *, _nid);


    DevAddNode(_path // ':PAR28_NAME', 'TEXT', "PertIdx1", *, _nid);
    DevAddNode(_path // ':PAR28_VAL', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':PAR29_NAME', 'TEXT', "PertAmp1", *, _nid);
    DevAddNode(_path // ':PAR29_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR30_NAME', 'TEXT', "PertOmega1", *, _nid);
    DevAddNode(_path // ':PAR30_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR31_NAME', 'TEXT', "PertPhi1", *, _nid);
    DevAddNode(_path // ':PAR31_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR32_NAME', 'TEXT', "PertIdx2", *, _nid);
    DevAddNode(_path // ':PAR32_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR33_NAME', 'TEXT', "PertAmp2", *, _nid);
    DevAddNode(_path // ':PAR33_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR34_NAME', 'TEXT', "PertOmega2", *, _nid);
    DevAddNode(_path // ':PAR34_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR35_NAME', 'TEXT', "PertPhi2", *, _nid);
    DevAddNode(_path // ':PAR35_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR36_NAME', 'TEXT', "PertIdx3", *, _nid);
    DevAddNode(_path // ':PAR36_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR37_NAME', 'TEXT', "PertAmp3", *, _nid);
    DevAddNode(_path // ':PAR37_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR38_NAME', 'TEXT', "PertOmega3", *, _nid);
    DevAddNode(_path // ':PAR38_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR39_NAME', 'TEXT', "PertPhi3", *, _nid);
    DevAddNode(_path // ':PAR39_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR40_NAME', 'TEXT', "PertIdx4", *, _nid);
    DevAddNode(_path // ':PAR40_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR41_NAME', 'TEXT', "PertAmp4", *, _nid);
    DevAddNode(_path // ':PAR41_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR42_NAME', 'TEXT', "PertOmega4", *, _nid);
    DevAddNode(_path // ':PAR42_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR43_NAME', 'TEXT', "PertPhi4", *, _nid);
    DevAddNode(_path // ':PAR43_VAL', 'NUMERIC', 0., *, _nid);


    DevAddNode(_path // ':PAR44_NAME', 'TEXT', "ModeControlKp", *, _nid);
    DevAddNode(_path // ':PAR44_VAL', 'NUMERIC', zero([32,32],0.), *, _nid);
    DevAddNode(_path // ':PAR45_NAME', 'TEXT', "ModeControlKpDiagRe", *, _nid);
    DevAddNode(_path // ':PAR45_VAL', 'NUMERIC', zero([32],0.), *, _nid);
    DevAddNode(_path // ':PAR46_NAME', 'TEXT', "ModeControlKpDiagIm", *, _nid);
    DevAddNode(_path // ':PAR46_VAL', 'NUMERIC', zero([32],0.), *, _nid);
    DevAddNode(_path // ':PAR47_NAME', 'TEXT', "ExcludedModeN1", *, _nid);
    DevAddNode(_path // ':PAR47_VAL', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path // ':PAR48_NAME', 'TEXT', "ExcludedModeN2", *, _nid);
    DevAddNode(_path // ':PAR48_VAL', 'NUMERIC', 1000, *, _nid);

    DevAddNode(_path // ':PAR49_NAME', 'TEXT', "ExtrapEnabled", *, _nid);
    DevAddNode(_path // ':PAR49_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PAR50_NAME', 'TEXT', "ExtrapDelta", *, _nid);
    DevAddNode(_path // ':PAR50_VAL', 'NUMERIC', 1E-4, *, _nid);

    DevAddNode(_path // ':PAR51_NAME', 'TEXT', "FieldToCurrentEnabled", *, _nid);
    DevAddNode(_path // ':PAR51_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PAR52_NAME', 'TEXT', "FieldToCurrent", *, _nid);
    DevAddNode(_path // ':PAR52_VAL', 'NUMERIC', zero([64,40.]), *, _nid);
    DevAddNode(_path // ':PAR53_NAME', 'TEXT', "FieldToCurrentModeMapping", *, _nid);
    DevAddNode(_path // ':PAR53_VAL', 'NUMERIC', zero([16],0.), *, _nid);
	
    DevAddNode(_path // ':PAR54_NAME', 'TEXT', "CurrentToVoltageEnabled", *, _nid);
    DevAddNode(_path // ':PAR54_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PAR55_NAME', 'TEXT', "CurrentToVoltageKp", *, _nid);
    DevAddNode(_path // ':PAR55_VAL', 'NUMERIC', zero([32],0.), *, _nid);
    DevAddNode(_path // ':PAR56_NAME', 'TEXT', "CurrentToVoltageKi", *, _nid);
    DevAddNode(_path // ':PAR56_VAL', 'NUMERIC', zero([32],0.), *, _nid);
    DevAddNode(_path // ':PAR57_NAME', 'TEXT', "CurrentToVoltageKd", *, _nid);
    DevAddNode(_path // ':PAR57_VAL', 'NUMERIC', zero([32],0.), *, _nid);
	

    DevAddNode(_path // ':ZERO_START', 'NUMERIC', 0.1, *, _nid);
    DevAddNode(_path // ':ZERO_END', 'NUMERIC', 0.05, *, _nid);
    DevAddNode(_path // ':ZERO', 'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':MAPPING_ID', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':MAPPING', 'NUMERIC', *, *, _nid);


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
    for (_c = 1; _c <=9; _c++)
    {
        DevAddNode(_path // ':MODE_MOD_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
        DevAddNode(_path // ':MODE_PHS_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=32; _c++)
    {
        DevAddNode(_path // ':MODE_MOD_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
        DevAddNode(_path // ':MODE_PHS_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <=9; _c++)
    {
        DevAddNode(_path // ':CURRENT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <=32; _c++)
    {
        DevAddNode(_path // ':CURRENT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 1; _c <=8; _c++)
    {
        DevAddNode(_path // ':USER_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
