public fun TR32HWTrigger(in _board_id)
{

/* Initialize Library if the first time */
    	if_error(_TR32_initialized, (TR32->TR32_InitLibrary(); public _TR32_initialized = 1;));


write(*, "HW TRIGGER");

	_handle = 0L;
	_status = TR32->TR32_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening TR32 device, board ID = "// _board_id);
		return(0);
	}

	TR32->TR32_Cmd_Trigger(val(_handle));
	wait(1);

	TR32->TR32_Close(val(_handle));
	return (1);
}
