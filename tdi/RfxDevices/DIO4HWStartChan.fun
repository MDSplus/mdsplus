public fun DIO4HWStartChan(in _nid, in _board_id, in _chan_mask, in _synch)
{
write(*, 'DIO4HWStartChan');
/* Initialize Library if the first time */
    if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));

/* Open device */
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error opening DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}

/*Arm Channel */
	_status = DIO4->DIO4_Cmd_TimingChannelArm(val(_handle), val(byte(_chan_mask)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error arming channels in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error arming channels in DIO4 device, board ID = "// _board_id);
		return(0);
	}



/*Start Channels if no synch event */
if(_synch == "NO")

	{
		_status = DIO4->DIO4_Cmd_TimingChannelStart(val(_handle), val(byte(_chan_mask)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error starting channels in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error starting channels in DIO4 device, board ID = "// _board_id);
			return(0);
		}
	}


	
/* Close device */
	DIO4->DIO4_Close(val(_handle));

    return(1);
}





