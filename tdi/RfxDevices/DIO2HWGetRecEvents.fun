public fun DIO2HWGetRecEvents(in _nid, in _board_id)
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


/* Create _rec_times global variable if the first time */ 
    if_error(_DIO2_rec_times,(public _DIO2_rec_times = 0;));


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

	_event_count = byte(0);
	_status = DIO2->DIO2_ER_GetEventCount(val(_handle), ref(_event_count));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error getting event count in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error getting event count  in DIO2 device, board ID = "// _board_id);
		return(0);
	}

	_rec_events = [];
        _DIO2_rec_times1 = [];

	for(_i = 0; _i < _event_count; _i++)
	{
		_curr_event = byte(0);
		_curr_time = long(0);
		DIO2->DIO2_ER_GetEvent(val(_handle), ref(_curr_event), ref(_curr_time));
		_rec_events = [_rec_events, TimingEncodeEvent(_curr_event)];
		_DIO2_rec_times1 = [_DIO2_rec_times1, _curr_time * 1e-7];
	}

        public _DIO2_rec_times = _DIO2_rec_times1;


/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(_rec_events);
}
		
