public fun OPCPut(in _serverHandle, in _item, in _value, optional in _nElem)
{
	if (present(_nElem))
		return ( opcAccess->OPCPut( val( _serverHandle ), _item, _value, val(_nElem)));
	else
		return ( opcAccess->OPCPut( val( _serverHandle ), _item, _value, val(0)));
}
