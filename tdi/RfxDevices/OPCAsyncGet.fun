public fun OPCAsyncGet(in _group, in _tag)
{
    _status = MdsOpcClient->OPCEnableAsyncRead(_group, _tag);
     return(_status);
}
