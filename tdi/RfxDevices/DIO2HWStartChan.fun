public fun DIO2HWStartChan(in _nid, in _board_id, in _chan_mask, in _synch_event)
{
/* Initialize Library if the first time */
    if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1;));

/* Open device */
	_handle = 0L;
	_status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error opening DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error opening DIO2 device, board ID = "// _board_id);
		return(0);
	}

/*Arm Channel */
	_status = DIO2->DIO2_Cmd_TimingChannelArm(val(_handle), val(byte(_chan_mask)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error arming channels in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error arming channels in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/*Start Channels if no synch event */
	if(_synch_event == 0)
	{
		_status = DIO2->DIO2_Cmd_TimingChannelStart(val(_handle), val(byte(_chan_mask)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error starting channels in DIO2 device, board ID = "// _board_id);
			else
				write(*, "Error starting channels in DIO2 device, board ID = "// _board_id);
			return(0);
		}
	}
	
/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(1);
}





