public fun OPCErrorMessage(in _serverHandle)
{
	_msg = "";
    _status = opcAccess->getErrorMessage(val( _serverHandle ), xd(_msg));
    return(_msg);
}
