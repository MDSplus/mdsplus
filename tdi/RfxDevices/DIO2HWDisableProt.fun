public fun DIO2HWDisableProt( in _board_id)
{

	private _DIO2_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO2_CLOCK_SOURCE_TIMING_HIGHWAY =	0x1;
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
        private _DIO2_REG_IO_CONNECTION = 0x10;

/* Initialize Library if the first time */
    _first = 0;
    if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1; _first = 1;));


/* Open device */
    _handle = 0L;
    _status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
    if(_status != 0)
    {
	write(*, "Error opening DIO2 device, board ID = ", _board_id);
	return("Error");
    }

    for (_chan=0; _chan < 8; _chan++)
    {
        _dwValue=0L;
        _c = _chan*2+1;
        _dwRegister = ((_c - 1)*4)  + _DIO2_REG_IO_CONNECTION;
        DIO2->DIO2_Tst_ReadRegister(val(_handle), val(_dwRegister), ref(_dwValue));
        write(*, _chan, _c, _dwValue);
        _dwValue = _dwValue | (1 << 10); /* set IOERRDIS bit */
        DIO2->DIO2_Tst_WriteRegister(val(_handle), val(_dwRegister), val(_dwValue));
    }

/* Close device */
	DIO2->DIO2_Close(val(_handle));
	
    return(1);
}
