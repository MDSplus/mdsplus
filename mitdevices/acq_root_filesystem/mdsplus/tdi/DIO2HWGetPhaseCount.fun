public fun DIO2HWGetPhaseCount(in _nid, in _board_id, in _channel)
{

	private _DIO2_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO2_CLOCK_SOURCE_TIMING_HIGHWAY =	0x3;
	private _DIO2_CLOCK_SOURCE_RISING_EDGE	=	0x0;
	private _DIO2_ER_START_EVENT_INT_DISABLE	= 0x0;
	private _DIO2_TC_NO_TRIGGER		=			0x00;
	private _DIO2_TC_IO_TRIGGER_RISING	=		0x01;
	private _DIO2_TC_IO_TRIGGER_FALLING	=		0x02;
	private _DIO2_TC_TIMING_EVENT		=		0x03;
	private _DIO2_TC_GATE_DISABLED	=			0x00;
	private _DIO2_TC_INTERRUPT_DISABLE	=		0x00;
	private _DIO2_TC_SOURCE_FRONT_REAR	=		0x01;
	private _DIO2_TC_CYCLIC			=			0x1;
	private _DIO2_EC_GENERAL_TRIGGER =			0x00;
	private _DIO2_EC_START_TRIGGER		=		0x01;
	private _DIO2_EC_STOP_TRIGGER		=		0x02;




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

	_phase1 = long(0);
	_phase2 = long(0);

	DIO2->DIO2_TC_GetPhase1Count(val(_handle), val(byte(_channel + 1)), ref(_phase1));
	DIO2->DIO2_TC_GetPhase2Count(val(_handle), val(byte(_channel + 1)), ref(_phase2));

	_phase1 = _phase1 * 1e-7;
	_phase2 = _phase1 + _phase2 * 1e-7;

	_phases = [_phase1, _phase2];


/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(_phases);
}
		
