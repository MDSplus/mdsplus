public fun DIO2EncoderHWTrigger(in _nid, in _board_id, in _event)
{

write(*, "DIO ENCODER  HW TRIGGER");

/* Initialize Library if the first time */
    	if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1;));


	_handle = 0L;
	_status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO2 device, board ID = "// _board_id);
		return(0);
	}


write(*, 'Generate Event ', _event);
	_status = DIO2->DIO2_Cmd_GenerateEvent(val(_handle), val(byte(_event)));

	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error software event generation in DIO2Encoder device, board ID = "// _board_id);
		else
			write(*, "Error software event generation in DIO2Encoder device, board ID = "// _board_id);
		return(0);
	}

	DIO2->DIO2_Close(val(_handle));
	return (1);
}
