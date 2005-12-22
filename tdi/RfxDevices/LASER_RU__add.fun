public fun LASER_RU__add(in _path, out _nidout)
{
    DevAddStart(_path, 'LASER_RU', 62, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
	DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':RS232_PORT',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE',   'TEXT', *, *, _nid);
    DevAddNode(_path // ':NUM_SHOTS',   'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':ENERGY',      'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':VOLTAGE_OSC', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':VOLTAGE_A_1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':VOLTAGE_A_2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':VOLTAGE_A_3', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BALANCE_P_1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BALANCE_P_2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':BALANCE_P_3', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_AMP',    'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_PULS_1', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_PULS_2', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_PULS_3', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_PULS_4', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_FIRE',   'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':CALIBRATION', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KOSC', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KAMP_1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KAMP_2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KAMP_3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KBAL_1', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KBAL_2', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATION:KBAL_3', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':OSC_PAR',    'NUMERIC', compile('NINT ( '// _path //':VOLTAGE_OSC * 4.095 / ( '//_path //':CALIBRATION.KOSC * 5.) )' ), *, _nid);
    DevAddNode(_path // ':AMP_PAR_1',  'NUMERIC', compile('NINT ( '// _path //':VOLTAGE_A_1 * 4.095 / ( '//_path //':CALIBRATION.KAMP_1 * 5.) )' ), *, _nid);
    DevAddNode(_path // ':AMP_PAR_2',  'NUMERIC', compile('NINT ( '// _path //':VOLTAGE_A_2 * 4.095 / ( '//_path //':CALIBRATION.KAMP_2 * 5.) )' ), *, _nid);
    DevAddNode(_path // ':AMP_PAR_3',  'NUMERIC', compile('NINT ( '// _path //':VOLTAGE_A_3 * 4.095 / ( '//_path //':CALIBRATION.KAMP_3 * 5.) )' ), *, _nid);
    DevAddNode(_path // ':PBAL_PAR_1', 'NUMERIC', compile('NINT ( '// _path //':BALANCE_P_1 * 4.095 / ( '//_path //':CALIBRATION.KBAL_1 * 10.) )' ), *, _nid);
    DevAddNode(_path // ':PBAL_PAR_2', 'NUMERIC', compile('NINT ( '// _path //':BALANCE_P_2 * 4.095 / ( '//_path //':CALIBRATION.KBAL_2 * 10.) )' ), *, _nid);
    DevAddNode(_path // ':PBAL_PAR_3', 'NUMERIC', compile('NINT ( '// _path //':BALANCE_P_3 * 4.095 / ( '//_path //':CALIBRATION.KBAL_3 * 10.) )' ), *, _nid);
    DevAddNode(_path // ':DTIME1_PAR_1', 'NUMERIC',  compile('NINT( 4000E3 * ('//_path //':DELAY_PULS_1 +'// _path //':DELAY_PULS_2 +'//_path //':DELAY_PULS_3) )'), *, _nid);
    DevAddNode(_path // ':DTIME1_PAR_2', 'NUMERIC',  compile('NINT( 4000E3 * ('//_path//':DELAY_PULS_1 +'//_path//':DELAY_PULS_2 +'//_path//':DELAY_PULS_3 +'//_path//':DELAY_PULS_4) )' ), *, _nid);
    DevAddNode(_path // ':DTIME1_PAR_3', 'NUMERIC',  compile('NINT( 4000E3 * ('//_path//':DELAY_PULS_1) )' ), *, _nid);
    DevAddNode(_path // ':DTIME1_PAR_4', 'NUMERIC',  compile('NINT( 4000E3 * ('//_path//':DELAY_PULS_1 +'//_path//':DELAY_PULS_2) )' ), *, _nid);
    DevAddNode(_path // ':DTIME1_PAR_5', 'NUMERIC',  compile('NINT( 100000 * '//_path//':DELAY_AMP )' ), *, _nid);
    DevAddNode(_path // ':DF1M_PAR_1',  'NUMERIC',  compile('NINT( 1000.  * ('//_path//':DELAY_FIRE) )' ), *, _nid);
    DevAddNode(_path // ':DF1M_PAR_2',  'NUMERIC',  compile('NINT( 1000E3 * ('//_path//':DELAY_FIRE) - INT('//_path//':DELAY_FIRE * 1000.) * 1000.)' ), *, _nid);
	DevAddNode(_path // ':STATUS_BITS', 'STRUCTURE', *, *, _nid);

    for (_s = 0; _s < 5; _s++)
    {
        _cs = _path // ':STATUS_BITS.BIT_' // TEXT(_s, 1);
		DevAddNode(_cs, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cs // ':PRE_SHOT', 'NUMERIC', *, *, _nid);
		DevAddNode(_cs // ':POST_SHOT', 'NUMERIC', *, *, _nid);
	}

    DevAddNode(_path // ':DATA',   'NUMERIC', 0, *, _nid);
    DevAddAction(_path//':INIT_ACTION',  'INIT',  'INIT', 50,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':RESET_ACTION', 'STORE', 'RESET', 70,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
