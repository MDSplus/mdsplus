public fun TR32HWStartStore(in _nid, in _board_id, in _pts)
{
/* Initialize Library if the first time */
    	if_error(_TR32_initialized, (TR32->TR32_InitLibrary(); public _TR32_initialized = 1;));

	/* Open device */
	_handle = 0L;
	_status = TR32->TR32_Open(val(_board_id), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening TR32 device, board ID = "// _board_id);
		return(1);
	}


	/* Read actual number of PTS samples */
	_act_pts = 0L;

	TR32->TR32_Trg_GetActPostSamples(val(_handle), ref(_act_pts));



	/* Check if at stop state */
	if(_act_pts < _pts - 32)
	{
		TR32->TR32_Close(val(_handle));
		return(-1);
	}
	
	return (_handle);
}
