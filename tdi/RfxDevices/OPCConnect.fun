public fun OPCConnect(in _name, in _updateRate)
{
	_status = opcAccess->OPCConnect( _name, val(_updateRate));
    return(_status);
}
