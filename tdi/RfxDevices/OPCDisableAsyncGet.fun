public fun OPCDisableAsyncGet(in _group, in _tag)
{
    _status = MdsOpcClient->OPCDisableAsyncRead(_group, _tag);
     return(_status);
}
