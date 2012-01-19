public fun DIO4HWGetPhaseCount(in _nid, in _board_id, in _channel)
{





	write(*, 'DIO4HWGetRecEvents');
    	if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}


	_phase1 = long(0);
	_phase2 = long(0);

	_status = DIO4->DIO4_TC_GetPhase1Count(val(_handle), val(byte(_channel + 1)), ref(_phase1));
	if(_status != 0)
	{
		write(*, "Error getting phase 1 count in DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_status = DIO4->DIO4_TC_GetPhase2Count(val(_handle), val(byte(_channel + 1)), ref(_phase2));
	if(_status != 0)
	{
		write(*, "Error getting phase 2 count in DIO4 device, board ID = "// _board_id);
		return(0);
	}


	_phase1 = _phase1 * 1e-7;
	_phase2 = _phase1 + _phase2 * 1e-7;

	_phases = [_phase1, _phase2];
        public _DIO4_phases = _phases;

/* Close device */
	DIO4->DIO4_Close(val(_handle));

    return(1);
}
		
