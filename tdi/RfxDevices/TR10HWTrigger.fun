public fun TR10HWTrigger(in _board_id)
{

/* Initialize Library if the first time */
    	if_error(_TR10_initialized, (TR10->TR10_InitLibrary(); public _TR10_initialized = 1;));


write(*, "HW TRIGGER");

	_handle = 0L;
	_status = TR10->TR10_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening TR10 device, board ID = "// _board_id);
		return(0);
	}

	TR10->TR10_Cmd_Trigger(val(_handle));
	wait(1);

	TR10->TR10_Close(val(_handle));
	return (1);
}
