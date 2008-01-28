public fun TimingRegisterEventTime(in _event, in _time)
{


    _event_names = if_error(data(\TIMING_SUPERVISOR:EVENT_NAMES), []);
    _event_times = if_error(data(\TIMING_SUPERVISOR:EVENT_TIMES), []);


    if(!(size(_event_names) == size(_event_times)))
        return (-1);	

    _new_times = [];
    _found = 0;

    for(_i = 0; _i < size(_event_names); _i++)
    {
		if(_event_names[_i] == _event)
		{
			_found = 1;
			_new_times = [_new_times,_time];
		}
		else
			_new_times = [_new_times, _event_times[_i]];
    }
	

   if(!_found)
    {
		_event_names = [_event_names, _event];
		_nid = getnci(\TIMING_SUPERVISOR:EVENT_NAMES, 'NID_NUMBER',);
		_status = TreeShr->TreePutRecord(val(_nid),xd(_event_names),val(0));
		_new_times = [_new_times, _time];
    }
    _nid = getnci(\TIMING_SUPERVISOR:EVENT_TIMES, 'NID_NUMBER',);
    _status = TreeShr->TreePutRecord(val(_nid),xd(_new_times),val(0));


   return (0);
}

