public fun AAGain(in _control, in _config)
{
	if(_config == 'Series')
	{
	    if(_control == 'Voltage')
		return(270.);
	    if(_control == 'Current')
		return(812.5);
	    if(_control == 'OpenLoop')
		return(1.);
	}
	if(_config == 'Parallel')
	{
	    if(_control == 'Voltage')
		return(135.);
	    if(_control == 'Current')
		return(1625.);
	    if(_control == 'OpenLoop')
		return(1.);
	}
	if(_config == 'Independent')
	{
	    if(_control == 'Voltage')
		return(135.);
	    if(_control == 'Current')
		return(812.5);
	    if(_control == 'OpenLoop')
		return(1.);
	}
    return (0);
}

