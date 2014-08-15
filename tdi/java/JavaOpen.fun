FUN PUBLIC JavaOpen(in _exp, in _shot, optional _readOnly)
{
	TreeShr->TreeClose(val(0),val(0));
	if ( PRESENT( _readOnly ) )
		RETURN(TreeShr->TreeOpen(_exp, val(_shot), val( _readOnly )));
	else
		RETURN(TreeShr->TreeOpen(_exp, val(_shot), val(1)));
}