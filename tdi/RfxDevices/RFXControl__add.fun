public fun RFXControl__add(in _path, out _nidout)
{
write(*,'RFXControl__add'); 
    DevAddStart(_path, 'RFXControl', 437 + 210, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':IN_CALIB', 'NUMERIC', 
		[1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':OUT_CALIB', 'NUMERIC', 
		[1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':INIT_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG1_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG2_CONTR', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG1_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':TRIG2_TIME', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':CONTR_DURAT', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':SYS_DURAT', 'NUMERIC', 10., *, _nid);
    DevAddNode(_path // ':PRE_TIME', 'NUMERIC', -.5, *, _nid);
    DevAddNode(_path // ':POST_TIME', 'NUMERIC', .5, *, _nid);

    DevAddNode(_path // ':ROUTINE_NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':N_ADC_IN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_DAC_OUT', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_NET_IN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_NET_OUT', 'NUMERIC', 0, *, _nid);
    for (_c = 1; _c <= 9; _c++)
    {
       DevAddNode(_path // ':ADC_IN_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // ':ADC_IN_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 192; _c++)
    {
        DevAddNode(_path // ':ADC_IN_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // ':DAC_OUT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 96; _c++)
    {
        DevAddNode(_path // ':DAC_OUT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // ':NET_IN_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 64; _c++)
    {
        DevAddNode(_path // ':NET_IN_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // ':NET_OUT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 64; _c++)
    {
        DevAddNode(_path // ':NET_OUT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
 
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);

/* Parameters */

/* Individual perturbations */
    DevAddNode(_path // ':PAR1_NAME', 'TEXT', "PertIdx1", *, _nid);
    DevAddNode(_path // ':PAR1_VAL', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':PAR2_NAME', 'TEXT', "PertAmp1", *, _nid);
    DevAddNode(_path // ':PAR2_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR3_NAME', 'TEXT', "PertAngVel1", *, _nid);
    DevAddNode(_path // ':PAR3_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR4_NAME', 'TEXT', "PertPhi1", *, _nid);
    DevAddNode(_path // ':PAR4_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR5_NAME', 'TEXT', "PertIdx2", *, _nid);
    DevAddNode(_path // ':PAR5_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR6_NAME', 'TEXT', "PertAmp2", *, _nid);
    DevAddNode(_path // ':PAR6_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR7_NAME', 'TEXT', "PertAngVel2", *, _nid);
    DevAddNode(_path // ':PAR7_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR8_NAME', 'TEXT', "PertPhi2", *, _nid);
    DevAddNode(_path // ':PAR8_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR9_NAME', 'TEXT', "PertIdx3", *, _nid);
    DevAddNode(_path // ':PAR9_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR10_NAME', 'TEXT', "PertAmp3", *, _nid);
    DevAddNode(_path // ':PAR10_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR11_NAME', 'TEXT', "PertAngVel3", *, _nid);
    DevAddNode(_path // ':PAR11_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR12_NAME', 'TEXT', "PertPhi3", *, _nid);
    DevAddNode(_path // ':PAR12_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR13_NAME', 'TEXT', "PertIdx4", *, _nid);
    DevAddNode(_path // ':PAR13_VAL', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':PAR14_NAME', 'TEXT', "PertAmp4", *, _nid);
    DevAddNode(_path // ':PAR14_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR15_NAME', 'TEXT', "PertAngVel4", *, _nid);
    DevAddNode(_path // ':PAR15_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR16_NAME', 'TEXT', "PertPhi4", *, _nid);
    DevAddNode(_path // ':PAR16_VAL', 'NUMERIC', 0., *, _nid);

/* Waveforms */
    DevAddNode(_path // ':PAR17_NAME', 'TEXT', "WaveX1", *, _nid);
    DevAddNode(_path // ':PAR17_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR18_NAME', 'TEXT', "WaveY1", *, _nid);
    DevAddNode(_path // ':PAR18_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR19_NAME', 'TEXT', "WaveX2", *, _nid);
    DevAddNode(_path // ':PAR19_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR20_NAME', 'TEXT', "WaveY2", *, _nid);
    DevAddNode(_path // ':PAR20_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR21_NAME', 'TEXT', "WaveX3", *, _nid);
    DevAddNode(_path // ':PAR21_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR22_NAME', 'TEXT', "WaveY3", *, _nid);
    DevAddNode(_path // ':PAR22_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR23_NAME', 'TEXT', "WaveX4", *, _nid);
    DevAddNode(_path // ':PAR23_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR24_NAME', 'TEXT', "WaveY4", *, _nid);
    DevAddNode(_path // ':PAR24_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR25_NAME', 'TEXT', "WaveX5", *, _nid);
    DevAddNode(_path // ':PAR25_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR26_NAME', 'TEXT', "WaveY5", *, _nid);
    DevAddNode(_path // ':PAR26_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR27_NAME', 'TEXT', "WaveX6", *, _nid);
    DevAddNode(_path // ':PAR27_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR28_NAME', 'TEXT', "WaveY6", *, _nid);
    DevAddNode(_path // ':PAR28_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR29_NAME', 'TEXT', "WaveX7", *, _nid);
    DevAddNode(_path // ':PAR29_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR30_NAME', 'TEXT', "WaveY7", *, _nid);
    DevAddNode(_path // ':PAR30_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR31_NAME', 'TEXT', "WaveX8", *, _nid);
    DevAddNode(_path // ':PAR31_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR32_NAME', 'TEXT', "WaveY8", *, _nid);
    DevAddNode(_path // ':PAR32_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR33_NAME', 'TEXT', "WaveX9", *, _nid);
    DevAddNode(_path // ':PAR33_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR34_NAME', 'TEXT', "WaveY9", *, _nid);
    DevAddNode(_path // ':PAR34_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR35_NAME', 'TEXT', "WaveX10", *, _nid);
    DevAddNode(_path // ':PAR35_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR36_NAME', 'TEXT', "WaveY10", *, _nid);
    DevAddNode(_path // ':PAR36_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR37_NAME', 'TEXT', "WaveX11", *, _nid);
    DevAddNode(_path // ':PAR37_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR38_NAME', 'TEXT', "WaveY11", *, _nid);
    DevAddNode(_path // ':PAR38_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR39_NAME', 'TEXT', "WaveX12", *, _nid);
    DevAddNode(_path // ':PAR39_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR40_NAME', 'TEXT', "WaveY12", *, _nid);
    DevAddNode(_path // ':PAR40_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR41_NAME', 'TEXT', "WaveX13", *, _nid);
    DevAddNode(_path // ':PAR41_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR42_NAME', 'TEXT', "WaveY13", *, _nid);
    DevAddNode(_path // ':PAR42_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR43_NAME', 'TEXT', "WaveX14", *, _nid);
    DevAddNode(_path // ':PAR43_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR44_NAME', 'TEXT', "WaveY14", *, _nid);
    DevAddNode(_path // ':PAR44_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR45_NAME', 'TEXT', "WaveX15", *, _nid);
    DevAddNode(_path // ':PAR45_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR46_NAME', 'TEXT', "WaveY15", *, _nid);
    DevAddNode(_path // ':PAR46_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR47_NAME', 'TEXT', "WaveX16", *, _nid);
    DevAddNode(_path // ':PAR47_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR48_NAME', 'TEXT', "WaveY16", *, _nid);
    DevAddNode(_path // ':PAR48_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR49_NAME', 'TEXT', "WaveX16", *, _nid);
    DevAddNode(_path // ':PAR49_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR50_NAME', 'TEXT', "WaveY17", *, _nid);
    DevAddNode(_path // ':PAR50_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR51_NAME', 'TEXT', "WaveX17", *, _nid);
    DevAddNode(_path // ':PAR51_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR52_NAME', 'TEXT', "WaveY18", *, _nid);
    DevAddNode(_path // ':PAR52_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR53_NAME', 'TEXT', "WaveX18", *, _nid);
    DevAddNode(_path // ':PAR53_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR54_NAME', 'TEXT', "WaveY18", *, _nid);
    DevAddNode(_path // ':PAR54_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR55_NAME', 'TEXT', "WaveX19", *, _nid);
    DevAddNode(_path // ':PAR55_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR56_NAME', 'TEXT', "WaveY19", *, _nid);
    DevAddNode(_path // ':PAR56_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR57_NAME', 'TEXT', "WaveX20", *, _nid);
    DevAddNode(_path // ':PAR57_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR58_NAME', 'TEXT', "WaveY21", *, _nid);
    DevAddNode(_path // ':PAR58_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR59_NAME', 'TEXT', "WaveX22", *, _nid);
    DevAddNode(_path // ':PAR59_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR60_NAME', 'TEXT', "WaveY22", *, _nid);
    DevAddNode(_path // ':PAR60_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR61_NAME', 'TEXT', "WaveX23", *, _nid);
    DevAddNode(_path // ':PAR61_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR62_NAME', 'TEXT', "WaveY23", *, _nid);
    DevAddNode(_path // ':PAR62_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR63_NAME', 'TEXT', "WaveX24", *, _nid);
    DevAddNode(_path // ':PAR63_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR64_NAME', 'TEXT', "WaveY24", *, _nid);
    DevAddNode(_path // ':PAR64_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR65_NAME', 'TEXT', "WaveX25", *, _nid);
    DevAddNode(_path // ':PAR65_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR66_NAME', 'TEXT', "WaveY25", *, _nid);
    DevAddNode(_path // ':PAR66_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR67_NAME', 'TEXT', "WaveX26", *, _nid);
    DevAddNode(_path // ':PAR67_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR68_NAME', 'TEXT', "WaveY26", *, _nid);
    DevAddNode(_path // ':PAR68_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR69_NAME', 'TEXT', "WaveX27", *, _nid);
    DevAddNode(_path // ':PAR69_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR70_NAME', 'TEXT', "WaveY27", *, _nid);
    DevAddNode(_path // ':PAR70_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR71_NAME', 'TEXT', "WaveX28", *, _nid);
    DevAddNode(_path // ':PAR71_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR72_NAME', 'TEXT', "WaveY28", *, _nid);
    DevAddNode(_path // ':PAR72_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR73_NAME', 'TEXT', "WaveX29", *, _nid);
    DevAddNode(_path // ':PAR73_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR74_NAME', 'TEXT', "WaveY29", *, _nid);
    DevAddNode(_path // ':PAR74_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR75_NAME', 'TEXT', "WaveX30", *, _nid);
    DevAddNode(_path // ':PAR75_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR76_NAME', 'TEXT', "WaveY30", *, _nid);
    DevAddNode(_path // ':PAR76_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR77_NAME', 'TEXT', "WaveX31", *, _nid);
    DevAddNode(_path // ':PAR77_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR78_NAME', 'TEXT', "WaveY31", *, _nid);
    DevAddNode(_path // ':PAR78_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR79_NAME', 'TEXT', "WaveX32", *, _nid);
    DevAddNode(_path // ':PAR79_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR80_NAME', 'TEXT', "WaveY32", *, _nid);
    DevAddNode(_path // ':PAR80_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	


/* Mode generation */
    DevAddNode(_path // ':PAR81_NAME', 'TEXT', "ModePertAmp1", *, _nid);
    DevAddNode(_path // ':PAR81_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR82_NAME', 'TEXT', "ModePertAmpAngVel1", *, _nid);
    DevAddNode(_path // ':PAR82_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR83_NAME', 'TEXT', "ModePertAmpPhase1", *, _nid);
    DevAddNode(_path // ':PAR83_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR84_NAME', 'TEXT', "ModePertAngVel1", *, _nid);
    DevAddNode(_path // ':PAR84_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR85_NAME', 'TEXT', "ModePertInitPhase1", *, _nid);
    DevAddNode(_path // ':PAR85_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR86_NAME', 'TEXT', "ModePertAmp2", *, _nid);
    DevAddNode(_path // ':PAR86_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR87_NAME', 'TEXT', "ModePertAmpAngVel2", *, _nid);
    DevAddNode(_path // ':PAR87_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR88_NAME', 'TEXT', "ModePertAmpPhase2", *, _nid);
    DevAddNode(_path // ':PAR88_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR89_NAME', 'TEXT', "ModePertAngVel2", *, _nid);
    DevAddNode(_path // ':PAR89_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR90_NAME', 'TEXT', "ModePertInitPhase2", *, _nid);
    DevAddNode(_path // ':PAR90_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR91_NAME', 'TEXT', "ModePertAmp3", *, _nid);
    DevAddNode(_path // ':PAR91_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR92_NAME', 'TEXT', "ModePertAmpAngVel3", *, _nid);
    DevAddNode(_path // ':PAR92_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR93_NAME', 'TEXT', "ModePertAmpPhase3", *, _nid);
    DevAddNode(_path // ':PAR93_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR94_NAME', 'TEXT', "ModePertAngVel3", *, _nid);
    DevAddNode(_path // ':PAR94_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR95_NAME', 'TEXT', "ModePertInitPhase3", *, _nid);
    DevAddNode(_path // ':PAR95_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // ':PAR96_NAME', 'TEXT', "ModePertAmp4", *, _nid);
    DevAddNode(_path // ':PAR96_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR97_NAME', 'TEXT', "ModePertAmpAngVel4", *, _nid);
    DevAddNode(_path // ':PAR97_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR98_NAME', 'TEXT', "ModePertAmpPhase4", *, _nid);
    DevAddNode(_path // ':PAR98_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR99_NAME', 'TEXT', "ModePertAngVel4", *, _nid);
    DevAddNode(_path // ':PAR99_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR100_NAME', 'TEXT', "ModePertInitPhase4", *, _nid);
    DevAddNode(_path // ':PAR100_VAL', 'NUMERIC', 0., *, _nid);

/* Mode Control */

    DevAddNode(_path // ':PAR101_NAME', 'TEXT', "K1", *, _nid);
    DevAddNode(_path // ':PAR101_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR102_NAME', 'TEXT', "K2", *, _nid);
    DevAddNode(_path // ':PAR102_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR103_NAME', 'TEXT', "K3", *, _nid);
    DevAddNode(_path // ':PAR103_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR104_NAME', 'TEXT', "K4", *, _nid);
    DevAddNode(_path // ':PAR104_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR105_NAME', 'TEXT', "K5", *, _nid);
    DevAddNode(_path // ':PAR105_VAL', 'NUMERIC', 0., *, _nid);

    DevAddEnd();
}
