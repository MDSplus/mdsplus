public fun ABGain(in _control, in _config)
{
	if(_config == 'PARALLEL')
	{
	    if(_control == 'VOLTAGE')
		return(135.);
	    if(_control == 'CURRENT')
		return(812.5);
	    if(_control == 'OPEN LOOP')
		return(1.);
	}
	if(_config == 'INDEPENDENT')
	{
	    if(_control == 'VOLTAGE')
		return(135.);
	    if(_control == 'CURRENT')
		return(406.25);
	    if(_control == 'OPEN LOOP')
		return(1.);
	}
    return (0);
}



