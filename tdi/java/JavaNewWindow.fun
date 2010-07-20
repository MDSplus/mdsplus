FUN PUBLIC JavaNewWindow(in _label, in _idx, optional _live_update)
{
	if(present(_live_update))
	    RETURN(JavaMds->createWindow(_label, val(long(_idx)), val(long(_live_update))));
	else
	    RETURN(JavaMds->createWindow(_label, val(long(_idx)), val(0)));


}
