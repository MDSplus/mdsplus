public fun OPCErrorMessage()
{
	_msg = "";
    _status = MdsOpcClient->OPCErrorMessage(xd(_msg));
    return(_msg);
}
