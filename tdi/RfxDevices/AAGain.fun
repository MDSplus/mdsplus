public fun AAGain(in _control, in _config)
{
	if(_config == 'SERIES')
	{
	    if(_control == 'VOLTAGE')
		return(300.);
	    if(_control == 'CURRENT')
		return(812.5);
	    if(_control == 'OPEN LOOP')
		return(1.);
	}
	if(_config == 'PARALLEL')
	{
	    if(_control == 'VOLTAGE')
		return(150.);
	    if(_control == 'CURRENT')
		return(1625.);
	    if(_control == 'OPEN LOOP')
		return(1.);
	}
	if(_config == 'INDEPENDENT')
	{
	    if(_control == 'VOLTAGE')
		return(150.);
	    if(_control == 'CURRENT')
		return(812.5);
	    if(_control == 'OPEN LOOP')
		return(1.);
	}
    return (0);
}



