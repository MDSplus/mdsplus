public fun FR10HWStartStore(in _nid, in _board_id, in _pts)
{



/* Initialize Library if the first time */
    	if_error(_FR10_initialized, (FR10->FR10_InitLibrary(); public _FR10_initialized = 1;));

	/* Open device */
	_handle = 0L;
	_status = FR10->FR10_Open(val(_board_id), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening FR10 device, board ID = "// _board_id);
		return(1);
	}


	/* Read actual number of PTS samples */
	_act_pts = 0L;

	FR10->FR10_Trg_GetActPostSamples(val(_handle), ref(_act_pts));



	/* Check if at stop state */
	if(_act_pts < _pts - 32)
	{

		FR10->FR10_Close(val(_handle));
		return(-1);
	}
	
	return (_handle);
}
