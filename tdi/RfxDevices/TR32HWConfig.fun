public fun TR32HWConfig(in _board_id)
{

/* Private constant definition */

	private _TR32_TRG_SOURCE_INTERNAL =		byte(0x0);
	private _TR32_TRG_SOURCE_PXI =			byte(0x1);
	private _TR32_TRG_SOURCE_PXI_STAR =		byte(0x2);
	private _TR32_TRG_SOURCE_EXTERNAL =		byte(0x3);
	private _TR32_TRG_NO_PXI_OUT =			byte(0x7);
	private _TR32_TRG_EXT_OUT_OFF =			byte(0x0);
	private _TR32_TRG_EXT_OUT_ON =			byte(0x1);
	private _TR32_TRG_RISING_EDGE =			byte(0x0);
	private _TR32_TRG_FALLING_EDGE =		byte(0x1);
	private _TR32_TRG_TERMINATION_ON =		byte(0x1);
	private _TR32_TRG_TERMINATION_OFF=		byte(0x0);
	private _TR32_TRG_SYNCHRONOUS =			byte(0x1);
	private _TR32_TRG_ASYNCHRONOUS =		byte(0x0);

	private _TR32_CLK_SOURCE_INTERNAL =		byte(0x0);
	private _TR32_CLK_SOURCE_PXI	=		byte(0x1);
	private _TR32_CLK_SOURCE_EXTERNAL =		byte(0x2);
	private _TR32_CLK_NO_EXT_CLOCK	=		byte(0x0);
	private _TR32_CLK_PXI_TRIG7		=	byte(0x1);
	private _TR32_CLK_DIVIDE	=		byte(0x0);
	private _TR32_CLK_SUB_SAMPLE	=		byte(0x1);
	private _TR32_CLK_RISING_EDGE	=		byte(0x0);
	private _TR32_CLK_FALLING_EDGE	=		byte(0x1);
	private _TR32_CLK_TERMINATION_OFF	=	byte(0x0);
	private _TR32_CLK_TERMINATION_ON	=	byte(0x1);


/* Initialize Library if the first time */
    _first = 0;
    if_error(_TR32_initialized, (TR32->TR32_InitLibrary(); public _TR32_initialized = 1; _first = 1;));


/* Open device */
	_handle = 0L;
	_status = TR32->TR32_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening TR32 device, board ID = "// _board_id);
		return(0);
	}


/* Reset module */
	TR32->TR32_Reset(val(_handle));


/* Set clock functions on external clock */
	_clock_source = byte(_TR32_CLK_SOURCE_EXTERNAL);
	_clock_div_mode = byte(_TR32_CLK_SUB_SAMPLE);
	TR32->TR32_Clk_SetClock(val(_handle), val(_clock_source), val(_TR32_CLK_NO_EXT_CLOCK),
		val(_clock_div_mode), val(_TR32_CLK_RISING_EDGE), val(_TR32_TRG_TERMINATION_OFF), val(long(1)));
		  
/* Set Trigger function */
	_trig_mode =_TR32_TRG_SOURCE_EXTERNAL;


	TR32->TR32_Trg_SetTrigger(val(_handle), val(_trig_mode), val(0B), val(0B),
		val(_TR32_TRG_EXT_OUT_ON), val(_TR32_TRG_RISING_EDGE), val(_TR32_TRG_TERMINATION_ON),
		val(_TR32_TRG_SYNCHRONOUS), val(0B), val(0W));
	


/* Close device */
	TR32->TR32_Close(val(_handle));

    return(1);
}
