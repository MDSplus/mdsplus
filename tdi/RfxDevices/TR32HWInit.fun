public fun TR32HWInit(in _nid, in _board_id, in _ext_clock, in _clock_div, in _pts, in _ext_trig, in _trig_edge, in _clk_termination, in _ranges)
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
    if_error(_TR32_initialized, (write(*, 'INITIALIZE');TR32->TR32_InitLibrary(); public _TR32_initialized = 1;));
	

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


/* Set clock functions */
	if(_ext_clock) 
	{
		_clock_div_mode = byte(_TR32_CLK_SUB_SAMPLE);
		_clock_source = byte(_TR32_CLK_SOURCE_EXTERNAL);
	}
	else
	{
		_clock_div_mode = byte(_TR32_CLK_DIVIDE);
		_clock_source = byte(_TR32_CLK_SOURCE_INTERNAL);
	}


	if(_clk_termination)
		_clock_termination = _TR32_CLK_TERMINATION_ON;
	else
		_clock_termination = _TR32_CLK_TERMINATION_OFF;



write(*, 'Clock div: ', data(_clock_div)); 
write(*, 'Clock source: ', _clock_source); 
write(*, 'prova init '); 


	TR32->TR32_Clk_SetClock(val(_handle), val(_clock_source), val(_TR32_CLK_NO_EXT_CLOCK),
		val(_clock_div_mode), val(_TR32_CLK_RISING_EDGE), val(_clock_termination), val(long(data(_clock_div))));
		  
/* Set Trigger function */
	if(_ext_trig)
	    _trig_mode =_TR32_TRG_SOURCE_EXTERNAL;
	else
	    _trig_mode =_TR32_TRG_SOURCE_INTERNAL;

	if(_trig_edge)
		_trig_edge_mode = _TR32_TRG_FALLING_EDGE;
	else
		_trig_edge_mode = _TR32_TRG_RISING_EDGE;

write(*, 'TRIG_EDGE:', _trig_edge_mode);


	TR32->TR32_Trg_SetTrigger(val(_handle), val(_trig_mode), val(0B), val(0B),
		val(_TR32_TRG_EXT_OUT_ON), val(_trig_edge_mode), val(_TR32_TRG_TERMINATION_OFF),
		val(_TR32_TRG_SYNCHRONOUS), val(0B), val(0W));

/* Set Post Trigger Samples */
 write(*, 'PTS: ', _pts);

	TR32->TR32_Trg_SetPostSamples(val(_handle), val(long(_pts)));

/* Set Channel input range */ 
	for (_c = 1; _c <= 4; _c++)
{
		TR32->TR32_Rng_SetChannelRange(val(_handle), val(byte(_c)), val(word(_ranges[_c - 1]))); 
}	

/* Start sampling */
	TR32->TR32_Cmd_StartSampling(val(_handle));


/* Close device */
	TR32->TR32_Close(val(_handle));

    return(1);
}
