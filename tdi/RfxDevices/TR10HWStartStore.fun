public fun TR10HWStartStore(in _nid, in _board_id, in _pts)
{
/* Initialize Library if the first time */
    	if_error(_TR10_initialized, (TR10->TR10_InitLibrary(); public _TR10_initialized = 1;));

	/* Open device */
	_handle = 0L;
	_status = TR10->TR10_Open(val(_board_id), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening TR10 device, board ID = "// _board_id);
		return(1);
	}


	/* Read actual number of PTS samples */
	_act_pts = 0L;

	TR10->TR10_Trg_GetActPostSamples(val(_handle), ref(_act_pts));



	/* Check if at stop state */
	if(_act_pts < _pts - 32)
	{
	/*	DevLogErr(_nid, "TR10 device not at STOP state, board ID = "// _board_id);
		write(*, "TR10 device not at STOP state, board ID = "// _board_id);
		write(*, _act_pts, _pts);*/
		TR10->TR10_Close(val(_handle));
		return(-1);
	}
	
	return (_handle);
}
