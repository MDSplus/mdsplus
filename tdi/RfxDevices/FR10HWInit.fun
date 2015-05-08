public fun FR10HWInit(in _nid, in _board_id, in _clock_div, in _pts, in _ext_trig, in _trig_edge)
{



/* Private constant definition */

	private _FR10_TRG_SOURCE_INTERNAL =		byte(0x0);
	private _FR10_TRG_SOURCE_PXI =			byte(0x1);
	private _FR10_TRG_SOURCE_PXI_STAR =		byte(0x2);
	private _FR10_TRG_SOURCE_EXTERNAL =		byte(0x3);
	private _FR10_TRG_NO_PXI_OUT =			byte(0x7);
	private _FR10_TRG_EXT_OUT_OFF =			byte(0x0);
	private _FR10_TRG_EXT_OUT_ON =			byte(0x1);
	private _FR10_TRG_RISING_EDGE =			byte(0x0);
	private _FR10_TRG_FALLING_EDGE =		byte(0x1);
	private _FR10_TRG_TERMINATION_ON =		byte(0x1);
	private _FR10_TRG_TERMINATION_OFF=		byte(0x0);
	private _FR10_TRG_SYNCHRONOUS =			byte(0x0);
	private _FR10_TRG_ASYNCHRONOUS =		byte(0x1);

	private _FR10_CLK_SOURCE_INTERNAL =		byte(0x0);
	private _FR10_CLK_SOURCE_PXI	=		byte(0x1);
	private _FR10_CLK_SOURCE_EXTERNAL =		byte(0x2);
	private _FR10_CLK_NO_EXT_CLOCK	=		byte(0x0);
	private _FR10_CLK_PXI_TRIG7	=		byte(0x1);
	private _FR10_CLK_DIVIDE	=		byte(0x0);
	private _FR10_CLK_SUB_SAMPLE	=		byte(0x1);
	private _FR10_CLK_RISING_EDGE	=		byte(0x0);
	private _FR10_CLK_FALLING_EDGE	=		byte(0x1);
	private _FR10_CLK_TERMINATION_OFF	=	byte(0x0);
	private _FR10_CLK_TERMINATION_ON	=	byte(0x1);


/* Initialize Library if the first time */
    if_error(_FR10_initialized, (FR10->FR10_InitLibrary(); public _FR10_initialized = 1;));
	

/* Open device */
	_handle = 0L;
	_status = FR10->FR10_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening FR10 device, board ID = "// _board_id);
		return(0);
	}

/* Reset module */
	FR10->FR10_Reset(val(_handle));


/* Set clock functions */
	if(_clock_div == 0) /*_clock_div == 0 means external clock */
		_clock_source = _FR10_CLK_SOURCE_EXTERNAL;
	else
		_clock_source = _FR10_CLK_SOURCE_INTERNAL;


	FR10->FR10_Clk_SetClockMode(val(_handle), val(_clock_source), val(_FR10_CLK_NO_EXT_CLOCK),
		val(_FR10_CLK_DIVIDE), val(_FR10_CLK_RISING_EDGE), val(_FR10_CLK_TERMINATION_OFF), val(long(_clock_div)));
		  
/* Set Trigger function */
	if(_ext_trig)
	    _trig_mode =_FR10_TRG_SOURCE_EXTERNAL;
	else
	    _trig_mode =_FR10_TRG_SOURCE_INTERNAL;


	if(_trig_edge)
		_trig_edge_mode = _FR10_TRG_FALLING_EDGE;
	else
		_trig_edge_mode = _FR10_TRG_RISING_EDGE;

	FR10->FR10_Trg_SetTrigger(val(_handle), val(_trig_mode), val(0B), val(_FR10_TRG_NO_PXI_OUT),
		val(_FR10_TRG_EXT_OUT_ON), val(_trig_edge_mode), val(_FR10_TRG_TERMINATION_ON),
		val(_FR10_TRG_SYNCHRONOUS));

/* Set Post Trigger Samples */
write(*, "PTS: " // _pts);
	FR10->FR10_Trg_SetPostSamples(val(_handle), val(long(_pts)));


/* Start sampling */
	FR10->FR10_Cmd_StartSampling(val(_handle));





/* Close device */
	FR10->FR10_Close(val(_handle));

    return(1);
}
