public fun TimingEncodeEvent(in _event)
{
    TimingGetEvents(_event_names, _event_codes);
    for(_i = 0; _i < size(_event_codes); _i++)
    {
    	if(_event_codes[_i] == _event) 
	    return(_event_names[_i]);
    }  
    return (trim("UndefinedCode "//adjustl(''//_event)));
}

