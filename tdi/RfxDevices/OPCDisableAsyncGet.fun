public fun OPCDisableAsyncGet(in _group)
{
    _status = MdsOpcClient->OPCDisableAsyncRead(_group);
     return(_status);
}
