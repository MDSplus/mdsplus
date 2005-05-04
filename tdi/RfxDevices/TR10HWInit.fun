public fun TR10HWInit(in _nid, in _board_id, in _clock_div, in _pts, in _ext_trig, in _trig_edge)
{

/* Private constant definition */

	private _TR10_TRG_SOURCE_INTERNAL =		byte(0x0);
	private _TR10_TRG_SOURCE_PXI =			byte(0x1);
	private _TR10_TRG_SOURCE_PXI_STAR =		byte(0x2);
	private _TR10_TRG_SOURCE_EXTERNAL =		byte(0x3);
	private _TR10_TRG_NO_PXI_OUT =			byte(0x7);
	private _TR10_TRG_EXT_OUT_OFF =			byte(0x0);
	private _TR10_TRG_EXT_OUT_ON =			byte(0x1);
	private _TR10_TRG_RISING_EDGE =			byte(0x0);
	private _TR10_TRG_FALLING_EDGE =		byte(0x1);
	private _TR10_TRG_TERMINATION_ON =		byte(0x1);
	private _TR10_TRG_TERMINATION_OFF=		byte(0x0);
	private _TR10_TRG_SYNCHRONOUS =			byte(0x0);
	private _TR10_TRG_ASYNCHRONOUS =		byte(0x1);

	private _TR10_CLK_SOURCE_INTERNAL =		byte(0x0);
	private _TR10_CLK_SOURCE_PXI	=		byte(0x1);
	private _TR10_CLK_SOURCE_EXTERNAL =		byte(0x2);
	private _TR10_CLK_NO_EXT_CLOCK	=		byte(0x0);
	private _TR10_CLK_PXI_TRIG7		=	byte(0x1);
	private _TR10_CLK_DIVIDE	=		byte(0x0);
	private _TR10_CLK_SUB_SAMPLE	=		byte(0x1);
	private _TR10_CLK_RISING_EDGE	=		byte(0x0);
	private _TR10_CLK_FALLING_EDGE	=		byte(0x1);
	private _TR10_CLK_TERMINATION_OFF	=	byte(0x0);
	private _TR10_CLK_TERMINATION_ON	=	byte(0x1);


/* Initialize Library if the first time */
    if_error(_TR10_initialized, (TR10->TR10_InitLibrary(); public _TR10_initialized = 1;));
	

/* Open device */
	_handle = 0L;
	_status = TR10->TR10_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening TR10 device, board ID = "// _board_id);
		return(0);
	}

/* Reset module */
	TR10->TR10_Reset(val(_handle));


/* Set clock functions */
	if(_clock_div == 0) /*_clock_div == 0 means external clock */
		_clock_source = byte(_TR10_TRG_SOURCE_EXTERNAL);
	else
		_clock_source = byte(_TR10_TRG_SOURCE_INTERNAL);


	TR10->TR10_Clk_SetClockMode(val(_handle), val(_clock_source), val(_TR10_CLK_NO_EXT_CLOCK),
		val(0B), val(_TR10_CLK_RISING_EDGE), val(_TR10_CLK_TERMINATION_OFF), val(long(_clock_div)));
		  
/* Set Trigger function */
	if(_ext_trig)
	    _trig_mode =_TR10_TRG_SOURCE_EXTERNAL;
	else
	    _trig_mode =_TR10_TRG_SOURCE_INTERNAL;

	



	TR10->TR10_Trg_SetTrigger(val(_handle), val(_trig_mode), val(0B), val(0B),
		val(_TR10_TRG_EXT_OUT_ON), val(byte(_trig_edge)), val(_TR10_TRG_TERMINATION_ON),
		val(_TR10_TRG_SYNCHRONOUS), val(0B));

/* Set Post Trigger Samples */
	TR10->TR10_Trg_SetPostSamples(val(_handle), val(long(_pts)));


/* Start sampling */
	TR10->TR10_Cmd_StartSampling(val(_handle));





/* Close device */
	TR10->TR10_Close(val(_handle));

    return(1);
}
