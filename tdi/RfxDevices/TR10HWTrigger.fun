public fun TR10HWTrigger(in _board_id)
{

	write(*, 'TR10HWTrigger', _board_id);
	return (1);


	_handle = 0L;
	_status = TR10->TR10_Open(val(long(_board_id)), _handle));
	if(_status != 0)
	{
		write(*, "Error opening TR10 device, board ID = "// _board_id);
		return(0);
	}

	TR10->TR10_Cmd_Trigger(val(_handle));

	TR10->TR10_Close(val(_handle));
	return (1);
}
