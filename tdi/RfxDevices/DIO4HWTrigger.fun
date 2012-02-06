public fun DIO4HWTrigger(in _nid, in _board_id, in _channel_mask)
{
	write(*, 'DIO4HWTrigger', _board_id, _channel_mask);


/* Initialize Library if the first time */
    	if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));


	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_status = DIO4->DIO4_Cmd_TimingChannelTrigger(val(_handle), val(byte(_channel_mask)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error software trigger generation in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error software trigger generation in DIO4 device, board ID = "// _board_id);
		return(0);
	}

	DIO4->DIO4_Close(val(_handle));
	return (1);
}
