public fun FR10HWTrigger(in _board_id)
{




/* Initialize Library if the first time */
    	if_error(_FR10_initialized, (FR10->FR10_InitLibrary(); public _FR10_initialized = 1;));



	_handle = 0L;
	_status = FR10->FR10_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening FR10 device, board ID = "// _board_id);
		return(0);
	}

	FR10->FR10_Cmd_Trigger(val(_handle));
	wait(1);

	FR10->FR10_Close(val(_handle));
	return (1);
}
