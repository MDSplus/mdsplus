public fun RFXControl__add(in _path, out _nidout)
{
write(*,'RFXControl__add'); 
    DevAddStart(_path, 'RFXControl', 831 + 341, _nidout);
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
    DevAddNode(_path // ':TRIG1_TIME', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':TRIG2_TIME', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':CONTR_DURAT', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':SYS_DURAT', 'NUMERIC', 10., *, _nid);
    DevAddNode(_path // ':PRE_TIME', 'NUMERIC', -.5, *, _nid);
    DevAddNode(_path // ':POST_TIME', 'NUMERIC', .5, *, _nid);
    DevAddNode(_path // ':ZERO_START', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':ZERO_END', 'NUMERIC', 0.5, *, _nid);
    DevAddNode(_path // ':ZERO', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':MAPPING_ID', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':MAPPING', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':RAMP_SLOPE', 'NUMERIC', 10., *, _nid);
    DevAddNode(_path // ':RAMP_TRIGGER', 'NUMERIC', 2, *, _nid);
    DevAddNode(_path // ':FEEDFORW', 'TEXT', 'DISABLED', *, _nid);

    DevAddNode(_path // ':ROUTINE_NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':N_ADC_IN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_DAC_OUT', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_NET_IN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_NET_OUT', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_MODES', 'NUMERIC', 0, *, _nid);
/* Signals */
    DevAddNode(_path// '.SIGNALS', 'STRUCTURE', *, *, _nid);
    for (_c = 1; _c <= 9; _c++)
    {
       DevAddNode(_path // '.SIGNALS:ADC_IN_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // '.SIGNALS:ADC_IN_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 192; _c++)
    {
        DevAddNode(_path // '.SIGNALS:ADC_IN_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // '.SIGNALS:DAC_OUT_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 96; _c++)
    {
        DevAddNode(_path // '.SIGNALS:DAC_OUT_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 128; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 1; _c <= 9; _c++)
    {
       DevAddNode(_path // '.SIGNALS:MODE_MOD_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
       DevAddNode(_path // '.SIGNALS:MODE_PHS_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // '.SIGNALS:MODE_MOD_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
        DevAddNode(_path // '.SIGNALS:MODE_PHS_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 192; _c++)
    {
        DevAddNode(_path // '.SIGNALS:MODE_MOD_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
        DevAddNode(_path // '.SIGNALS:MODE_PHS_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}

 
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);


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
    DevAddNode(_path // ':PAR49_NAME', 'TEXT', "WaveX17", *, _nid);
    DevAddNode(_path // ':PAR49_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR50_NAME', 'TEXT', "WaveY17", *, _nid);
    DevAddNode(_path // ':PAR50_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR51_NAME', 'TEXT', "WaveX18", *, _nid);
    DevAddNode(_path // ':PAR51_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR52_NAME', 'TEXT', "WaveY18", *, _nid);
    DevAddNode(_path // ':PAR52_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR53_NAME', 'TEXT', "WaveX19", *, _nid);
    DevAddNode(_path // ':PAR53_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR54_NAME', 'TEXT', "WaveY19", *, _nid);
    DevAddNode(_path // ':PAR54_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR55_NAME', 'TEXT', "WaveX20", *, _nid);
    DevAddNode(_path // ':PAR55_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR56_NAME', 'TEXT', "WaveY20", *, _nid);
    DevAddNode(_path // ':PAR56_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // ':PAR57_NAME', 'TEXT', "WaveX21", *, _nid);
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
	DevAddNode(_path // ':PAR81_NAME', 'TEXT', "WaveX33", *, _nid);
    DevAddNode(_path // ':PAR81_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR82_NAME', 'TEXT', "WaveY33", *, _nid);
    DevAddNode(_path // ':PAR82_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR83_NAME', 'TEXT', "WaveX34", *, _nid);
    DevAddNode(_path // ':PAR83_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR84_NAME', 'TEXT', "WaveY34", *, _nid);
    DevAddNode(_path // ':PAR84_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR85_NAME', 'TEXT', "WaveX35", *, _nid);
    DevAddNode(_path // ':PAR85_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR86_NAME', 'TEXT', "WaveY35", *, _nid);
    DevAddNode(_path // ':PAR86_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR87_NAME', 'TEXT', "WaveX36", *, _nid);
    DevAddNode(_path // ':PAR87_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR88_NAME', 'TEXT', "WaveY36", *, _nid);
    DevAddNode(_path // ':PAR88_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR89_NAME', 'TEXT', "WaveX37", *, _nid);
    DevAddNode(_path // ':PAR89_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR90_NAME', 'TEXT', "WaveY37", *, _nid);
    DevAddNode(_path // ':PAR90_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR91_NAME', 'TEXT', "WaveX38", *, _nid);
    DevAddNode(_path // ':PAR91_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR92_NAME', 'TEXT', "WaveY38", *, _nid);
    DevAddNode(_path // ':PAR92_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR93_NAME', 'TEXT', "WaveX39", *, _nid);
    DevAddNode(_path // ':PAR93_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR94_NAME', 'TEXT', "WaveY39", *, _nid);
    DevAddNode(_path // ':PAR94_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR95_NAME', 'TEXT', "WaveX40", *, _nid);
    DevAddNode(_path // ':PAR95_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR96_NAME', 'TEXT', "WaveY40", *, _nid);
    DevAddNode(_path // ':PAR96_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);


/* Rotating perturmation */
    DevAddNode(_path // ':PAR97_NAME', 'TEXT', "RotPertN1", *, _nid);
    DevAddNode(_path // ':PAR97_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR98_NAME', 'TEXT', "RotPertM1", *, _nid);
    DevAddNode(_path // ':PAR98_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR99_NAME', 'TEXT', "RotPertAmplitude1", *, _nid);
    DevAddNode(_path // ':PAR99_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR100_NAME', 'TEXT', "RotPertAmpAngVel1", *, _nid);
    DevAddNode(_path // ':PAR100_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR101_NAME', 'TEXT', "RotPertAmpPhase1", *, _nid);
    DevAddNode(_path // ':PAR101_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR102_NAME', 'TEXT', "RotPertAngVel1", *, _nid);
    DevAddNode(_path // ':PAR102_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR103_NAME', 'TEXT', "RotPertPhase1", *, _nid);
    DevAddNode(_path // ':PAR103_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // ':PAR104_NAME', 'TEXT', "RotPertN2", *, _nid);
    DevAddNode(_path // ':PAR104_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR105_NAME', 'TEXT', "RotPertM2", *, _nid);
    DevAddNode(_path // ':PAR105_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR106_NAME', 'TEXT', "RotPertAmplitude2", *, _nid);
    DevAddNode(_path // ':PAR106_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR107_NAME', 'TEXT', "RotPertAmpAngVel2", *, _nid);
    DevAddNode(_path // ':PAR107_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR108_NAME', 'TEXT', "RotPertAmpPhase2", *, _nid);
    DevAddNode(_path // ':PAR108_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR109_NAME', 'TEXT', "RotPertAngVel2", *, _nid);
    DevAddNode(_path // ':PAR109_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR110_NAME', 'TEXT', "RotPertPhase2", *, _nid);
    DevAddNode(_path // ':PAR110_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // ':PAR111_NAME', 'TEXT', "RotPertN3", *, _nid);
    DevAddNode(_path // ':PAR111_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR112_NAME', 'TEXT', "RotPertM3", *, _nid);
    DevAddNode(_path // ':PAR112_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR113_NAME', 'TEXT', "RotPertAmplitude3", *, _nid);
    DevAddNode(_path // ':PAR113_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR114_NAME', 'TEXT', "RotPertAmpAngVel3", *, _nid);
    DevAddNode(_path // ':PAR114_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR115_NAME', 'TEXT', "RotPertAmpPhase3", *, _nid);
    DevAddNode(_path // ':PAR115_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR116_NAME', 'TEXT', "RotPertAngVel3", *, _nid);
    DevAddNode(_path // ':PAR116_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR117_NAME', 'TEXT', "RotPertPhase3", *, _nid);
    DevAddNode(_path // ':PAR117_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // ':PAR118_NAME', 'TEXT', "RotPertN4", *, _nid);
    DevAddNode(_path // ':PAR118_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR119_NAME', 'TEXT', "RotPertM4", *, _nid);
    DevAddNode(_path // ':PAR119_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR120_NAME', 'TEXT', "RotPertAmplitude4", *, _nid);
    DevAddNode(_path // ':PAR120_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR121_NAME', 'TEXT', "RotPertAmpAngVel4", *, _nid);
    DevAddNode(_path // ':PAR121_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR122_NAME', 'TEXT', "RotPertAmpPhase4", *, _nid);
    DevAddNode(_path // ':PAR122_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR123_NAME', 'TEXT', "RotPertAngVel4", *, _nid);
    DevAddNode(_path // ':PAR123_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR124_NAME', 'TEXT', "RotPertPhase4", *, _nid);
    DevAddNode(_path // ':PAR124_VAL', 'NUMERIC', 0., *, _nid);

 /* Mode Control */

    DevAddNode(_path // ':PAR125_NAME', 'TEXT', "ModeControlKp", *, _nid);
    DevAddNode(_path // ':PAR125_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR126_NAME', 'TEXT', "ModeControlKi", *, _nid);
    DevAddNode(_path // ':PAR126_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR127_NAME', 'TEXT', "ModeControlKd", *, _nid);
    DevAddNode(_path // ':PAR127_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR128_NAME', 'TEXT', "K4", *, _nid);
    DevAddNode(_path // ':PAR128_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':PAR129_NAME', 'TEXT', "K5", *, _nid);
    DevAddNode(_path // ':PAR129_VAL', 'NUMERIC', 0., *, _nid);

/* Simulink Parameters */
    DevAddNode(_path // ':PAR130_NAME', 'TEXT', "SimulinkAxiFeedforw", *, _nid);
    DevAddNode(_path // ':PAR130_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR131_NAME', 'TEXT', "SimulinkAxiFeedback", *, _nid);
    DevAddNode(_path // ':PAR131_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR132_NAME', 'TEXT', "SimulinkPar3", *, _nid);
    DevAddNode(_path // ':PAR132_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR133_NAME', 'TEXT', "SimulinkPar4", *, _nid);
    DevAddNode(_path // ':PAR133_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR134_NAME', 'TEXT', "SimulinkPar5", *, _nid);
    DevAddNode(_path // ':PAR134_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR135_NAME', 'TEXT', "SimulinkPar6", *, _nid);
    DevAddNode(_path // ':PAR135_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR136_NAME', 'TEXT', "SimulinkPar7", *, _nid);
    DevAddNode(_path // ':PAR136_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // ':PAR137_NAME', 'TEXT', "SimulinkPar8", *, _nid);
    DevAddNode(_path // ':PAR137_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);

/*Simuling Waveforms */
	DevAddNode(_path // ':PAR138_NAME', 'TEXT', "SimulinkWaveX1", *, _nid);
    DevAddNode(_path // ':PAR138_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR139_NAME', 'TEXT', "SimulinkWaveY1", *, _nid);
    DevAddNode(_path // ':PAR139_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR140_NAME', 'TEXT', "SimulinkWaveX2", *, _nid);
    DevAddNode(_path // ':PAR140_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR141_NAME', 'TEXT', "SimulinkWaveY2", *, _nid);
    DevAddNode(_path // ':PAR141_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR142_NAME', 'TEXT', "SimulinkWaveX3", *, _nid);
    DevAddNode(_path // ':PAR142_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR143_NAME', 'TEXT', "SimulinkWaveY3", *, _nid);
    DevAddNode(_path // ':PAR143_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR144_NAME', 'TEXT', "SimulinkWaveX4", *, _nid);
    DevAddNode(_path // ':PAR144_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR145_NAME', 'TEXT', "SimulinkWaveY4", *, _nid);
    DevAddNode(_path // ':PAR145_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR146_NAME', 'TEXT', "SimulinkWaveX5", *, _nid);
    DevAddNode(_path // ':PAR146_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR147_NAME', 'TEXT', "SimulinkWaveY5", *, _nid);
    DevAddNode(_path // ':PAR147_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR148_NAME', 'TEXT', "SimulinkWaveX6", *, _nid);
    DevAddNode(_path // ':PAR148_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR149_NAME', 'TEXT', "SimulinkWaveY6", *, _nid);
    DevAddNode(_path // ':PAR149_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR150_NAME', 'TEXT', "SimulinkWaveX7", *, _nid);
    DevAddNode(_path // ':PAR150_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR151_NAME', 'TEXT', "SimulinkWaveY7", *, _nid);
    DevAddNode(_path // ':PAR151_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR152_NAME', 'TEXT', "SimulinkWaveX8", *, _nid);
    DevAddNode(_path // ':PAR152_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR153_NAME', 'TEXT', "SimulinkWaveY8", *, _nid);
    DevAddNode(_path // ':PAR153_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR154_NAME', 'TEXT', "SimulinkWaveX9", *, _nid);
    DevAddNode(_path // ':PAR154_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR155_NAME', 'TEXT', "SimulinkWaveY9", *, _nid);
    DevAddNode(_path // ':PAR155_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR156_NAME', 'TEXT', "SimulinkWaveX10", *, _nid);
    DevAddNode(_path // ':PAR156_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR157_NAME', 'TEXT', "SimulinkWaveY10", *, _nid);
    DevAddNode(_path // ':PAR157_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR158_NAME', 'TEXT', "SimulinkWaveX11", *, _nid);
    DevAddNode(_path // ':PAR158_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR159_NAME', 'TEXT', "SimulinkWaveY11", *, _nid);
    DevAddNode(_path // ':PAR159_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR160_NAME', 'TEXT', "SimulinkWaveX12", *, _nid);
    DevAddNode(_path // ':PAR160_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR161_NAME', 'TEXT', "SimulinkWaveY12", *, _nid);
    DevAddNode(_path // ':PAR161_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR162_NAME', 'TEXT', "SimulinkWaveX13", *, _nid);
    DevAddNode(_path // ':PAR162_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR163_NAME', 'TEXT', "SimulinkWaveY13", *, _nid);
    DevAddNode(_path // ':PAR163_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR164_NAME', 'TEXT', "SimulinkWaveX14", *, _nid);
    DevAddNode(_path // ':PAR164_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR165_NAME', 'TEXT', "SimulinkWaveY14", *, _nid);
    DevAddNode(_path // ':PAR165_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR166_NAME', 'TEXT', "SimulinkWaveX15", *, _nid);
    DevAddNode(_path // ':PAR166_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR167_NAME', 'TEXT', "SimulinkWaveY15", *, _nid);
    DevAddNode(_path // ':PAR167_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // ':PAR168_NAME', 'TEXT', "SimulinkWaveX16", *, _nid);
    DevAddNode(_path // ':PAR168_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // ':PAR169_NAME', 'TEXT', "SimulinkWaveY16", *, _nid);
    DevAddNode(_path // ':PAR169_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);

/* Simulink Algorithm Selector */
    DevAddNode(_path // ':PAR170_NAME', 'TEXT', "SimulinkSelector", *, _nid);
    DevAddNode(_path // ':PAR170_VAL', 'NUMERIC', [0,0,0,0,0,0,0,0], *, _nid);


    DevAddEnd();
}
