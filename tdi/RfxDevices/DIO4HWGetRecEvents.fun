public fun DIO4HWGetRecEvents(in _nid, in _board_id)
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


	_status = DIO4->DIO4_Cmd_StopEventRecorder(val(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error stopping event recorder in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error stopping event recorder in DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_event_count = word(0);
	_status = DIO4->DIO4_ER_GetEventCount(val(_handle), ref(_event_count));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error getting event count in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error getting event count  in DIO4 device, board ID = "// _board_id);
		return(0);
	}


	_rec_events = [];
        _rec_times  = [0];
	_found_event = 0;

	for(_i = 0; _i < _event_count; _i++)
	{
		_curr_event = byte(0);
		_curr_time = long(0);
		_status = DIO4->DIO4_ER_GetEvent(val(_handle), ref(_curr_event), ref(_curr_time));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error getting event in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error getting event in DIO4 device, board ID = "// _board_id);
		return(0);
	}
		_found_event = 1;

		_rec_events = [_rec_events, TimingEncodeEvent(_curr_event)];
		_rec_times = [_rec_times, _curr_time * 1e-7];
	}




	DIO4->DIO4_Close(val(_handle));


        public _DIO4_rec_events = _rec_events[_found_event : size(_rec_events) ];
        public _DIO4_rec_times  = _rec_times[_found_event : size(_rec_times) ];

	write(*, _DIO4_rec_events);
	write(*, _DIO4_rec_times);


	write(*, 'DIO4HWGetRecEvents OK');

    return(1);







}
