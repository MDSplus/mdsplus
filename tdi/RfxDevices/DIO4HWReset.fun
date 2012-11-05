public fun DIO4HWReset(in _nid, in _board_id)
{


	write(*, 'DIO4HWReset');
    	if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_status = DIO4->DIO4_Cmd_TimingChannelDisarm(val(_handle),val(byte(255)));

		DIO4->DIO4_ResetIOError(val(_handle));

		DIO4->DIO4_Reset(val(_handle));
	DIO4->DIO4_Close(val(_handle));
	return (1);
}
