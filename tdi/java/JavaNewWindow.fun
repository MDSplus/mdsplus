FUN PUBLIC JavaNewWindow(in _label, in _idx)
{
	RETURN(JavaMds->createWindow(_label, val(long(_idx))));
}