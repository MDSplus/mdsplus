public fun TR10HWStartStore(in _nid, in _board_id, in _pts)
{


	write(*, 'TR10HWStartStore', _board_id, _pts);
	return (123L);





	/* Open device */
	_handle = 0;
	_status = TR10->TR10_Open(val(_board_id), _handle);
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening TR10 device, board ID = "// _board_id);
		return(0);
	}


	/* Read actual number of PTS samples */
	_act_pts = 0L;

	TR10->TR10_Trg_GetActPostSamples(val(_handle), _act_pts);

	/* Check if at stop state */
	if(_act_pts < _pts)
	{
		DevLogErr(_nid, "TR10 device not at STOP state, board ID = "// _board_id);
		TR10->TR10_Close(val(_handle));
		return(0);
	}
	
	return (_handle);
}
