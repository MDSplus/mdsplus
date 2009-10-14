public fun PELLETErrorMessage()
{
	_msg = repeat(" ", 256);
	Pellet->PelletErrorMsg(ref(_msg), 256);
	return( _msg );
}
