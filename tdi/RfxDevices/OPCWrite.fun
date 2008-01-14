public fun OPCWrite(in _serverHandle, in _groupName, in _item, in _value, optional in _nElem)
{
	if (present(_nElem))
		return ( opcAccess->OPCWrite( val( _serverHandle ), _groupName, _item, _value, val(_nElem)));
	else
		return ( opcAccess->OPCWrite( val( _serverHandle ), _groupName, _item, _value, val(0)));
}
