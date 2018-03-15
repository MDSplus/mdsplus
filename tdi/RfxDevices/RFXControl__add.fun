public fun RFXControl__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXControl', 1797, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FREQUENCY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':IN_CALIB', 'ANY', 
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
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,
		 1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0,1.,0], *, _nid);
    DevAddNode(_path // ':OUT_CALIB', 'ANY', 
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
    DevAddNode(_path // ':SPARE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':N_MODES', 'NUMERIC', 0, *, _nid);

/* Simulink model */
	DevAddNode(_path // ':MODEL_1', 'NUMERIC', *, *, _nid);
	DevAddNode(_path // ':MODEL_2', 'NUMERIC', *, *, _nid);

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
        DevAddNode(_path // '.SIGNALS:SPARE_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // '.SIGNALS:SPARE_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 128; _c++)
    {
        DevAddNode(_path // '.SIGNALS:SPARE_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
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

 
     DevAddNode(_path// '.PARAMETERS', 'STRUCTURE', *, *, _nid);

/* Individual perturbations */
    DevAddNode(_path // '.PARAMETERS:PAR1_NAME', 'TEXT', "PertIdx1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR1_VAL', 'NUMERIC', zero(192,0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR2_NAME', 'TEXT', "PertAmp1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR2_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR3_NAME', 'TEXT', "PertAngVel1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR3_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR4_NAME', 'TEXT', "PertPhi1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR4_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR5_NAME', 'TEXT', "PertIdx2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR5_VAL', 'NUMERIC', zero(192,0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR6_NAME', 'TEXT', "PertAmp2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR6_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR7_NAME', 'TEXT', "PertAngVel2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR7_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR8_NAME', 'TEXT', "PertPhi2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR8_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR9_NAME', 'TEXT', "PertIdx3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR9_VAL', 'NUMERIC', zero(192,0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR10_NAME', 'TEXT', "PertAmp3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR10_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR11_NAME', 'TEXT', "PertAngVel3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR11_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR12_NAME', 'TEXT', "PertPhi3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR12_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR13_NAME', 'TEXT', "PertIdx4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR13_VAL', 'NUMERIC', zero(192,0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR14_NAME', 'TEXT', "PertAmp4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR14_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR15_NAME', 'TEXT', "PertAngVel4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR15_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR16_NAME', 'TEXT', "PertPhi4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR16_VAL', 'NUMERIC', 0., *, _nid);

/* Waveforms */
    DevAddNode(_path // '.PARAMETERS:PAR17_NAME', 'TEXT', "WaveX1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR17_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR18_NAME', 'TEXT', "WaveY1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR18_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR19_NAME', 'TEXT', "WaveX2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR19_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR20_NAME', 'TEXT', "WaveY2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR20_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR21_NAME', 'TEXT', "WaveX3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR21_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR22_NAME', 'TEXT', "WaveY3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR22_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR23_NAME', 'TEXT', "WaveX4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR23_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR24_NAME', 'TEXT', "WaveY4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR24_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR25_NAME', 'TEXT', "WaveX5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR25_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR26_NAME', 'TEXT', "WaveY5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR26_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR27_NAME', 'TEXT', "WaveX6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR27_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR28_NAME', 'TEXT', "WaveY6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR28_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR29_NAME', 'TEXT', "WaveX7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR29_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR30_NAME', 'TEXT', "WaveY7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR30_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR31_NAME', 'TEXT', "WaveX8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR31_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR32_NAME', 'TEXT', "WaveY8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR32_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR33_NAME', 'TEXT', "WaveX9", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR33_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR34_NAME', 'TEXT', "WaveY9", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR34_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR35_NAME', 'TEXT', "WaveX10", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR35_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR36_NAME', 'TEXT', "WaveY10", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR36_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR37_NAME', 'TEXT', "WaveX11", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR37_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR38_NAME', 'TEXT', "WaveY11", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR38_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR39_NAME', 'TEXT', "WaveX12", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR39_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR40_NAME', 'TEXT', "WaveY12", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR40_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR41_NAME', 'TEXT', "WaveX13", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR41_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR42_NAME', 'TEXT', "WaveY13", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR42_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR43_NAME', 'TEXT', "WaveX14", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR43_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR44_NAME', 'TEXT', "WaveY14", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR44_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR45_NAME', 'TEXT', "WaveX15", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR45_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR46_NAME', 'TEXT', "WaveY15", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR46_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR47_NAME', 'TEXT', "WaveX16", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR47_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR48_NAME', 'TEXT', "WaveY16", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR48_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR49_NAME', 'TEXT', "WaveX17", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR49_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR50_NAME', 'TEXT', "WaveY17", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR50_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR51_NAME', 'TEXT', "WaveX18", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR51_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR52_NAME', 'TEXT', "WaveY18", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR52_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR53_NAME', 'TEXT', "WaveX19", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR53_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR54_NAME', 'TEXT', "WaveY19", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR54_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR55_NAME', 'TEXT', "WaveX20", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR55_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR56_NAME', 'TEXT', "WaveY20", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR56_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR57_NAME', 'TEXT', "WaveX21", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR57_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR58_NAME', 'TEXT', "WaveY21", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR58_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR59_NAME', 'TEXT', "WaveX22", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR59_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR60_NAME', 'TEXT', "WaveY22", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR60_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR61_NAME', 'TEXT', "WaveX23", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR61_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR62_NAME', 'TEXT', "WaveY23", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR62_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR63_NAME', 'TEXT', "WaveX24", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR63_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR64_NAME', 'TEXT', "WaveY24", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR64_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR65_NAME', 'TEXT', "WaveX25", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR65_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR66_NAME', 'TEXT', "WaveY25", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR66_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR67_NAME', 'TEXT', "WaveX26", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR67_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR68_NAME', 'TEXT', "WaveY26", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR68_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR69_NAME', 'TEXT', "WaveX27", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR69_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR70_NAME', 'TEXT', "WaveY27", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR70_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR71_NAME', 'TEXT', "WaveX28", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR71_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR72_NAME', 'TEXT', "WaveY28", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR72_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR73_NAME', 'TEXT', "WaveX29", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR73_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR74_NAME', 'TEXT', "WaveY29", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR74_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR75_NAME', 'TEXT', "WaveX30", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR75_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR76_NAME', 'TEXT', "WaveY30", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR76_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR77_NAME', 'TEXT', "WaveX31", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR77_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR78_NAME', 'TEXT', "WaveY31", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR78_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR79_NAME', 'TEXT', "WaveX32", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR79_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR80_NAME', 'TEXT', "WaveY32", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR80_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR81_NAME', 'TEXT', "WaveX33", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR81_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR82_NAME', 'TEXT', "WaveY33", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR82_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR83_NAME', 'TEXT', "WaveX34", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR83_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR84_NAME', 'TEXT', "WaveY34", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR84_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR85_NAME', 'TEXT', "WaveX35", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR85_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR86_NAME', 'TEXT', "WaveY35", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR86_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR87_NAME', 'TEXT', "WaveX36", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR87_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR88_NAME', 'TEXT', "WaveY36", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR88_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR89_NAME', 'TEXT', "WaveX37", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR89_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR90_NAME', 'TEXT', "WaveY37", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR90_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR91_NAME', 'TEXT', "WaveX38", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR91_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR92_NAME', 'TEXT', "WaveY38", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR92_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR93_NAME', 'TEXT', "WaveX39", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR93_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR94_NAME', 'TEXT', "WaveY39", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR94_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR95_NAME', 'TEXT', "WaveX40", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR95_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR96_NAME', 'TEXT', "WaveY40", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR96_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);


/* Rotating perturmation */
    DevAddNode(_path // '.PARAMETERS:PAR97_NAME', 'TEXT', "RotPertN1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR97_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR98_NAME', 'TEXT', "RotPertM1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR98_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR99_NAME', 'TEXT', "RotPertAmplitude1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR99_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR100_NAME', 'TEXT', "RotPertStart1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR100_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR101_NAME', 'TEXT', "RotPertEnd1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR101_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR102_NAME', 'TEXT', "RotPertAngVel1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR102_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR103_NAME', 'TEXT', "RotPertPhase1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR103_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // '.PARAMETERS:PAR104_NAME', 'TEXT', "RotPertN2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR104_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR105_NAME', 'TEXT', "RotPertM2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR105_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR106_NAME', 'TEXT', "RotPertAmplitude2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR106_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR107_NAME', 'TEXT', "RotPertStart2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR107_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR108_NAME', 'TEXT', "RotPertEnd2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR108_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR109_NAME', 'TEXT', "RotPertAngVel2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR109_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR110_NAME', 'TEXT', "RotPertPhase2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR110_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // '.PARAMETERS:PAR111_NAME', 'TEXT', "RotPertN3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR111_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR112_NAME', 'TEXT', "RotPertM3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR112_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR113_NAME', 'TEXT', "RotPertAmplitude3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR113_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR114_NAME', 'TEXT', "RotPertStart3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR114_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR115_NAME', 'TEXT', "RotPertEnd3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR115_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR116_NAME', 'TEXT', "RotPertAngVel3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR116_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR117_NAME', 'TEXT', "RotPertPhase3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR117_VAL', 'NUMERIC', 0., *, _nid);

 
    DevAddNode(_path // '.PARAMETERS:PAR118_NAME', 'TEXT', "RotPertN4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR118_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR119_NAME', 'TEXT', "RotPertM4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR119_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR120_NAME', 'TEXT', "RotPertAmplitude4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR120_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR121_NAME', 'TEXT', "RotPertStart4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR121_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR122_NAME', 'TEXT', "RotPertEnd4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR122_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR123_NAME', 'TEXT', "RotPertAngVel4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR123_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR124_NAME', 'TEXT', "RotPertPhase4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR124_VAL', 'NUMERIC', 0., *, _nid);

/* Virtual Shell PID */

    DevAddNode(_path // '.PARAMETERS:PAR125_NAME', 'TEXT', "VirtualShellKp", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR125_VAL', 'NUMERIC', zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR126_NAME', 'TEXT', "VirtualShellKi", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR126_VAL', 'NUMERIC', zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR127_NAME', 'TEXT', "VirtualShellKd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR127_VAL', 'NUMERIC', zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR128_NAME', 'TEXT', "VirtualShellExcluded", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR128_VAL', 'NUMERIC', zero(192, 0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR129_NAME', 'TEXT', "VirtualShellDerivativeCutOff", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR129_VAL', 'NUMERIC', 100., *, _nid);

/* Simulink Parameters */
    DevAddNode(_path // '.PARAMETERS:PAR130_NAME', 'TEXT', "SimulinkAxiFeedforw", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR130_VAL', 'NUMERIC', [0.,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR131_NAME', 'TEXT', "SimulinkAxiFeedback", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR131_VAL', 'NUMERIC', [0.,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR132_NAME', 'TEXT', "SimulinkAmpTurnCompDistr", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR132_VAL', 'NUMERIC', [0.,0,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR133_NAME', 'TEXT', "SimulinkBvGenDistr", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR133_VAL', 'NUMERIC', [0.,0,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR134_NAME', 'TEXT', "SimulinkCurrentControl", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR134_VAL', 'NUMERIC', [0.,0,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR135_NAME', 'TEXT', "SimulinkRfpCurrentControl", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR135_VAL', 'NUMERIC', [0.,0,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR136_NAME', 'TEXT', "SimulinkPar7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR136_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR137_NAME', 'TEXT', "SimulinkPar8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR137_VAL', 'NUMERIC', [0.,0,0,0,0], *, _nid);

/*Simuling Waveforms */
	DevAddNode(_path // '.PARAMETERS:PAR138_NAME', 'TEXT', "SimulinkWaveX1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR138_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR139_NAME', 'TEXT', "SimulinkWaveY1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR139_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR140_NAME', 'TEXT', "SimulinkWaveX2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR140_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR141_NAME', 'TEXT', "SimulinkWaveY2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR141_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR142_NAME', 'TEXT', "SimulinkWaveX3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR142_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR143_NAME', 'TEXT', "SimulinkWaveY3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR143_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR144_NAME', 'TEXT', "SimulinkWaveX4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR144_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR145_NAME', 'TEXT', "SimulinkWaveY4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR145_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR146_NAME', 'TEXT', "SimulinkWaveX5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR146_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR147_NAME', 'TEXT', "SimulinkWaveY5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR147_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR148_NAME', 'TEXT', "SimulinkWaveX6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR148_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR149_NAME', 'TEXT', "SimulinkWaveY6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR149_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR150_NAME', 'TEXT', "SimulinkWaveX7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR150_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR151_NAME', 'TEXT', "SimulinkWaveY7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR151_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR152_NAME', 'TEXT', "SimulinkWaveX8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR152_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR153_NAME', 'TEXT', "SimulinkWaveY8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR153_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR154_NAME', 'TEXT', "SimulinkWaveX9", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR154_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR155_NAME', 'TEXT', "SimulinkWaveY9", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR155_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR156_NAME', 'TEXT', "SimulinkWaveX10", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR156_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR157_NAME', 'TEXT', "SimulinkWaveY10", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR157_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR158_NAME', 'TEXT', "SimulinkWaveX11", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR158_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR159_NAME', 'TEXT', "SimulinkWaveY11", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR159_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR160_NAME', 'TEXT', "SimulinkWaveX12", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR160_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR161_NAME', 'TEXT', "SimulinkWaveY12", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR161_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR162_NAME', 'TEXT', "SimulinkWaveX13", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR162_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR163_NAME', 'TEXT', "SimulinkWaveY13", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR163_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR164_NAME', 'TEXT', "SimulinkWaveX14", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR164_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR165_NAME', 'TEXT', "SimulinkWaveY14", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR165_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR166_NAME', 'TEXT', "SimulinkWaveX15", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR166_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR167_NAME', 'TEXT', "SimulinkWaveY15", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR167_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	DevAddNode(_path // '.PARAMETERS:PAR168_NAME', 'TEXT', "SimulinkWaveX16", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR168_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR169_NAME', 'TEXT', "SimulinkWaveY16", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR169_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);

/* Simulink Algorithm Selector */
    DevAddNode(_path // '.PARAMETERS:PAR170_NAME', 'TEXT', "SimulinkSelector", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR170_VAL', 'NUMERIC', [0,0,0,0,0,0,0,0], *, _nid);

/* Lock rotation parameters */

    DevAddNode(_path // '.PARAMETERS:PAR171_NAME', 'TEXT', "LockRotStart", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR171_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR172_NAME', 'TEXT', "LockTreshStrength", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR172_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR173_NAME', 'TEXT', "LockTreshTime", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR173_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR174_NAME', 'TEXT', "LockTreshInterval", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR174_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR175_NAME', 'TEXT', "LockCycleTime", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR175_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR176_NAME', 'TEXT', "LockPertN1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR176_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR177_NAME', 'TEXT', "LockPertDeltaPhi1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR177_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR178_NAME', 'TEXT', "LockPertAmp1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR178_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR179_NAME', 'TEXT', "LockPertFreq1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR179_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR180_NAME', 'TEXT', "LockPertN2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR180_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR181_NAME', 'TEXT', "LockPertDeltaPhi2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR181_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR182_NAME', 'TEXT', "LockPertAmp2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR182_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR183_NAME', 'TEXT', "LockPertFreq2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR183_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR184_NAME', 'TEXT', "LockPertN3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR184_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR185_NAME', 'TEXT', "LockPertDeltaPhi3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR185_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR186_NAME', 'TEXT', "LockPertAmp3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR186_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR187_NAME', 'TEXT', "LockPertFreq3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR187_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR188_NAME', 'TEXT', "LockPertN4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR188_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR189_NAME', 'TEXT', "LockPertDeltaPhi4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR189_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR190_NAME', 'TEXT', "LockPertAmp4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR190_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR191_NAME', 'TEXT', "LockPertFreq4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR191_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR192_NAME', 'TEXT', "LockPertN5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR192_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR193_NAME', 'TEXT', "LockPertDeltaPhi5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR193_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR194_NAME', 'TEXT', "LockPertAmp5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR194_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR195_NAME', 'TEXT', "LockPertFreq5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR195_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR196_NAME', 'TEXT', "LockWaveform", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR196_VAL', 'NUMERIC', 0, *, _nid);

/* Mode Control Parameters */

    DevAddNode(_path // '.PARAMETERS:PAR197_NAME', 'TEXT', "ModeControl1Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR197_VAL', 'NUMERIC', -1, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR198_NAME', 'TEXT', "ModeControl1End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR198_VAL', 'NUMERIC', -1, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR199_NAME', 'TEXT', "ModeControl1GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR199_VAL', 'NUMERIC',zero(192, 0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR200_NAME', 'TEXT', "ModeControl1GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR200_VAL', 'NUMERIC',zero(192, 0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR201_NAME', 'TEXT', "ModeControl1IntGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR201_VAL', 'NUMERIC', zero(192, 0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR202_NAME', 'TEXT', "ModeControl1DerGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR202_VAL', 'NUMERIC', zero(192, 0.), *,_nid);

    DevAddNode(_path // '.PARAMETERS:PAR203_NAME', 'TEXT', "ModeControl2Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR203_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR204_NAME', 'TEXT', "ModeControl2End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR204_VAL', 'NUMERIC',-1, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR205_NAME', 'TEXT', "ModeControl2GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR205_VAL', 'NUMERIC',zero(192, 0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR206_NAME', 'TEXT', "ModeControl2GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR206_VAL', 'NUMERIC', zero(192,0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR207_NAME', 'TEXT', "ModeControl2IntGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR207_VAL', 'NUMERIC', zero(192, 0.),*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR208_NAME', 'TEXT', "ModeControl2DerGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR208_VAL', 'NUMERIC', zero(192, 0.),*, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR209_NAME', 'TEXT', "ModeControl3Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR209_VAL', 'NUMERIC',-1, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR210_NAME', 'TEXT', "ModeControl3End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR210_VAL', 'NUMERIC',-1, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR211_NAME', 'TEXT', "ModeControl3GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR211_VAL', 'NUMERIC', zero(192, 0.), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR212_NAME', 'TEXT', "ModeControl3GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR212_VAL', 'NUMERIC', zero(192, 0.), *,_nid);
    DevAddNode(_path // '.PARAMETERS:PAR213_NAME', 'TEXT', "ModeControl3IntGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR213_VAL', 'NUMERIC', zero(192, 0.), *,_nid);
    DevAddNode(_path // '.PARAMETERS:PAR214_NAME', 'TEXT', "ModeControl3DerGain", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR214_VAL', 'NUMERIC',zero(192, 0.), *, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR215_NAME', 'TEXT', "ModeControlDerivativeCutOff", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR215_VAL', 'NUMERIC',100., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR216_NAME', 'TEXT', "ModeControlSwitchLevel", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR216_VAL', 'NUMERIC', 0, *, _nid);



/* VirtualShell Input Parameters */

    DevAddNode(_path // '.PARAMETERS:PAR217_NAME', 'TEXT', "VirtualShellInput1Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR217_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR218_NAME', 'TEXT', "VirtualShellInput1End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR218_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR219_NAME', 'TEXT', "VirtualShellInput1GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR219_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR220_NAME', 'TEXT', "VirtualShellInput1GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR220_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR221_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR221_VAL', 'NUMERIC', 0, *, _nid);
	
    DevAddNode(_path // '.PARAMETERS:PAR222_NAME', 'TEXT', "VirtualShellInput2Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR222_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR223_NAME', 'TEXT', "VirtualShellInput2End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR223_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR224_NAME', 'TEXT', "VirtualShellInput2GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR224_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR225_NAME', 'TEXT', "VirtualShellInput2GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR225_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR226_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR226_VAL', 'NUMERIC', 0, *, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR227_NAME', 'TEXT', "VirtualShellInput3Start", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR227_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR228_NAME', 'TEXT', "VirtualShellInput3End", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR228_VAL', 'NUMERIC', -1,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR229_NAME', 'TEXT', "VirtualShellInput3GainMod", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR229_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR230_NAME', 'TEXT', "VirtualShellInput3GainPhs", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR230_VAL', 'NUMERIC',zero(192), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR231_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR231_VAL', 'NUMERIC', 0, *, _nid);

/* Rise times for rotating perturbations */

    DevAddNode(_path // '.PARAMETERS:PAR232_NAME', 'TEXT', "RotPertRiseTime1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR232_VAL', 'NUMERIC', 0,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR233_NAME', 'TEXT', "RotPertRiseTime2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR233_VAL', 'NUMERIC', 0,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR234_NAME', 'TEXT', "RotPertRiseTime3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR234_VAL', 'NUMERIC', 0,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR235_NAME', 'TEXT', "RotPertRiseTime4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR235_VAL', 'NUMERIC', 0,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR236_NAME', 'TEXT', "Decoupling", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR236_VAL', 'NUMERIC', diagonal(zero(192)+1.), *, _nid);

/* Toroidal control */
    DevAddNode(_path // '.PARAMETERS:PAR237_NAME', 'TEXT', "BtControlKp", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR237_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR238_NAME', 'TEXT', "BtControlKi", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR238_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR239_NAME', 'TEXT', "BtControlKd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR239_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR240_NAME', 'TEXT', "BtControlStart", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR240_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR241_NAME', 'TEXT', "BtControlEnd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR241_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR242_NAME', 'TEXT', "BtControlReferenceX", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR242_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR243_NAME', 'TEXT', "BtControlReferenceY", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR243_VAL', 'NUMERIC', 0. ,*, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR244_NAME', 'TEXT', "FControlKp", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR244_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR245_NAME', 'TEXT', "FControlKi", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR245_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR246_NAME', 'TEXT', "FControlKd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR246_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR247_NAME', 'TEXT', "FControlStart", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR247_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR248_NAME', 'TEXT', "FControlEnd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR248_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR249_NAME', 'TEXT', "FControlReferenceX", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR249_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR250_NAME', 'TEXT', "FControlReferenceY", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR250_VAL', 'NUMERIC', 0. ,*, _nid);

/* More rotating perturbation info */

    DevAddNode(_path // '.PARAMETERS:PAR251_NAME', 'TEXT', "RotPertRelPhase1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR251_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR252_NAME', 'TEXT', "RotPertRelPhase2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR252_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR253_NAME', 'TEXT', "RotPertRelPhase3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR253_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR254_NAME', 'TEXT', "RotPertRelPhase4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR254_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR255_NAME', 'TEXT', "RotPertThreshold1", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR255_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR256_NAME', 'TEXT', "RotPertThreshold2", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR256_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR257_NAME', 'TEXT', "RotPertThreshold3", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR257_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR258_NAME', 'TEXT', "RotPertThreshold4", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR258_VAL', 'NUMERIC', 0., *, _nid);
	

    DevAddNode(_path // '.PARAMETERS:PAR259_NAME', 'TEXT', "RotPertN5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR259_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR260_NAME', 'TEXT', "RotPertM5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR260_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR261_NAME', 'TEXT', "RotPertAmplitude5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR261_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR262_NAME', 'TEXT', "RotPertStart5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR262_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR263_NAME', 'TEXT', "RotPertEnd5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR263_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR264_NAME', 'TEXT', "RotPertAngVel5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR264_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR265_NAME', 'TEXT', "RotPertPhase5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR265_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR266_NAME', 'TEXT', "RotPertRiseTime5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR266_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR267_NAME', 'TEXT', "RotPertRelPhase5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR267_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR268_NAME', 'TEXT', "RotPertThreshold5", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR268_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR269_NAME', 'TEXT', "RotPertN6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR269_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR270_NAME', 'TEXT', "RotPertM6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR270_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR271_NAME', 'TEXT', "RotPertAmplitude6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR271_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR272_NAME', 'TEXT', "RotPertStart6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR272_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR273_NAME', 'TEXT', "RotPertEnd6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR273_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR274_NAME', 'TEXT', "RotPertAngVel6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR274_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR275_NAME', 'TEXT', "RotPertPhase6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR275_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR276_NAME', 'TEXT', "RotPertRiseTime6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR276_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR277_NAME', 'TEXT', "RotPertRelPhase6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR277_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR278_NAME', 'TEXT', "RotPertThreshold6", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR278_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR279_NAME', 'TEXT', "RotPertN7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR279_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR280_NAME', 'TEXT', "RotPertM7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR280_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR281_NAME', 'TEXT', "RotPertAmplitude7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR281_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR282_NAME', 'TEXT', "RotPertStart7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR282_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR283_NAME', 'TEXT', "RotPertEnd7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR283_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR284_NAME', 'TEXT', "RotPertAngVel7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR284_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR285_NAME', 'TEXT', "RotPertPhase7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR285_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR286_NAME', 'TEXT', "RotPertRiseTime7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR286_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR287_NAME', 'TEXT', "RotPertRelPhase7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR287_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR288_NAME', 'TEXT', "RotPertThreshold7", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR288_VAL', 'NUMERIC', 0., *, _nid);

    DevAddNode(_path // '.PARAMETERS:PAR289_NAME', 'TEXT', "RotPertN8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR289_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR290_NAME', 'TEXT', "RotPertM8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR290_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR291_NAME', 'TEXT', "RotPertAmplitude8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR291_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR292_NAME', 'TEXT', "RotPertStart8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR292_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR293_NAME', 'TEXT', "RotPertEnd8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR293_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR294_NAME', 'TEXT', "RotPertAngVel8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR294_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR295_NAME', 'TEXT', "RotPertPhase8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR295_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR296_NAME', 'TEXT', "RotPertRiseTime8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR296_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR297_NAME', 'TEXT', "RotPertRelPhase8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR297_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR298_NAME', 'TEXT', "RotPertThreshold8", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR298_VAL', 'NUMERIC', 0., *, _nid);

/* Ramp Down Flags */

    DevAddNode(_path // '.PARAMETERS:PAR299_NAME', 'TEXT', "RampDownFlags", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR299_VAL', 'NUMERIC', zero(192, 0.), *, _nid);
 
/* SPARE */ 
    DevAddNode(_path // '.PARAMETERS:PAR300_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR300_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR301_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR301_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR302_NAME', 'TEXT', "Spare", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR302_VAL', 'NUMERIC', 0., *, _nid);


/* MHD-Br parameters */
    DevAddNode(_path // '.PARAMETERS:PAR303_NAME', 'TEXT', "BrMeasureRadius", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR303_VAL', 'NUMERIC', 0.507, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR304_NAME', 'TEXT', "M1N0SineExcluded", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR304_VAL', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR305_NAME', 'TEXT', "SidebandCorrectionRange", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR305_VAL', 'NUMERIC', zero(8, 0), *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR306_NAME', 'TEXT', "BtCutOff", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR306_VAL', 'NUMERIC', 100., *, _nid);

/* Flux control */
    DevAddNode(_path // '.PARAMETERS:PAR307_NAME', 'TEXT', "FluxControlKp", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR307_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR308_NAME', 'TEXT', "FluxControlKi", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR308_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR309_NAME', 'TEXT', "FluxControlKd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR309_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR310_NAME', 'TEXT', "FluxControlStart", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR310_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR311_NAME', 'TEXT', "FluxControlEnd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR311_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR312_NAME', 'TEXT', "FluxControlReferenceX", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR312_VAL', 'NUMERIC', 0. ,*, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR313_NAME', 'TEXT', "FluxControlReferenceY", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR313_VAL', 'NUMERIC', 0. ,*, _nid);


/* Q Control */

    DevAddNode(_path // '.PARAMETERS:PAR314_NAME', 'TEXT', "QControlStart", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR314_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR315_NAME', 'TEXT', "QControlEnd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR315_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR316_NAME', 'TEXT', "QControlReferenceX", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR316_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR317_NAME', 'TEXT', "QControlReferenceY", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR317_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR318_NAME', 'TEXT', "QControlKp", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR318_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR319_NAME', 'TEXT', "QControlKi", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR319_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR320_NAME', 'TEXT', "QControlKd", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR320_VAL', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR321_NAME', 'TEXT', "QControlDeltaTime", *, _nid);
    DevAddNode(_path // '.PARAMETERS:PAR321_VAL', 'NUMERIC', 0., *, _nid);


/* Additional Simulink Waveforms */
	
	for(_i = 0; _i < 16; _i++)
	{
		DevAddNode(_path // '.PARAMETERS:PAR'//trim(adjustl(2*_i+322))//'_NAME', 'TEXT', 'SimulinkWaveX'//trim(adjustl(17+_i)), *, _nid);
		DevAddNode(_path // '.PARAMETERS:PAR'//trim(adjustl(2*_i+322))//'_VAL', 'NUMERIC', [0.,0.5,1.], *, _nid);
		DevAddNode(_path // '.PARAMETERS:PAR'//trim(adjustl(2*_i+323))//'_NAME', 'TEXT', 'SimulinkWaveY'//trim(adjustl(17+_i)), *, _nid);
		DevAddNode(_path // '.PARAMETERS:PAR'//trim(adjustl(2*_i+323))//'_VAL', 'NUMERIC', [0.,0.,0.], *, _nid);
	}	

	DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 25,'VME_SERVER',getnci(_path, 'fullpath'), _nid);




/* User signals */
    for (_c = 1; _c <= 9; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 1) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    for (_c = 10; _c <= 99; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 2) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
    for (_c = 100; _c <= 256; _c++)
    {
        DevAddNode(_path // '.SIGNALS:USER_' // TEXT(_c, 3) , 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddNode(_path // ':VERSION', 'TEXT', *, *, _nid);

    DevAddEnd();
}
