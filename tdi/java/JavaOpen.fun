FUN PUBLIC JavaOpen(in _exp, in _shot)
{
	TreeShr->TreeClose(0,0);
	RETURN(TreeShr->TreeOpen(_exp, val(_shot), val(1)));
}