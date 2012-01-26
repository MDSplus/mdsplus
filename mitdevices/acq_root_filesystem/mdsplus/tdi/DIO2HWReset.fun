public fun DIO2HWReset(in _board_id)
{

/* Initialize Library if the first time */
    	if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1;));


	_handle = 0L;
	_status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO2 device, board ID = "// _board_id);
		return(0);
	}

	DIO2->DIO2_ResetIOError(val(_handle));

	DIO2->DIO2_Close(val(_handle));
	return (1);
}
