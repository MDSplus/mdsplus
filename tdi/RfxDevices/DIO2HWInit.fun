public fun DIO2HWInit(in _nid, in _board_id, in _ext_clock, in _rec_event, in _synch_event)
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



	write(*, 'DIO2HWInit', _board_id, _ext_clock, _rec_event, _synch_event);
	return(1);

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

write(*, 'OPEN');

/* Reset module */
	DIO2->DIO2_Reset(val(_handle));

write(*, 'RESET');

/* Set clock functions */
	if(_ext_clock)
		_clock_source = byte(_DIO2_CLOCK_SOURCE_TIMING_HIGHWAY);
	else
		_clock_source = byte(_DIO2_CLOCK_SOURCE_INTERNAL);
		
	for(_c = 0; _c <= 8; _c++)
		_status = DIO2->DIO2_CS_SetClockSource(val(_handle), val(_clock_source), val(byte(_c)), val(byte(_DIO2_CLOCK_SOURCE_RISING_EDGE)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting clock source in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting clock source in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/* Set recorder start event and arm recorder */
	if(_rec_event != 0)
	{
		_status = DIO2->DIO2_ER_SetStartEvent(val(_handle), val(byte(_rec_event)), val(byte(_DIO2_START_EVENT_INT_DISABLE)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting recorder start event in DIO2 device, board ID = "// _board_id);
			else
				write(*, "Error setting recorder start event in DIO2 device, board ID = "// _board_id);
			return(0);
		}
		_status = DIO2->DIO2_CmdArmEventRecorder(val(_handle));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error arming recorder in DIO2 device, board ID = "// _board_id);
			else
				write(*, "Error setting arming recorder  in DIO2 device, board ID = "// _board_id);
			return(0);
		}
	}


/* Set synch event if defined */
	if(_synch_event != 0)
	{
		_status = DIO2->DIO2_EC_SetEventDecoder(val(_handle), val(byte(10)), val(byte(_synch_event)),
			val(byte(255)), val(byte(_DIO2_EC_GENERAL_TRIGGER)));

		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting recorder start event in DIO2 device, board ID = "// _board_id);
			else
				write(*, "Error setting recorder start event in DIO2 device, board ID = "// _board_id);
			return(0);
		}
	}		




/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(1);
}
		
