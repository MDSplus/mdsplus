public fun OPCDisconnect(in _serverHandle)
{
    _status = opcAccess->OPCDisconnect( val( _serverHandle ) );
    return(_status);
}
