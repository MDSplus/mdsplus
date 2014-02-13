public fun TimingGetEventTime(in _event)
{
    _event_ok = 1;
    _event_names = if_error(data(\TIMING_SUPERVISOR:EVENT_NAMES),_event_ok = 0);
    _event_times = if_error(data(\TIMING_SUPERVISOR:EVENT_TIMES),_event_ok = 0);

    if(!_event_ok) return (0D0);
    if(!(size(_event_names) == size(_event_times)))
       return (-HUGE(0.));	
   for(_i = 0; _i < size(_event_names); _i++)
    {
		if(_event_names[_i] == _event)
	    /*return (compile('ft_float(' // _event_times[_i]//')'));*/
 	    return (compile( _event_times[_i])); 
   }
    return (HUGE(0.));
}

