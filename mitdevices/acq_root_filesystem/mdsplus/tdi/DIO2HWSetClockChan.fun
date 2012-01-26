public fun DIO2HWSetClockChan(in _nid, in _board_id, in _channel, in _frequency, in _duty_cycle)
{

	private _DIO2_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO2_CLOCK_SOURCE_TIMING_HIGHWAY =	0x3;
	private _DIO2_CLOCK_SOURCE_RISING_EDGE	=	0x0;
	private _DIO2_ER_START_EVENT_INT_DISABLE	= 0x0;
	private _DIO2_TC_NO_TRIGGER		=			0x00;
	private _DIO2_TC_GATE_DISABLED	=			0x00;
	private _DIO2_TC_INTERRUPT_DISABLE	=		0x00;



	_period = 1./_frequency;
	_tot_cycles = long(_period / 1E-7);
/*	_cycles_1 = long(_tot_cycles * _duty_cycle / 100.) + 1; Cesare */ 
/*	_cycles_2 = long(_tot_cycles - _cycles_1 + 2); Cesare
          */ 

	_cycles_1 = long(_tot_cycles * _duty_cycle / 100.) - 1; 
	if(_cycles_1 < 0) _cycles_1 = 0;

	_cycles_2 = long(_tot_cycles - _cycles_1 - 2); 
	if(_cycles_2 < 0) _cycles_2 = 0;

/*	_cycles = [_cycles_1, _cycles_2, 2,2]; Cesare*/

	_cycles = [_cycles_1, _cycles_2, 0, 0];

	_levels = [1,0,1,0];



/* Initialize Library if the first time */
    if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1;));


/* Open device */
	_handle = 0L;
	_status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error opening DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error opening DIO2 device, board ID = "// _board_id);
		return(0);
	}



/* No trigger */
	_status = DIO2->DIO2_TC_SetTrigger(val(_handle), val(byte(_channel + 1)), val(byte(_DIO2_TC_NO_TRIGGER)), val(byte(0)),
		val(byte(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/* No Gating */
	_status = DIO2->DIO2_TC_SetGate(val(_handle), val(byte(_channel + 1)), val(byte(_DIO2_TC_GATE_DISABLED)), val(byte(0)),
		val(byte(0)), val(byte(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/* Phase setting */
	_status = DIO2->DIO2_TC_SetPhaseSettings(val(_handle), val(byte(_channel + 1)), val(byte(0)), 
		val(byte(_DIO2_TC_INTERRUPT_DISABLE)), _levels);
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase in DIO2 device, board ID = "// _board_id);
		return(0);
	}


/* Timing setting */
/*	_status = DIO2->DIO2_TC_SetPhaseTiming(val(_handle), val(byte(_channel + 1)), _cycles, val(long(1)), val(long(1))); Cesare */
	_status = DIO2->DIO2_TC_SetPhaseTiming(val(_handle), val(byte(_channel + 1)), _cycles, val(long(0)), val(long(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase timing in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase timing in DIO2 device, board ID = "// _board_id);
		return(0);
	}
	

/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(1);
}

