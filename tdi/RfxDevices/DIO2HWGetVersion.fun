public fun DIO2HWGetVersion(in _board_id)
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

	private _DIO2_IO_CONNECTION_SOURCE_REGISTER	=		0x00;
	private _DIO2_IO_CONNECTION_SOURCE_BUFFER	=		0x01;
	private _DIO2_IO_CONNECTION_SOURCE_PXI_TRIGGER=		0x02;
	private _DIO2_IO_CONNECTION_SOURCE_TIMING	=		0x03;

	private _DIO2_IO_CONNECTION_SIDE_FRONT		=		0x00;
	private _DIO2_IO_CONNECTION_SIDE_REAR		=		0x01;
	private _DIO2_IO_CONNECTION_TERMINATION_ON	=		0x01;
	private _DIO2_IO_CONNECTION_TERMINATION_OFF	=		0x00;
	private _DIO2_IO_INT_ENABLE	=	0x1;
	private _DIO2_IO_INT_DISABLE	=	0x0;



	write(*, 'DIO2HWGetVersion', _board_id);


/* Initialize Library if the first time */
    _first = 0;
    if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1; _first = 1;));


/* Open device */
    _handle = 0L;
    _status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
    if(_status != 0)
    {
	write(*, "Error opening DIO2 device, board ID = ", _board_id);
	return("Errore");
    }

    _version = "    ";
    _status = DIO2->DIO2_GetVersion(val(long(_board_id)), ref(_version));
    if(_status != 0)
    {
	write(*, "Error reading DIO2 firmware version");
	return("Errore");
    }

 /* Close device */
    DIO2->DIO2_Close(val(_handle));

    return (_version);
}
