public fun OPCAsyncGet(in _group, in _tag)
{
    _status = MdsOpcClient->OPCEnableAsyncRead(_group, _tag, VAL(0), _group, VAL(sizeof(_group)));
/*
    _status = MdsOpcClient->OPCEnableAsyncRead(_group, _tag, 0, _group);
*/
     return(_status);
}
