public fun TimingGetEventTime(in _event)
{
    _event_names = if_error(data(\TIMING_SUPERVISOR:EVENT_NAMES),return(0));
    _event_times = if_error(data(\TIMING_SUPERVISOR:EVENT_TIMES), return(0));
    for(_i = 0; _i < size(_event_names); _i++)
    {
	if(_event_names[_i] == _event)
	    return (compile(_event_times[_i]));
    }
    return (0);

}
