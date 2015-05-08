FUN PUBLIC JavaClearWindow(in _label, in _idx)
{
	RETURN(JavaMds->clearWindow(_label, val(long(_idx))));
}