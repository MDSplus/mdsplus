public fun TimingDecodeEvent(in _event)
{
    TimingGetEvents(_event_names, _event_codes);
    for(_i = 0; _i < size(_event_names); _i++)
    {
    	if(_event_names[_i] == _event) 
	    return(_event_codes[_i]);
    }  
    return (0);
}

