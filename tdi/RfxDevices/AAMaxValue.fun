public fun AAMaxValue(in _window, in _control, in _config)
{
    if(_window == 0.5)
    {
	if(_config == 'Series')
	{
	    if(_control == 'Current')
		return(2.7E3);
	    if(_control == 'Voltage')
		return(8.125E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
	if(_config == 'Parallel')
	{
	    if(_control == 'Current')
		return(1.35E3);
	    if(_control == 'Voltage')
		return(16.25E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
	if(_config == 'Independent')
	{
	    if(_control == 'Current')
		return(1.35E3);
	    if(_control == 'Voltage')
		return(8.125E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
    }
    if(_window == 5.)
    {
	if(_config == 'Series')
	{
	    if(_control == 'Current')
		return(2.7E3);
	    if(_control == 'Voltage')
		return(6.25E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
	if(_config == 'Parallel')
	{
	    if(_control == 'Current')
		return(1.35E3);
	    if(_control == 'Voltage')
		return(12.5E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
	if(_config == 'Independent')
	{
	    if(_control == 'Current')
		return(1.35E3);
	    if(_control == 'Voltage')
		return(6.25E3);
	    if(_control == 'OpenLoop')
		return(10.);
	}
    }
    return (0);
}

