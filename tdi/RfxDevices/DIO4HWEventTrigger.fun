public fun DIO4HWEventTrigger(in _nid, in _board_id, in _evCode)
{
	private _DIO4_TH_ASYNCHRONOUS  =  0;
	private _DIO4_TH_SYNCHRONOUS   =  1;
	private _DIO4_TH_OUTPUT_DISABLE  = 0;
	private _DIO4_TH_OUTPUT_ENABLE =  1;
	private _DIO4_TH_INT_DISABLE = 0; 
	private _DIO4_TH_INT_ENABLE =1;



	write(*, 'DIO4HWEventTrigger with event ', _evCode);

/* Initialize Library if the first time */
    if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));


	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}

/*
	_synch = byte(0);
	_out = byte(0);
	_int = byte(0);
	_status = DIO4->DIO4_TH_GetTimingHighway(val(_handle), ref(_synch), ref(_out), ref(_int));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error DIO4_TH_GetTimingHighway in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error DIO4_TH_GetTimingHighway in DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(byte(_DIO4_TH_SYNCHRONOUS)), 
	val(_DIO4_TH_OUTPUT_ENABLE), val(byte(_DIO4_TH_INT_DISABLE)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error DIO4_TH_SetTimingHighway in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error DIO4_TH_SetTimingHighway in DIO4 device, board ID = "// _board_id);
		return(0);
	}
*/

	_status = DIO4->DIO4_Cmd_GenerateEvent(val(_handle), val(byte(_evCode)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error software event generation in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error software event generation in DIO4 device, board ID = "// _board_id);
		return(0);
	}

/*
	_status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(_synch),  val(_out), val(_int));

	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error DIO4_TH_SetTimingHighway in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error DIO4_TH_SetTimingHighway in DIO4 device, board ID = "// _board_id);
		return(0);
	}
*/

	DIO4->DIO4_Close(val(_handle));
	return (1);
}
