public fun OPCDisconnect()
{
    _status = MdsOpcClient->OPCDisconnect();
    return(_status);
}
